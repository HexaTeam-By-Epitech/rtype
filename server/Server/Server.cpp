/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** Server.cpp
*/

#include "server/Server/Server.hpp"
#include <thread>
#include "Capnp/Messages/Messages.hpp"
#include "Capnp/Messages/Shared/SharedTypes.hpp"
#include "Capnp/NetworkMessages.hpp"
#include "NetworkFactory.hpp"
#include "common/ECS/Components/Enemy.hpp"
#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/IComponent.hpp"
#include "common/ECS/Components/Player.hpp"
#include "common/ECS/Components/Projectile.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECSWrapper/ECSWorld.hpp"
#include "common/Logger/Logger.hpp"
#include "server/Core/Clock/FrameTimer.hpp"
#include "server/Core/EventBus/EventBus.hpp"
#include "server/Core/ServerLoop/ServerLoop.hpp"
#include "server/Core/ThreadPool/ThreadPool.hpp"
#include "server/Events/GameEvent/GameEndedEvent.hpp"
#include "server/Events/GameEvent/GameEvent.hpp"
#include "server/Events/GameEvent/GameStartedEvent.hpp"
#include "server/Events/GameEvent/PlayerJoinedEvent.hpp"
#include "server/Events/GameEvent/PlayerLeftEvent.hpp"
#include "server/Game/Logic/GameLogic.hpp"
#include "server/Game/Logic/GameStateSerializer.hpp"
#include "server/Sessions/Session/Session.hpp"

Server::Server(uint16_t port, size_t maxClients) : _port(port), _maxClients(maxClients) {}

Server::~Server() {
    LOG_INFO("Server shutting down...");
    stop();
    deinitializeNetworking();
}

bool Server::initialize() {
    if (_initialized) {
        return true;
    }

    LOG_INFO("Initializing R-Type server with proper architecture...");

    // Initialize networking
    if (!initializeNetworking()) {
        LOG_ERROR("Failed to initialize networking");
        return false;
    }

    // Create network manager
    _networkManager = std::make_unique<ServerNetworkManager>(_port, _maxClients);
    LOG_INFO("✓ Network manager created");

    // Set packet handler
    _networkManager->setPacketHandler([this](HostNetworkEvent &event) { this->handlePacket(event); });
    LOG_INFO("✓ Packet handler set");

    // Start network thread
    if (!_networkManager->start()) {
        LOG_ERROR("Failed to start network manager");
        return false;
    }
    LOG_INFO("✓ Network thread started");

    // Create EventBus
    _eventBus = std::make_shared<server::EventBus>();
    LOG_INFO("✓ EventBus created");

    // Create SessionManager (track players)
    _sessionManager = std::make_shared<server::SessionManager>();
    LOG_INFO("✓ SessionManager created");

    // Create RoomManager (manage game instances)
    _roomManager = std::make_shared<server::RoomManager>();
    LOG_INFO("✓ RoomManager created");

    // Create default room
    _roomManager->createRoom("default");
    _defaultRoom = _roomManager->getRoom("default");
    LOG_INFO("✓ Default room created");

    // Create World with ECSWorld
    std::shared_ptr<ecs::wrapper::ECSWorld> ecsWorld = std::make_shared<ecs::wrapper::ECSWorld>();
    LOG_INFO("✓ ECSWorld created");

    // Create ThreadPool for parallel system execution (4 workers)
    std::shared_ptr<server::ThreadPool> threadPool = std::make_shared<server::ThreadPool>(4);
    threadPool->start();
    std::unique_ptr<server::GameLogic> gameLogic =
        std::make_unique<server::GameLogic>(ecsWorld, threadPool, _eventBus);
    LOG_INFO("✓ ThreadPool enabled with 4 workers");

    // Create ServerLoop with GameLogic and EventBus
    _gameLoop = std::make_unique<server::ServerLoop>(std::move(gameLogic), _eventBus);
    LOG_INFO("✓ Game loop created (implements IServerLoop)");

    // Initialize game loop (initializes GameLogic and all systems)
    if (!_gameLoop->initialize()) {
        LOG_ERROR("Failed to initialize game loop");
        return false;
    }
    LOG_INFO("✓ Game loop initialized");

    // Subscribe to game events
    _eventBus->subscribe<server::PlayerJoinedEvent>([](const server::PlayerJoinedEvent &event) {
        LOG_INFO("[EVENT] Player joined: ", event.getPlayerName(), " (ID: ", event.getPlayerId(), ")");
    });

    _eventBus->subscribe<server::PlayerLeftEvent>([](const server::PlayerLeftEvent &event) {
        LOG_INFO("[EVENT] Player left (ID: ", event.getPlayerId(), ")");
    });

    _eventBus->subscribe<server::GameStartedEvent>(
        [](const server::GameStartedEvent &) { LOG_INFO("[EVENT] Game started!"); });

    _eventBus->subscribe<server::GameEndedEvent>([](const server::GameEndedEvent &event) {
        LOG_INFO("[EVENT] Game ended. Reason: ", event.getReason());
    });

    LOG_INFO("✓ Event subscriptions registered");

    _initialized = true;
    LOG_INFO("✓ Initialization complete!");

    return true;
}

