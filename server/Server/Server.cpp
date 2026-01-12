/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** Server.cpp
*/

#include "server/Server/Server.hpp"
#include <thread>
#include "Capnp/ConnectionMessages.hpp"
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
#include "server/Game/Logic/IGameLogic.hpp"
#include "server/Game/Rules/GameRules.hpp"
#include "server/Game/Rules/GameruleBroadcaster.hpp"
#include "server/Sessions/Session/Session.hpp"

// CONFIGURATION: Set to true to bypass matchmaking and use default room
constexpr bool DEV_MODE_SKIP_MATCHMAKING = false;

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

    if (!initializeNetworking()) {
        LOG_ERROR("Failed to initialize networking");
        return false;
    }

    _networkManager = std::make_unique<ServerNetworkManager>(_port, _maxClients);
    _networkManager->setPacketHandler([this](HostNetworkEvent &event) { this->handlePacket(event); });

    if (!_networkManager->start()) {
        LOG_ERROR("Failed to start network manager");
        return false;
    }

    _eventBus = std::make_shared<server::EventBus>();
    _sessionManager = std::make_shared<server::SessionManager>();
    _roomManager = std::make_shared<server::RoomManager>();
    _lobby = std::make_shared<server::Lobby>(_roomManager);

    _defaultRoom = _roomManager->createRoom("default");
    if (!_defaultRoom) {
        LOG_ERROR("Failed to create default room");
        return false;
    }
    LOG_INFO("✓ Default room created");

    // Subscribe to game events on global EventBus
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

    _initialized = true;
    LOG_INFO("✓ Server initialized successfully");

    return true;
}

