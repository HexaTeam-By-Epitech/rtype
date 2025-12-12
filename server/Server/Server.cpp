/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** Server.cpp
*/

#include "Server.hpp"
#include <chrono>
#include <thread>
#include "../../common/Logger/Logger.hpp"
#include "../Core/EventBus/EventBus.hpp"
#include "../Core/ServerLoop/DeterministicGameLoop.hpp"
#include "../Game/Logic/GameLogic.hpp"
#include "../Game/Logic/GameStateSerializer.hpp"
#include "Capnp/Messages/Messages.hpp"
#include "Capnp/NetworkMessages.hpp"
#include "NetworkFactory.hpp"

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

    LOG_INFO("Initializing R-Type server...");

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

    // Create GameLogic
    auto gameLogic = std::make_unique<server::GameLogic>();

    // Create DeterministicGameLoop with GameLogic
    _gameLoop = std::make_unique<server::DeterministicGameLoop>(std::move(gameLogic), _eventBus);
    LOG_INFO("✓ Game loop created");

    // Initialize game loop (initializes GameLogic and all systems)
    if (!_gameLoop->initialize()) {
        LOG_ERROR("Failed to initialize game loop");
        return false;
    }
    LOG_INFO("✓ Game loop initialized");

    _initialized = true;
    LOG_INFO("Initialization complete!");

    return true;
}

void Server::handlePacket(HostNetworkEvent &event) {
    if (!event.packet || !event.peer) {
        return;
    }

    try {
        using namespace RType::Messages;

        // Get message type
        auto messageType = NetworkMessages::getMessageType(event.packet->getData());

        switch (messageType) {
            case NetworkMessages::MessageType::HANDSHAKE_REQUEST: {
                // Parse connect request
                std::string playerName = NetworkMessages::parseConnectRequest(event.packet->getData());
                LOG_INFO("Player '", playerName, "' requesting to join...");

                // Assign unique entity ID to new player
                static std::atomic<uint32_t> nextPlayerId{1000};
                uint32_t newPlayerId = nextPlayerId.fetch_add(1);

                // Spawn player in game logic
                auto &gameLogic = _gameLoop->getGameLogic();
                uint32_t entityId = gameLogic.spawnPlayer(newPlayerId, playerName);

                if (entityId == 0) {
                    LOG_ERROR("Failed to spawn player ", newPlayerId);
                    break;
                }

                // Track player peer for broadcasting
                _playerPeers[newPlayerId] = event.peer;

                // Create GameStart message with current tick
                S2C::GameStart gameStart;
                gameStart.yourEntityId = newPlayerId;
                gameStart.initialState.serverTick = _gameLoop->getCurrentTick();

                // Serialize and send
                auto gameStartPayload = gameStart.serialize();
                auto gameStartPacket = NetworkMessages::createMessage(
                    NetworkMessages::MessageType::S2C_GAME_START, gameStartPayload);
                auto packet = createPacket(gameStartPacket, static_cast<uint32_t>(PacketFlag::RELIABLE));
                event.peer->send(std::move(packet), 0);

                LOG_INFO("✓ Player '", playerName, "' joined (ID: ", newPlayerId, ", Entity: ", entityId,
                         ")");
                break;
            }

            case NetworkMessages::MessageType::C2S_PLAYER_INPUT: {
                auto payload = NetworkMessages::getPayload(event.packet->getData());
                auto input = C2S::PlayerInput::deserialize(payload);

                // Process each action in the input
                int dx = 0, dy = 0;
                bool shoot = false;

                for (const auto &action : input.actions) {
                    int actionDx = 0, actionDy = 0;
                    bool actionShoot = false;
                    _actionToInput(action, actionDx, actionDy, actionShoot);
                    dx += actionDx;
                    dy += actionDy;
                    shoot = shoot || actionShoot;
                }

                // TODO: Extract player ID from peer or session
                // For now, we need to map peer to player ID
                uint32_t playerId = 0;
                for (const auto &[pid, peer] : _playerPeers) {
                    if (peer == event.peer) {
                        playerId = pid;
                        break;
                    }
                }

                if (playerId != 0) {
                    auto &gameLogic = _gameLoop->getGameLogic();
                    gameLogic.processPlayerInput(playerId, dx, dy, shoot);
                }

                break;
            }

            default:
                LOG_WARNING("Received unknown message type: ", static_cast<int>(messageType));
                break;
        }

    } catch (const std::exception &e) {
        LOG_ERROR("Error handling packet: ", e.what());
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
    LOG_INFO("  THREAD 1: Network (accepting connections)");
    LOG_INFO("  THREAD 2: Game loop (DeterministicGameLoop at 60 Hz)");
    LOG_INFO("Press Ctrl+C to stop");
    LOG_INFO("========================================");

    // Start the deterministic game loop in its own thread
    if (!_gameLoop->start()) {
        LOG_ERROR("Failed to start game loop");
        return;
    }

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
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
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
}

void Server::_broadcastGameState() {
    // TODO: Use GameStateSerializer to get real ECS state
    // For now, broadcast a placeholder message
    using namespace RType::Messages;

    S2C::GameState state;
    state.serverTick = _gameLoop->getCurrentTick();

    // Serialize and create packet
    auto payload = state.serialize();
    auto packet = NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_GAME_STATE, payload);

    // Broadcast to all connected players (unreliable, unsequenced for game state updates)
    for (const auto &[playerId, peer] : _playerPeers) {
        if (peer) {
            // Create packet copy for each peer (unsequenced = unreliable fast updates)
            auto peerPacket = createPacket(packet, static_cast<uint32_t>(PacketFlag::UNSEQUENCED));
            peer->send(std::move(peerPacket), 0);
        }
    }
}

void Server::_actionToInput(RType::Messages::Shared::Action action, int &dx, int &dy, bool &shoot) {
    // Initialize outputs
    dx = 0;
    dy = 0;
    shoot = false;

    switch (action) {
        case RType::Messages::Shared::Action::MoveUp:
            dy = -1;
            break;
        case RType::Messages::Shared::Action::MoveDown:
            dy = 1;
            break;
        case RType::Messages::Shared::Action::MoveLeft:
            dx = -1;
            break;
        case RType::Messages::Shared::Action::MoveRight:
            dx = 1;
            break;
        case RType::Messages::Shared::Action::Shoot:
            shoot = true;
            break;
    }
}