void Server::handlePacket(HostNetworkEvent &event) {
    // Dispatch based on event type
    if (event.type == NetworkEventType::DISCONNECT) {
        _handleDisconnect(event);
        return;
    }

    if (!event.packet || !event.peer) {
        return;
    }

    try {
        using namespace RType::Messages;

        // Get message type and dispatch to appropriate handler
        NetworkMessages::MessageType messageType = NetworkMessages::getMessageType(event.packet->getData());

        switch (messageType) {
            case NetworkMessages::MessageType::HANDSHAKE_REQUEST:
                _handleHandshakeRequest(event);
                break;

            case NetworkMessages::MessageType::C2S_PLAYER_INPUT:
                _handlePlayerInput(event);
                break;

            default:
                LOG_WARNING("Received unknown message type: ", static_cast<int>(messageType));
                break;
        }

    } catch (const std::exception &e) {
        LOG_ERROR("Error handling packet: ", e.what());
    }
}

void Server::_handleDisconnect(HostNetworkEvent &event) {
    if (!event.peer) {
        return;
    }

    // Fast O(1) lookup using reverse map
    auto it = _peerToSession.find(event.peer);
    if (it == _peerToSession.end()) {
        return;
    }

    std::string sessionId = it->second;
    std::shared_ptr<server::Session> session = _sessionManager->getSession(sessionId);
    uint32_t playerId = session ? session->getPlayerId() : 0;

    if (playerId == 0) {
        return;
    }

    LOG_INFO("Player ", playerId, " disconnected, cleaning up...");

    // Despawn player from game
    server::IGameLogic &gameLogic = _gameLoop->getGameLogic();
    gameLogic.despawnPlayer(playerId);

    // Remove from room
    _defaultRoom->leave(playerId);

    // Publish PLAYER_LEFT event
    _eventBus->publish(server::PlayerLeftEvent(playerId));

    // Clean up session and mappings
    _sessionManager->removeSession(sessionId);
    _sessionPeers.erase(sessionId);
    _peerToSession.erase(it);

    LOG_INFO("✓ Player ", playerId, " fully cleaned up");
}