void Server::handlePacket(HostNetworkEvent &event) {
    if (event.type == NetworkEventType::DISCONNECT) {
        _handleDisconnect(event);
        return;
    }

    if (!event.packet || !event.peer) {
        return;
    }

    try {
        using namespace RType::Messages;

        NetworkMessages::MessageType messageType = NetworkMessages::getMessageType(event.packet->getData());

        switch (messageType) {
            case NetworkMessages::MessageType::HANDSHAKE_REQUEST:
                _handleHandshakeRequest(event);
                break;

            case NetworkMessages::MessageType::REGISTER_REQUEST:
                _handleRegisterRequest(event);
                break;

            case NetworkMessages::MessageType::C2S_PLAYER_INPUT:
                _handlePlayerInput(event);
                break;

            case NetworkMessages::MessageType::C2S_LIST_ROOMS:
                _handleListRooms(event);
                break;

            case NetworkMessages::MessageType::C2S_CREATE_ROOM:
                _handleCreateRoom(event);
                break;

            case NetworkMessages::MessageType::C2S_JOIN_ROOM:
                _handleJoinRoom(event);
                break;

            case NetworkMessages::MessageType::C2S_LEAVE_ROOM:
                _handleLeaveRoom(event);
                break;

            case NetworkMessages::MessageType::C2S_START_GAME:
                _handleStartGame(event);
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

    std::shared_ptr<server::Room> playerRoom = _roomManager->getRoomByPlayer(playerId);
    if (playerRoom) {
        std::shared_ptr<server::IGameLogic> gameLogic = playerRoom->getGameLogic();
        if (gameLogic) {
            gameLogic->despawnPlayer(playerId);
        }

        playerRoom->leave(playerId);
        LOG_INFO("✓ Player ", playerId, " removed from room '", playerRoom->getId(), "'");

        // Broadcast updated room state to remaining players
        _broadcastRoomState(playerRoom);
    }

    _eventBus->publish(server::PlayerLeftEvent(playerId));

    _sessionManager->removeSession(sessionId);
    _sessionPeers.erase(sessionId);
    _peerToSession.erase(it);
    _playerIdToSessionId.erase(playerId);
}

void Server::_handleHandshakeRequest(HostNetworkEvent &event) {
    using namespace RType::Messages;
    using namespace ConnectionMessages;

    // Parse handshake request with authentication credentials
    std::vector<uint8_t> payload = NetworkMessages::getPayload(event.packet->getData());
    HandshakeRequestData handshakeData = parseHandshakeRequest(payload);

    std::string playerName = handshakeData.playerName;
    std::string username = handshakeData.username;
    std::string password = handshakeData.password;

    LOG_INFO("Authentication attempt - Username: '", username, "', Player: '", playerName, "'");

    // Use SessionManager's authenticateAndCreateSession (respects architecture)
    std::string sessionId = _sessionManager->authenticateAndCreateSession(username, password);

    if (sessionId.empty()) {
        LOG_WARNING("❌ Authentication FAILED for user: ", username);

        // Send rejection response
        std::vector<uint8_t> responseData =
            NetworkMessages::createConnectResponse("Authentication failed! Invalid username or password.");
        std::unique_ptr<IPacket> responsePacket =
            createPacket(responseData, static_cast<int>(PacketFlag::RELIABLE));
        event.peer->send(std::move(responsePacket), 0);

        // Disconnect the peer
        event.peer->disconnect();
        return;
    }

    LOG_INFO("✓ Authentication SUCCESS for user: ", username);

    // Get the created session
    std::shared_ptr<server::Session> session = _sessionManager->getSession(sessionId);
    if (!session) {
        LOG_ERROR("Session creation failed after authentication");
        event.peer->disconnect();
        return;
    }

    // Configure session
    static std::atomic<uint32_t> nextPlayerId{1000};
    uint32_t newPlayerId = nextPlayerId.fetch_add(1);

    session->setPlayerId(newPlayerId);
    session->setSpectator(false);
    session->setActive(true);

    _sessionPeers[sessionId] = event.peer;
    _peerToSession[event.peer] = sessionId;
    _playerIdToSessionId[newPlayerId] = sessionId;

    _lobby->addPlayer(newPlayerId, playerName);

    LOG_INFO("✓ Player '", playerName, "' (", username, ") authenticated (Session: ", sessionId,
             ", Player ID: ", newPlayerId, ")");

    std::vector<uint8_t> responseData = NetworkMessages::createConnectResponse(
        "✓ Authentication successful! Welcome to R-Type, " + playerName + "!");
    std::unique_ptr<IPacket> responsePacket =
        createPacket(responseData, static_cast<int>(PacketFlag::RELIABLE));
    event.peer->send(std::move(responsePacket), 0);

    // Send server constants (game rules) early so client can configure itself before gameplay.
    // If/when rules become per-room, we also resend them on GameStart.
    {
        server::GameRules defaultRules;
        server::GameruleBroadcaster::sendAllGamerules(event.peer, defaultRules);
    }

    // DELETE ME - START
    // DEV MODE: Auto-join default room and start game
    if (DEV_MODE_SKIP_MATCHMAKING && _defaultRoom) {
        LOG_INFO("[DEV MODE] Auto-joining player ", newPlayerId, " to default room");

        if (_defaultRoom->join(newPlayerId)) {
            LOG_INFO("✓ Player ", newPlayerId, " auto-joined default room");

            // Send JoinedRoom response
            S2C::JoinedRoom joinedResponse;
            joinedResponse.success = true;
            joinedResponse.roomId = _defaultRoom->getId();
            joinedResponse.errorMessage = "";

            std::vector<uint8_t> joinedData = NetworkMessages::createMessage(
                NetworkMessages::MessageType::S2C_JOINED_ROOM, joinedResponse.serialize());
            std::unique_ptr<IPacket> joinedPacket =
                createPacket(joinedData, static_cast<int>(PacketFlag::RELIABLE));
            event.peer->send(std::move(joinedPacket), 0);

            // Auto-start the game
            if (_defaultRoom->getState() == server::RoomState::WAITING) {
                LOG_INFO("[DEV MODE] Auto-starting game in default room");
                // NOTE: Don't call _sendGameStartToRoom() here.
                // The main server loop will detect IN_PROGRESS and call it exactly once
                // using room->tryMarkGameStartSent(). Calling it here causes double GameStart.
                _defaultRoom->startGame();
            }
        } else {
            LOG_ERROR("Failed to auto-join player ", newPlayerId, " to default room");
        }
    } else {
        LOG_INFO("  Player is now in lobby - waiting for room selection");
    }
    // DELETE ME - END
}

void Server::_handleRegisterRequest(HostNetworkEvent &event) {
    using namespace RType::Messages;
    using namespace ConnectionMessages;

    std::vector<uint8_t> payload = NetworkMessages::getPayload(event.packet->getData());
    RegisterRequestData registerData = parseRegisterRequest(payload);

    std::string username = registerData.username;
    std::string password = registerData.password;

    LOG_INFO("Registration attempt - Username: '", username, "'");

    // Try to register the user
    bool success = _sessionManager->getAuthService()->registerUser(username, password);

    RegisterResponseData response;
    if (success) {
        response.success = true;
        response.message = "✓ Account created successfully! You can now login.";
        LOG_INFO("✓ Registration SUCCESS for user: ", username);
    } else {
        response.success = false;
        response.message =
            "❌ Registration failed! Username may already exist or invalid credentials (min 3 chars "
            "username, 4 chars password).";
        LOG_WARNING("❌ Registration FAILED for user: ", username);
    }

    // Send response
    std::vector<uint8_t> responsePayload = createRegisterResponse(response);
    std::vector<uint8_t> responseData =
        NetworkMessages::createMessage(NetworkMessages::MessageType::REGISTER_RESPONSE, responsePayload);
    std::unique_ptr<IPacket> responsePacket =
        createPacket(responseData, static_cast<int>(PacketFlag::RELIABLE));
    event.peer->send(std::move(responsePacket), 0);
}

void Server::_handlePlayerInput(HostNetworkEvent &event) {
    using namespace RType::Messages;

    std::vector<uint8_t> payload = NetworkMessages::getPayload(event.packet->getData());

    try {
        C2S::PlayerInput packet = C2S::PlayerInput::deserialize(payload);

        // Find session from peer using fast reverse lookup
        uint32_t playerId = 0;
        bool isSpectator = false;
        auto it = _peerToSession.find(event.peer);
        if (it != _peerToSession.end()) {
            std::shared_ptr<server::Session> session = _sessionManager->getSession(it->second);
            if (session) {
                playerId = session->getPlayerId();
                isSpectator = session->isSpectator();
            }
        }

        // Spectators cannot send inputs
        if (isSpectator || playerId == 0) {
            return;
        }

        std::shared_ptr<server::Room> playerRoom = _roomManager->getRoomByPlayer(playerId);
        if (!playerRoom) {
            return;
        }

        std::shared_ptr<server::IGameLogic> gameLogic = playerRoom->getGameLogic();
        if (!gameLogic) {
            return;
        }

        // Iterate through all snapshots in the redundant packet
        // The packet contains a history of inputs. We try to apply all of them.
        // GameLogic::processPlayerInput has a filter to ignore already processed sequence IDs.
        for (const auto &snapshot : packet.inputs) {
            int dx = 0;
            int dy = 0;
            bool shoot = false;

            for (const auto &action : snapshot.actions) {
                int actionDx = 0;
                int actionDy = 0;
                bool actionShoot = false;
                _actionToInput(action, actionDx, actionDy, actionShoot);
                dx += actionDx;
                dy += actionDy;
                shoot = shoot || actionShoot;
            }

            gameLogic->processPlayerInput(playerId, dx, dy, shoot, snapshot.sequenceId);
        }
    } catch (const std::exception &e) {
        LOG_WARNING("Failed to deserialize input packet: ", e.what());
    }
}

void Server::_handleListRooms(HostNetworkEvent &event) {
    using namespace RType::Messages;

    LOG_INFO("Sending room list...");

    auto publicRooms = _roomManager->getPublicRooms();

    S2C::RoomList roomList;
    for (const auto &room : publicRooms) {
        S2C::RoomInfoData info;
        info.roomId = room->getId();
        info.roomName = room->getName();
        info.playerCount = static_cast<uint32_t>(room->getPlayerCount());
        info.maxPlayers = static_cast<uint32_t>(room->getMaxPlayers());
        info.isPrivate = room->isPrivate();

        auto state = room->getState();
        if (state == server::RoomState::WAITING)
            info.state = 0;
        else if (state == server::RoomState::STARTING)
            info.state = 1;
        else if (state == server::RoomState::IN_PROGRESS)
            info.state = 2;
        else if (state == server::RoomState::FINISHED)
            info.state = 3;
        else
            info.state = 0;

        roomList.rooms.push_back(info);
    }

    std::vector<uint8_t> payload = roomList.serialize();
    std::vector<uint8_t> packet =
        NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_ROOM_LIST, payload);
    std::unique_ptr<IPacket> netPacket = createPacket(packet, static_cast<int>(PacketFlag::RELIABLE));
    event.peer->send(std::move(netPacket), 0);
}

void Server::_handleCreateRoom(HostNetworkEvent &event) {
    using namespace RType::Messages;

    std::vector<uint8_t> payload = NetworkMessages::getPayload(event.packet->getData());
    C2S::CreateRoom request = C2S::CreateRoom::deserialize(payload);

    uint32_t playerId = 0;
    bool isSpectator = false;
    auto it = _peerToSession.find(event.peer);
    if (it != _peerToSession.end()) {
        std::shared_ptr<server::Session> session = _sessionManager->getSession(it->second);
        if (session) {
            playerId = session->getPlayerId();
            isSpectator = session->isSpectator();
        }
    }

    if (playerId == 0) {
        LOG_ERROR("Failed to find player for room creation");
        S2C::RoomCreated response("", false, "Session not found");
        std::vector<uint8_t> respPayload = response.serialize();
        std::vector<uint8_t> respPacket =
            NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_ROOM_CREATED, respPayload);
        std::unique_ptr<IPacket> netPacket = createPacket(respPacket, static_cast<int>(PacketFlag::RELIABLE));
        event.peer->send(std::move(netPacket), 0);
        return;
    }

    if (isSpectator) {
        LOG_ERROR("Spectators cannot create rooms");
        S2C::RoomCreated response("", false, "Spectators cannot create rooms");
        std::vector<uint8_t> respPayload = response.serialize();
        std::vector<uint8_t> respPacket =
            NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_ROOM_CREATED, respPayload);
        std::unique_ptr<IPacket> netPacket = createPacket(respPacket, static_cast<int>(PacketFlag::RELIABLE));
        event.peer->send(std::move(netPacket), 0);
        return;
    }

    static std::atomic<uint32_t> nextRoomId{1};
    std::string roomId = "room_" + std::to_string(nextRoomId.fetch_add(1));

    auto room = _roomManager->createRoom(roomId, request.roomName, request.maxPlayers, request.isPrivate);
    if (!room) {
        LOG_ERROR("Failed to create room");
        S2C::RoomCreated response("", false, "Failed to create room");
        std::vector<uint8_t> respPayload = response.serialize();
        std::vector<uint8_t> respPacket =
            NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_ROOM_CREATED, respPayload);
        std::unique_ptr<IPacket> netPacket = createPacket(respPacket, static_cast<int>(PacketFlag::RELIABLE));
        event.peer->send(std::move(netPacket), 0);
        return;
    }

    if (!room->join(playerId)) {
        LOG_ERROR("Failed to join created room");
        _roomManager->removeRoom(roomId);  // Clean up the room since it couldn't be joined
        S2C::RoomCreated response(roomId, false, "Failed to join room");
        std::vector<uint8_t> respPayload = response.serialize();
        std::vector<uint8_t> respPacket =
            NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_ROOM_CREATED, respPayload);
        std::unique_ptr<IPacket> netPacket = createPacket(respPacket, static_cast<int>(PacketFlag::RELIABLE));
        event.peer->send(std::move(netPacket), 0);
        return;
    }

    // Only remove from lobby after successfully joining the room
    _lobby->removePlayer(playerId);

    LOG_INFO("Room '", request.roomName, "' created by player ", playerId);

    S2C::RoomCreated response(roomId, true);
    std::vector<uint8_t> respPayload = response.serialize();
    std::vector<uint8_t> respPacket =
        NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_ROOM_CREATED, respPayload);
    std::unique_ptr<IPacket> netPacket = createPacket(respPacket, static_cast<int>(PacketFlag::RELIABLE));
    event.peer->send(std::move(netPacket), 0);

    // Broadcast room state to the creator (now only player in room)
    _broadcastRoomState(room);
}

void Server::_handleJoinRoom(HostNetworkEvent &event) {
    using namespace RType::Messages;

    std::vector<uint8_t> payload = NetworkMessages::getPayload(event.packet->getData());
    C2S::JoinRoom request = C2S::JoinRoom::deserialize(payload);

    uint32_t playerId = 0;
    bool isSpectator = false;
    auto it = _peerToSession.find(event.peer);
    if (it != _peerToSession.end()) {
        std::shared_ptr<server::Session> session = _sessionManager->getSession(it->second);
        if (session) {
            playerId = session->getPlayerId();
            isSpectator = session->isSpectator();
        }
    }

    if (playerId == 0) {
        LOG_ERROR("Failed to find player for room join");
        S2C::JoinedRoom response("", false, "Session not found");
        std::vector<uint8_t> respPayload = response.serialize();
        std::vector<uint8_t> respPacket =
            NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_JOINED_ROOM, respPayload);
        std::unique_ptr<IPacket> netPacket = createPacket(respPacket, static_cast<int>(PacketFlag::RELIABLE));
        event.peer->send(std::move(netPacket), 0);
        return;
    }

    auto room = _roomManager->getRoom(request.roomId);
    if (!room) {
        LOG_ERROR("Room '", request.roomId, "' not found");
        S2C::JoinedRoom response("", false, "Room not found");
        std::vector<uint8_t> respPayload = response.serialize();
        std::vector<uint8_t> respPacket =
            NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_JOINED_ROOM, respPayload);
        std::unique_ptr<IPacket> netPacket = createPacket(respPacket, static_cast<int>(PacketFlag::RELIABLE));
        event.peer->send(std::move(netPacket), 0);
        return;
    }

    // Join as spectator or player based on session type
    bool joinSuccess = false;
    bool autoSpectator = false;

    if (isSpectator) {
        joinSuccess = room->joinAsSpectator(playerId);
    } else {
        joinSuccess = room->join(playerId);

        // If join fails because game is in progress, automatically join as spectator
        if (!joinSuccess && room->getState() == server::RoomState::IN_PROGRESS) {
            LOG_INFO("Game already in progress, joining player ", playerId, " as spectator");
            joinSuccess = room->joinAsSpectator(playerId);
            autoSpectator = true;
        }
    }

    if (!joinSuccess) {
        std::string errorMsg =
            isSpectator ? "Failed to join as spectator" : "Room is full or game already started";
        LOG_ERROR(errorMsg);
        S2C::JoinedRoom response("", false, errorMsg);
        std::vector<uint8_t> respPayload = response.serialize();
        std::vector<uint8_t> respPacket =
            NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_JOINED_ROOM, respPayload);
        std::unique_ptr<IPacket> netPacket = createPacket(respPacket, static_cast<int>(PacketFlag::RELIABLE));
        event.peer->send(std::move(netPacket), 0);
        return;
    }

    // Only remove from lobby after successfully joining the room
    _lobby->removePlayer(playerId);

    std::string modeStr = (isSpectator || autoSpectator) ? " as spectator" : "";
    LOG_INFO("Player ", playerId, " joined room '", request.roomId, "'", modeStr);

    S2C::JoinedRoom response(request.roomId, true);
    std::vector<uint8_t> respPayload = response.serialize();
    std::vector<uint8_t> respPacket =
        NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_JOINED_ROOM, respPayload);
    std::unique_ptr<IPacket> netPacket = createPacket(respPacket, static_cast<int>(PacketFlag::RELIABLE));
    event.peer->send(std::move(netPacket), 0);

    // Broadcast updated room state to all players in the room
    _broadcastRoomState(room);
}