void Server::_handleHandshakeRequest(HostNetworkEvent &event) {
    using namespace RType::Messages;

    // Parse connect request
    std::string playerName = NetworkMessages::parseConnectRequest(event.packet->getData());
    LOG_INFO("Player '", playerName, "' requesting to join...");

    // TODO: For production, add real authentication here:
    // if (!_sessionManager->getAuthService()->authenticate(username, password)) {
    //     sendAuthFailedMessage(event.peer);
    //     return;
    // }

    // Assign unique player ID
    static std::atomic<uint32_t> nextPlayerId{1000};
    uint32_t newPlayerId = nextPlayerId.fetch_add(1);

    // Create session for player
    std::string sessionId = "session_" + std::to_string(newPlayerId);
    std::shared_ptr<server::Session> session = _sessionManager->createSession(sessionId);
    session->setPlayerId(newPlayerId);
    session->setActive(true);

    // Track session to peer mapping for network communication
    _sessionPeers[sessionId] = event.peer;
    _peerToSession[event.peer] = sessionId;

    // Add player to matchmaking queue instead of joining room directly
    // This allows for proper match creation when enough players join
    _roomManager->addPlayerToMatchmaking(newPlayerId);
    LOG_INFO("Player ", newPlayerId, " added to matchmaking queue");

    // For now, also join default room for immediate play (skip matchmaking for dev)
    // In production, remove this and only join after match is found
    _defaultRoom->join(newPlayerId);
    LOG_INFO("Player ", newPlayerId, " joined room 'default' (dev mode)");

    // Publish PLAYER_JOINED event to EventBus
    _eventBus->publish(server::PlayerJoinedEvent(newPlayerId, playerName));

    // Spawn player in game logic
    server::IGameLogic &gameLogic = _gameLoop->getGameLogic();
    uint32_t entityId = gameLogic.spawnPlayer(newPlayerId, playerName);

    if (entityId == 0) {
        LOG_ERROR("Failed to spawn player ", newPlayerId);
        return;
    }

    // Create GameStart message with current tick
    S2C::GameStart gameStart;
    gameStart.yourEntityId = newPlayerId;
    gameStart.initialState.serverTick = _gameLoop->getCurrentTick();

    // Get ECS world to read spawned player entity
    std::shared_ptr<ecs::wrapper::ECSWorld> ecsWorld = _gameLoop->getECSWorld();
    if (ecsWorld) {
        // Serialize the spawned player entity
        ecs::wrapper::Entity entity = ecsWorld->getEntity(entityId);
        if (entity.has<ecs::Transform>()) {
            S2C::EntityState entityState = _serializeEntity(entity);
            gameStart.initialState.entities.push_back(entityState);
            LOG_DEBUG("Added player entity ", entityId, " to GameStart at pos (", entityState.position.x,
                      ", ", entityState.position.y, ")");
        }
    }

    // Serialize and send
    std::vector<uint8_t> gameStartPayload = gameStart.serialize();
    std::vector<uint8_t> gameStartPacket =
        NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_GAME_START, gameStartPayload);
    std::unique_ptr<IPacket> packet = createPacket(gameStartPacket, std::to_underlying(PacketFlag::RELIABLE));
    event.peer->send(std::move(packet), 0);

    LOG_INFO("✓ Player '", playerName, "' joined (Session: ", sessionId, ", Player ID: ", newPlayerId,
             ", Entity: ", entityId, ")");
}

void Server::_handlePlayerInput(HostNetworkEvent &event) {
    using namespace RType::Messages;

    std::vector<uint8_t> payload = NetworkMessages::getPayload(event.packet->getData());
    C2S::PlayerInput input = C2S::PlayerInput::deserialize(payload);

    // Process each action in the input
    int dx = 0;
    int dy = 0;
    bool shoot = false;

    for (const auto &action : input.actions) {
        int actionDx = 0;
        int actionDy = 0;
        bool actionShoot = false;
        _actionToInput(action, actionDx, actionDy, actionShoot);
        dx += actionDx;
        dy += actionDy;
        shoot = shoot || actionShoot;
    }

    // Find session from peer using fast reverse lookup
    uint32_t playerId = 0;
    auto it = _peerToSession.find(event.peer);
    if (it != _peerToSession.end()) {
        std::shared_ptr<server::Session> session = _sessionManager->getSession(it->second);
        if (session) {
            playerId = session->getPlayerId();
        }
    }

    if (playerId != 0) {
        server::IGameLogic &gameLogic = _gameLoop->getGameLogic();
        gameLogic.processPlayerInput(playerId, dx, dy, shoot);
    }
}

void Server::run() {
    if (!_initialized) {
        LOG_ERROR("Cannot run: not initialized");
        return;
    }

    LOG_INFO("========================================");
    LOG_INFO("R-Type server running!");
    LOG_INFO("Port: ", _port);
    LOG_INFO("Max clients: ", _maxClients);
    LOG_INFO("Architecture:");
    LOG_INFO("  SessionManager - tracking player sessions");
    LOG_INFO("  RoomManager - managing game instances");
    LOG_INFO("  Room '", _defaultRoom->getId(), "' - active game lobby");
    LOG_INFO("  DeterministicGameLoop (IServerLoop) - 60 Hz game loop");
    LOG_INFO("  World - entity management layer");
    LOG_INFO("  GameLogic - 8 ECS systems");
    LOG_INFO("========================================");
    LOG_INFO("THREAD 1: Network (accepting connections)");
    LOG_INFO("THREAD 2: Game loop (DeterministicGameLoop at 60 Hz)");
    LOG_INFO("Press Ctrl+C to stop");
    LOG_INFO("========================================");

    // Publish GAME_STARTED event
    _eventBus->publish(server::GameStartedEvent());

    // Start the deterministic game loop in its own thread
    _gameLoop->start();  // Now void, throws on error

    _running = true;
    uint32_t lastBroadcastTick = 0;
    const uint32_t BROADCAST_INTERVAL = 3;  // Broadcast every 3 ticks (~50ms at 60 Hz)

    // Main server loop - handle network and broadcasting
    while (_running && _networkManager->isRunning() && _gameLoop->isRunning()) {
        // Process network messages from network thread
        _networkManager->processMessages();

        // Broadcast game state periodically
        uint32_t currentTick = _gameLoop->getCurrentTick();
        if (currentTick - lastBroadcastTick >= BROADCAST_INTERVAL) {
            _broadcastGameState();
            lastBroadcastTick = currentTick;
        }

        // Sleep to avoid busy-waiting (network processing is the bottleneck here)
        server::FrameTimer::sleepMilliseconds(5);
    }

    LOG_INFO("Server loop stopped.");
}