void Server::_handleStartGame(HostNetworkEvent &event) {
    using namespace RType::Messages;

    uint32_t playerId = 0;
    auto it = _peerToSession.find(event.peer);
    if (it != _peerToSession.end()) {
        std::shared_ptr<server::Session> session = _sessionManager->getSession(it->second);
        if (session) {
            playerId = session->getPlayerId();
        }
    }

    if (playerId == 0) {
        LOG_WARNING("❌ Start game request from unknown session");
        return;
    }

    std::shared_ptr<server::Room> playerRoom = _roomManager->getRoomByPlayer(playerId);

    if (!playerRoom) {
        LOG_WARNING("❌ Player ", playerId, " is not in any room");
        return;
    }
    if (playerRoom->getHost() != playerId) {
        LOG_WARNING("❌ Player ", playerId, " is not the host of room '", playerRoom->getId(), "'");
        return;
    }
    if (playerRoom->getState() != server::RoomState::WAITING) {
        LOG_WARNING("❌ Room '", playerRoom->getId(), "' is not in WAITING state");
        return;
    }

    playerRoom->requestStartGame();
    LOG_INFO("Room '", playerRoom->getId(), "' starting game");
}

void Server::_handleLeaveRoom(HostNetworkEvent &event) {
    using namespace RType::Messages;

    uint32_t playerId = 0;
    auto it = _peerToSession.find(event.peer);
    if (it != _peerToSession.end()) {
        std::shared_ptr<server::Session> session = _sessionManager->getSession(it->second);
        if (session) {
            playerId = session->getPlayerId();
        }
    }

    if (playerId == 0) {
        LOG_WARNING("❌ Leave room request from unknown session");
        return;
    }

    std::shared_ptr<server::Room> playerRoom = _roomManager->getRoomByPlayer(playerId);

    if (!playerRoom) {
        LOG_WARNING("Player ", playerId, " is not in any room");
        return;
    }

    // Remove player from room
    playerRoom->leave(playerId);
    LOG_INFO("✓ Player ", playerId, " left room '", playerRoom->getId(), "'");

    // Broadcast updated room state to remaining players
    _broadcastRoomState(playerRoom);
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
    LOG_INFO("Press Ctrl+C to stop");
    LOG_INFO("========================================");

    _eventBus->publish(server::GameStartedEvent());

    _running = true;
    _frameTimer.reset();

    while (_running && _networkManager->isRunning()) {
        float deltaTime = static_cast<float>(_frameTimer.tick());

        _networkManager->processMessages();

        if (_roomManager) {
            _roomManager->update(deltaTime);

            auto rooms = _roomManager->getAllRooms();
            for (const auto &room : rooms) {
                if (room->getState() == server::RoomState::IN_PROGRESS && room->tryMarkGameStartSent()) {
                    _sendGameStartToRoom(room);
                }
            }
        }

        _broadcastGameState();

        // Sleep to avoid busy-waiting (network processing is the bottleneck here)
        server::FrameTimer::sleepMilliseconds(16);  // ~60 Hz
    }

    LOG_INFO("Server loop stopped.");
}

void Server::stop() {
    LOG_INFO("Stop requested...");
    _running = false;

    // Stop all rooms (which will stop their game loops)
    if (_roomManager) {
        // Each room's destructor will stop its GameLoop
        LOG_INFO("✓ Stopping all rooms...");
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

    // Get all rooms and broadcast each room's game state to its players
    auto rooms = _roomManager->getAllRooms();

    for (const auto &room : rooms) {
        server::ServerLoop *roomLoop = room->getServerLoop();
        if (!roomLoop) {
            continue;
        }

        std::shared_ptr<ecs::wrapper::ECSWorld> ecsWorld = roomLoop->getECSWorld();
        if (!ecsWorld) {
            continue;
        }

        S2C::GameState state;
        state.serverTick = roomLoop->getCurrentTick();

        // Get all entities with Transform component
        auto entities = ecsWorld->query<ecs::Transform>();

        // Serialize each entity's state
        // We need GameLogic to access player input state
        std::shared_ptr<server::IGameLogic> gameLogic = room->getGameLogic();

        for (auto &entity : entities) {
            try {
                S2C::EntityState entityState = _serializeEntity(entity, gameLogic.get());
                state.entities.push_back(entityState);
            } catch (const std::exception &e) {
                LOG_ERROR("Failed to serialize entity: ", e.what());
                continue;
            }
        }

        // Serialize and create packet
        std::vector<uint8_t> payload = state.serialize();
        std::vector<uint8_t> packet =
            NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_GAME_STATE, payload);

        // Broadcast to both players and spectators in this room
        auto players = room->getPlayers();
        auto spectators = room->getSpectators();

        // Combine players and spectators
        std::vector<uint32_t> allRecipients = players;
        allRecipients.insert(allRecipients.end(), spectators.begin(), spectators.end());

        for (uint32_t recipientId : allRecipients) {
            // Find session for this recipient using the mapping
            auto sessionIt = _playerIdToSessionId.find(recipientId);
            if (sessionIt == _playerIdToSessionId.end()) {
                continue;
            }
            const std::string &sessionId = sessionIt->second;

            auto peerIt = _sessionPeers.find(sessionId);
            if (peerIt != _sessionPeers.end() && peerIt->second) {
                std::unique_ptr<IPacket> peerPacket =
                    createPacket(packet, static_cast<int>(PacketFlag::UNSEQUENCED));
                peerIt->second->send(std::move(peerPacket), 0);
            }
        }
    }
}