void Server::stop() {
    LOG_INFO("Stop requested...");
    _running = false;

    if (_gameLoop) {
        _gameLoop->stop();
    }

    if (_networkManager) {
        _networkManager->stop();
    }

    if (_eventBus) {
        _eventBus->clear();
        LOG_INFO("✓ EventBus cleared");
    }
}

void Server::_broadcastGameState() {
    using namespace RType::Messages;

    // Get ECS world from game loop
    std::shared_ptr<ecs::wrapper::ECSWorld> ecsWorld = _gameLoop->getECSWorld();
    if (!ecsWorld) {
        LOG_ERROR("ECSWorld not available");
        return;
    }

    S2C::GameState state;
    state.serverTick = _gameLoop->getCurrentTick();

    // Get all entities with Transform component (everything that has a position)
    auto entities = ecsWorld->query<ecs::Transform>();

    // Serialize each entity's state using helper method
    for (auto &entity : entities) {
        try {
            S2C::EntityState entityState = _serializeEntity(entity);
            state.entities.push_back(entityState);
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to serialize entity: ", e.what());
            continue;  // Skip this entity but continue with others
        }
    }

    // Serialize and create packet
    std::vector<uint8_t> payload = state.serialize();
    std::vector<uint8_t> packet =
        NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_GAME_STATE, payload);

    // Broadcast to all connected players (unreliable, unsequenced for game state updates)
    for (const auto &[sessionId, peer] : _sessionPeers) {
        if (peer) {
            // Create packet copy for each peer (unsequenced = unreliable fast updates)
            std::unique_ptr<IPacket> peerPacket =
                createPacket(packet, std::to_underlying(PacketFlag::UNSEQUENCED));
            peer->send(std::move(peerPacket), 0);
        }
    }
}

RType::Messages::S2C::EntityState Server::_serializeEntity(ecs::wrapper::Entity &entity) {
    using namespace RType::Messages;

    S2C::EntityState entityState;
    entityState.entityId = entity.getAddress();

    // Get Transform (required - entity was queried with it)
    ecs::Transform &transform = entity.get<ecs::Transform>();
    entityState.position.x = transform.getPosition().x;
    entityState.position.y = transform.getPosition().y;

    // Determine entity type and get health
    if (entity.has<ecs::Player>()) {
        entityState.type = Shared::EntityType::Player;
        entityState.health = entity.has<ecs::Health>() ? entity.get<ecs::Health>().getCurrentHealth() : -1;
    } else if (entity.has<ecs::Enemy>()) {
        ecs::Enemy &enemy = entity.get<ecs::Enemy>();
        // Map enemy type to EntityType enum (simplified)
        entityState.type =
            (enemy.getEnemyType() == 0) ? Shared::EntityType::EnemyType1 : Shared::EntityType::EnemyType1;
        entityState.health = entity.has<ecs::Health>() ? entity.get<ecs::Health>().getCurrentHealth() : -1;
    } else if (entity.has<ecs::Projectile>()) {
        ecs::Projectile &projectile = entity.get<ecs::Projectile>();
        entityState.type =
            projectile.isFriendly() ? Shared::EntityType::PlayerBullet : Shared::EntityType::EnemyBullet;
        entityState.health = -1;  // Projectiles don't have health
    } else {
        // Unknown entity type - default to generic
        entityState.type = Shared::EntityType::Player;
        entityState.health = -1;
    }

    return entityState;
}

void Server::_actionToInput(RType::Messages::Shared::Action action, int &dx, int &dy, bool &shoot) {

    using enum RType::Messages::Shared::Action;
    switch (action) {
        case MoveUp:
            dy = -1;
            break;
        case MoveDown:
            dy = 1;
            break;
        case MoveLeft:
            dx = -1;
            break;
        case MoveRight:
            dx = 1;
            break;
        case Shoot:
            shoot = true;
            break;
    }
}