void Server::_sendGameStartToRoom(std::shared_ptr<server::Room> room) {
    using namespace RType::Messages;

    if (!room || room->getState() != server::RoomState::IN_PROGRESS) {
        return;
    }

    server::ServerLoop *roomLoop = room->getServerLoop();
    if (!roomLoop) {
        LOG_ERROR("ServerLoop not available for room ", room->getId());
        return;
    }

    std::shared_ptr<ecs::wrapper::ECSWorld> ecsWorld = roomLoop->getECSWorld();
    if (!ecsWorld) {
        LOG_ERROR("ECSWorld not available for room ", room->getId());
        return;
    }

    std::shared_ptr<server::IGameLogic> gameLogic = room->getGameLogic();
    if (!gameLogic) {
        LOG_ERROR("GameLogic not available for room ", room->getId());
        return;
    }

    // (Re)send gamerules right before the game starts, in case they are room-specific.
    auto sendRulesToRecipient = [&](uint32_t recipientId) {
        auto sessionIt = _playerIdToSessionId.find(recipientId);
        if (sessionIt == _playerIdToSessionId.end()) {
            return;
        }
        const std::string &sessionId = sessionIt->second;

        auto peerIt = _sessionPeers.find(sessionId);
        if (peerIt == _sessionPeers.end() || !peerIt->second) {
            return;
        }
        server::GameruleBroadcaster::sendAllGamerules(peerIt->second, gameLogic->getGameRules());
    };

    // Get all players in room
    auto players = room->getPlayers();
    auto spectators = room->getSpectators();

    for (uint32_t playerId : players) {
        sendRulesToRecipient(playerId);

        // Find entity ID for this player
        uint32_t entityId = 0;
        auto entities = ecsWorld->query<ecs::Transform, ecs::Player>();
        for (auto &entity : entities) {
            auto &player = entity.get<ecs::Player>();
            if (player.getPlayerId() == static_cast<int>(playerId)) {
                entityId = entity.getAddress();
                break;
            }
        }

        if (entityId == 0) {
            LOG_ERROR("Failed to find entity for player ", playerId);
            continue;
        }

        // Create GameStart message
        S2C::GameStart gameStart;
        gameStart.yourEntityId = entityId;
        gameStart.initialState.serverTick = roomLoop->getCurrentTick();

        // Serialize all entities in the room
        auto allEntities = ecsWorld->query<ecs::Transform>();
        for (auto &entity : allEntities) {
            try {
                S2C::EntityState entityState = _serializeEntity(entity);
                gameStart.initialState.entities.push_back(entityState);
            } catch (const std::exception &e) {
                LOG_ERROR("Failed to serialize entity: ", e.what());
                continue;
            }
        }

        // Send to player
        auto sessionIt = _playerIdToSessionId.find(playerId);
        if (sessionIt != _playerIdToSessionId.end()) {
            const std::string &sessionId = sessionIt->second;
            auto peerIt = _sessionPeers.find(sessionId);

            if (peerIt != _sessionPeers.end() && peerIt->second) {
                std::vector<uint8_t> payload = gameStart.serialize();
                std::vector<uint8_t> packet =
                    NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_GAME_START, payload);
                std::unique_ptr<IPacket> netPacket =
                    createPacket(packet, static_cast<int>(PacketFlag::RELIABLE));
                peerIt->second->send(std::move(netPacket), 0);

                LOG_INFO("✓ Sent GameStart to player ", playerId, " (entity: ", entityId,
                         ", room: ", room->getId(), ")");
            }
        }
    }

    // Send to spectators (with entityId = 0)
    for (uint32_t spectatorId : spectators) {
        // Create GameStart message for spectator
        S2C::GameStart gameStart;
        gameStart.yourEntityId = 0;  // Spectators don't have an entity
        gameStart.initialState.serverTick = roomLoop->getCurrentTick();

        // Serialize all entities in the room
        auto allEntities = ecsWorld->query<ecs::Transform>();
        for (auto &entity : allEntities) {
            try {
                S2C::EntityState entityState = _serializeEntity(entity);
                gameStart.initialState.entities.push_back(entityState);
            } catch (const std::exception &e) {
                LOG_ERROR("Failed to serialize entity: ", e.what());
                continue;
            }
        }

        // Send to spectator
        auto sessionIt = _playerIdToSessionId.find(spectatorId);
        if (sessionIt != _playerIdToSessionId.end()) {
            const std::string &sessionId = sessionIt->second;
            auto peerIt = _sessionPeers.find(sessionId);

            if (peerIt != _sessionPeers.end() && peerIt->second) {
                std::vector<uint8_t> payload = gameStart.serialize();
                std::vector<uint8_t> packet =
                    NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_GAME_START, payload);
                std::unique_ptr<IPacket> netPacket =
                    createPacket(packet, static_cast<int>(PacketFlag::RELIABLE));
                peerIt->second->send(std::move(netPacket), 0);

                LOG_INFO("✓ Sent GameStart to spectator ", spectatorId, " (room: ", room->getId(), ")");
            }
        }
    }
}

void Server::_broadcastRoomState(std::shared_ptr<server::Room> room) {
    using namespace RType::Messages;

    if (!room) {
        return;
    }

    // Create RoomState message
    S2C::RoomState roomState;
    roomState.roomId = room->getId();
    roomState.roomName = room->getName();
    roomState.maxPlayers = static_cast<uint32_t>(room->getMaxPlayers());
    roomState.state = static_cast<uint8_t>(room->getState());

    // Get all players and spectators
    auto players = room->getPlayers();
    auto spectators = room->getSpectators();
    uint32_t hostId = room->getHost();

    roomState.currentPlayers = static_cast<uint32_t>(players.size() + spectators.size());

    // Build player list
    for (uint32_t playerId : players) {
        auto sessionIt = _playerIdToSessionId.find(playerId);
        if (sessionIt == _playerIdToSessionId.end()) {
            continue;
        }

        auto session = _sessionManager->getSession(sessionIt->second);
        if (!session) {
            continue;
        }

        S2C::PlayerData playerData;
        playerData.playerId = playerId;

        // Get player name from lobby
        const server::LobbyPlayer *lobbyPlayer = _lobby->getPlayer(playerId);
        playerData.playerName = lobbyPlayer ? lobbyPlayer->playerName : ("Player" + std::to_string(playerId));

        playerData.isHost = (playerId == hostId);
        playerData.isSpectator = false;

        roomState.players.push_back(playerData);
    }

    // Add spectators
    for (uint32_t spectatorId : spectators) {
        auto sessionIt = _playerIdToSessionId.find(spectatorId);
        if (sessionIt == _playerIdToSessionId.end()) {
            continue;
        }

        auto session = _sessionManager->getSession(sessionIt->second);
        if (!session) {
            continue;
        }

        S2C::PlayerData playerData;
        playerData.playerId = spectatorId;

        // Get player name from lobby
        const server::LobbyPlayer *lobbyPlayer = _lobby->getPlayer(spectatorId);
        playerData.playerName =
            lobbyPlayer ? lobbyPlayer->playerName : ("Spectator" + std::to_string(spectatorId));

        playerData.isHost = false;
        playerData.isSpectator = true;

        roomState.players.push_back(playerData);
    }

    // Serialize message
    std::vector<uint8_t> payload = roomState.serialize();
    std::vector<uint8_t> packet =
        NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_ROOM_STATE, payload);

    // Send to all players and spectators
    std::vector<uint32_t> allRecipients = players;
    allRecipients.insert(allRecipients.end(), spectators.begin(), spectators.end());

    for (uint32_t recipientId : allRecipients) {
        auto sessionIt = _playerIdToSessionId.find(recipientId);
        if (sessionIt == _playerIdToSessionId.end()) {
            continue;
        }

        const std::string &sessionId = sessionIt->second;
        auto peerIt = _sessionPeers.find(sessionId);

        if (peerIt != _sessionPeers.end() && peerIt->second) {
            std::unique_ptr<IPacket> netPacket = createPacket(packet, static_cast<int>(PacketFlag::RELIABLE));
            peerIt->second->send(std::move(netPacket), 0);
        }
    }

    LOG_INFO("✓ Broadcast RoomState to ", allRecipients.size(), " players in room '", room->getId(), "'");
}

RType::Messages::S2C::EntityState Server::_serializeEntity(ecs::wrapper::Entity &entity,
                                                           server::IGameLogic *gameLogic) {
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

        // Retrieve last processed sequence ID for this player
        if (gameLogic) {
            ecs::Player &player = entity.get<ecs::Player>();
            entityState.lastProcessedInput = gameLogic->getLastProcessedInput(player.getPlayerId());
        }
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
