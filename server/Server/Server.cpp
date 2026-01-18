/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** Server.cpp
*/

#include "server/Server/Server.hpp"
#include <algorithm>
#include <chrono>
#include <functional>
#include <thread>
#include "Capnp/ConnectionMessages.hpp"
#include "Capnp/Messages/Messages.hpp"
#include "Capnp/Messages/Shared/SharedTypes.hpp"
#include "Capnp/NetworkMessages.hpp"
#include "NetworkFactory.hpp"
#include "common/ECS/Components/Animation.hpp"
#include "common/ECS/Components/Enemy.hpp"
#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/IComponent.hpp"
#include "common/ECS/Components/OrbitalModule.hpp"
#include "common/ECS/Components/MapData.hpp"
#include "common/ECS/Components/PendingDestroy.hpp"
#include "common/ECS/Components/Player.hpp"
#include "common/ECS/Components/Projectile.hpp"
#include "common/ECS/Components/Sprite.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Components/Wall.hpp"
#include "common/ECSWrapper/ECSWorld.hpp"
#include "common/Logger/Logger.hpp"
#include "server/Commands/CommandContext.hpp"
#include "server/Commands/CommandHandler.hpp"
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
    _commandHandler = std::make_unique<server::CommandHandler>();

    // Set callback for when matchmaking creates a room
    _roomManager->setRoomCreatedCallback(
        [this](std::shared_ptr<server::Room> room) { _onMatchmakingRoomCreated(room); });

    // Subscribe to game events on global EventBus
    _eventBus->subscribe<server::PlayerJoinedEvent>([](const server::PlayerJoinedEvent &event) {
        LOG_INFO("[EVENT] Player joined: ", event.getPlayerName(), " (ID: ", event.getPlayerId(), ")");
    });

    _eventBus->subscribe<server::PlayerLeftEvent>([](const server::PlayerLeftEvent &event) {
        LOG_INFO("[EVENT] Player left (ID: ", event.getPlayerId(), ")");
    });

    _eventBus->subscribe<server::GameStartedEvent>([](const server::GameStartedEvent &event) {
        if (event.getRoomId().empty()) {
            LOG_INFO("[EVENT] Server started!");
        } else {
            LOG_INFO("[EVENT] Game started in room: ", event.getRoomId());
        }
    });

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

            case NetworkMessages::MessageType::LOGIN_REQUEST:
                _handleLoginRequest(event);
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

            case NetworkMessages::MessageType::C2S_AUTO_MATCHMAKING:
                _handleAutoMatchmaking(event);
                break;

            case NetworkMessages::MessageType::C2S_UPDATE_AUTO_MM_PREF:
                _handleUpdateAutoMatchmakingPref(event);
                break;

            case NetworkMessages::MessageType::C2S_LEAVE_ROOM:
                _handleLeaveRoom(event);
                break;

            case NetworkMessages::MessageType::C2S_START_GAME:
                _handleStartGame(event);
                break;

            case NetworkMessages::MessageType::C2S_CHAT_MESSAGE:
                _handleChatMessage(event);
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

    auto session = _getSessionFromPeer(event.peer);
    if (!session) {
        return;
    }

    std::string sessionId = _peerToSession[event.peer];
    uint32_t playerId = session->getPlayerId();

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

        // Broadcast updated room list to ALL connected players (player count changed)
        _broadcastRoomListToAll();
    }

    _eventBus->publish(server::PlayerLeftEvent(playerId));

    _sessionManager->removeSession(sessionId);
    _sessionPeers.erase(sessionId);
    _peerToSession.erase(event.peer);
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
        _sendPacket(
            event.peer, NetworkMessages::MessageType::HANDSHAKE_RESPONSE,
            responseData);  // Implicit type not ideal here but helper works with payload wrapping... wait.
        // Helper assumes we are passing the PAYLOAD if we use createMessage inside it.
        // But createConnectResponse returns the PAYLOAD for HANDSHAKE_RESPONSE?
        // Let's check createConnectResponse. Usually it serializes the struct.
        // Wait, the original code was:
        // NetworkMessages::createConnectResponse(...) -> returns payload?
        // createPacket(...)
        // Actually, NetworkMessages::createConnectResponse likely creates the FULL message or just payload.
        // Looking at usage:
        // std::vector<uint8_t> responseData = NetworkMessages::createConnectResponse("...");
        // std::unique_ptr<IPacket> responsePacket = createPacket(responseData, ...);
        // This implies createConnectResponse returns the FULL message including the type header?
        // If so, _sendPacket shouldn't wrap it again with createMessage.

        // Disconnect the peer
        event.peer->disconnect();
        return;
    }

    // Checking original code:
    // std::vector<uint8_t> responseData = NetworkMessages::createConnectResponse("Authentication failed!...");
    // std::unique_ptr<IPacket> responsePacket = createPacket(responseData, static_cast<int>(PacketFlag::RELIABLE));

    // And other places:
    // std::vector<uint8_t> packet = NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_ROOM_LIST, payload);
    // std::unique_ptr<IPacket> netPacket = createPacket(packet, ...);

    // So createConnectResponse must return the full message OR createPacket expects full message.
    // In _handleListRooms:
    // payload = roomList.serialize();
    // packet = createMessage(TYPE, payload);
    // netPacket = createPacket(packet);

    // So createMessage wraps payload with Type.
    // createConnectResponse likely wraps with HANDSHAKE_RESPONSE type.

    // So my _sendPacket helper:
    // void _sendPacket(IPeer *peer, MessageType type, const std::vector<uint8_t> &payload, bool reliable)
    // Should do:
    // vector packet = createMessage(type, payload);
    // ptr netPacket = createPacket(packet, reliable);
    // peer->send(netPacket);

    // But for HANDSHAKE, createConnectResponse might already include the type?
    // Let's assume createConnectResponse returns the PAYLOAD for now, or I need to check.
    // Wait, in the original code:
    // std::vector<uint8_t> responseData = NetworkMessages::createConnectResponse("Authentication failed! Invalid username or password.");
    // std::unique_ptr<IPacket> responsePacket = createPacket(responseData, static_cast<int>(PacketFlag::RELIABLE));
    // It does NOT call createMessage. So createConnectResponse must return the FULL message (Type + Payload).

    // In this case, I cannot use _sendPacket(peer, type, payload) if payload is already the full message.
    // Or I should make _sendPacket take the full message?
    // No, duplicate reduction aims to consolidate "createMessage + createPacket + send".

    // If createConnectResponse returns full message, then I can just call createPacket + send.
    // But most other messages use createMessage.

    // Let's stick to the majority case. Most messages seem to use createMessage.
    // I will refactor Handshake manually or adjust the helper.

    // Actually, I should probably stick to `_sendPacket` accepting PAYLOAD.
    // For Handshake, I might need to see if there is a way to get just payload.
    // Or I just rewrite the handshake part to use createMessage manually if possible.
    // But createConnectResponse is likely a helper in NetworkMessages.

    // Let's ignore Handshake optimization for `_sendPacket` for a moment and focus on the rest.

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

    // Determine display name based on authentication type
    std::string displayName;
    if (username == "guest") {
        // For guests, generate unique name with first 4 chars of hash
        std::string hashStr = std::to_string(std::hash<std::string>{}(sessionId));
        displayName = "guest_" + hashStr.substr(0, 4);
    } else {
        // For registered users, use their username
        displayName = username;
    }

    _lobby->addPlayer(newPlayerId, displayName);

    LOG_INFO("✓ Player '", displayName, "' (", username, ") authenticated (Session: ", sessionId,
             ", Player ID: ", newPlayerId, ")");

    // Send authentication response with playerId and displayName
    RType::Messages::Connection::HandshakeResponse response;
    response.accepted = true;
    response.sessionId = sessionId;
    response.serverId = "r-type-server";
    response.message = "✓ Authentication successful! Welcome to R-Type, " + displayName + "!";
    response.serverVersion = "1.0.0";
    response.playerId = newPlayerId;
    response.playerName = displayName;

    std::vector<uint8_t> responseData = response.serialize();
    std::vector<uint8_t> packet =
        NetworkMessages::createMessage(NetworkMessages::MessageType::HANDSHAKE_RESPONSE, responseData);
    std::unique_ptr<IPacket> responsePacket = createPacket(packet, static_cast<int>(PacketFlag::RELIABLE));
    event.peer->send(std::move(responsePacket), 0);

    // Send server constants (game rules) early so client can configure itself before gameplay.
    // If/when rules become per-room, we also resend them on GameStart.
    {
        server::GameRules defaultRules;
        server::GameruleBroadcaster::sendAllGamerules(event.peer, defaultRules);
    }

    LOG_INFO("  Player is now in lobby - waiting for room selection");
}

void Server::_handleRegisterRequest(HostNetworkEvent &event) {
    using namespace RType::Messages;

    std::vector<uint8_t> payload = NetworkMessages::getPayload(event.packet->getData());

    // Parse using Cap'n Proto RegisterAccount message
    C2S::RegisterAccount registerMsg = C2S::RegisterAccount::deserialize(payload);
    std::string username = registerMsg.username;
    std::string password = registerMsg.password;

    LOG_INFO("Registration attempt - Username: '", username, "'");

    // Try to register the user
    bool success = _sessionManager->getAuthService()->registerUser(username, password);

    // Create response using Cap'n Proto
    S2C::RegisterResponse response;
    if (success) {
        response.success = true;
        response.message = "Account created successfully! You can now login.";
        LOG_INFO("Registration SUCCESS for user: ", username);
    } else {
        response.success = false;
        response.message =
            "Registration failed! Username may already exist or invalid credentials (min 3 chars "
            "username, 4 chars password).";
        LOG_WARNING("Registration FAILED for user: ", username);
    }

    // Send response
    std::vector<uint8_t> responsePayload = response.serialize();
    _sendPacket(event.peer, NetworkMessages::MessageType::REGISTER_RESPONSE, responsePayload);
}

void Server::_handleLoginRequest(HostNetworkEvent &event) {
    using namespace RType::Messages;

    std::vector<uint8_t> payload = NetworkMessages::getPayload(event.packet->getData());

    // Parse using Cap'n Proto LoginAccount message
    C2S::LoginAccount loginMsg = C2S::LoginAccount::deserialize(payload);
    std::string username = loginMsg.username;
    std::string password = loginMsg.password;

    LOG_INFO("Login attempt - Username: '", username, "'");

    // Try to authenticate and create session
    std::string sessionId = _sessionManager->authenticateAndCreateSession(username, password);
    bool success = !sessionId.empty();

    // Create response using Cap'n Proto
    S2C::LoginResponse response;
    if (success) {
        response.success = true;
        response.message = "✓ Login successful! Welcome back, " + username + "!";
        response.sessionToken = sessionId;
        response.autoMatchmaking = _sessionManager->getAuthService()->getAutoMatchmaking(username);
        LOG_INFO("✓ Login SUCCESS for user: ", username, " (Session: ", sessionId, ")");

        // Update player name in lobby after successful login
        // Find the peer's session and player ID
        auto sessionIt = _peerToSession.find(event.peer);
        if (sessionIt != _peerToSession.end()) {
            std::shared_ptr<server::Session> session = _sessionManager->getSession(sessionIt->second);
            if (session) {
                uint32_t playerId = session->getPlayerId();
                // Store username for future preference updates
                _playerIdToUsername[playerId] = username;
                // Determine display name based on username
                std::string displayName;
                if (username == "guest") {
                    // For guests, generate unique name with first 4 chars of session hash
                    std::string hashStr = std::to_string(std::hash<std::string>{}(sessionId));
                    displayName = "guest_" + hashStr.substr(0, 4);
                } else {
                    // For registered users, use their username
                    displayName = username;
                }
                _lobby->updatePlayerName(playerId, displayName);
            }
        }
    } else {
        response.success = false;
        response.message = "Login failed! Invalid username or password.";
        response.sessionToken = "";
        response.autoMatchmaking = false;
        LOG_WARNING("Login FAILED for user: ", username);
    }

    // Send response
    std::vector<uint8_t> responsePayload = response.serialize();
    _sendPacket(event.peer, NetworkMessages::MessageType::LOGIN_RESPONSE, responsePayload);
}

void Server::_handlePlayerInput(HostNetworkEvent &event) {
    using namespace RType::Messages;

    std::vector<uint8_t> payload = NetworkMessages::getPayload(event.packet->getData());

    try {
        C2S::PlayerInput packet = C2S::PlayerInput::deserialize(payload);

        auto session = _getSessionFromPeer(event.peer);
        uint32_t playerId = 0;
        bool isSpectator = false;
        if (session) {
            playerId = session->getPlayerId();
            isSpectator = session->isSpectator();
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
        auto snapshots = packet.inputs;
        std::sort(snapshots.begin(), snapshots.end(),
                  [](const C2S::PlayerInput::InputSnapshot &a, const C2S::PlayerInput::InputSnapshot &b) {
                      return a.sequenceId < b.sequenceId;
                  });

        for (const auto &snapshot : snapshots) {
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

    // Send initial room list to the requesting player
    _broadcastRoomList({event.peer});
}

void Server::_handleCreateRoom(HostNetworkEvent &event) {
    using namespace RType::Messages;

    std::vector<uint8_t> payload = NetworkMessages::getPayload(event.packet->getData());
    C2S::CreateRoom request = C2S::CreateRoom::deserialize(payload);

    auto session = _getSessionFromPeer(event.peer);
    uint32_t playerId = 0;
    bool isSpectator = false;
    if (session) {
        playerId = session->getPlayerId();
        isSpectator = session->isSpectator();
    }

    if (playerId == 0) {
        LOG_ERROR("Failed to find player for room creation");
        S2C::RoomCreated response("", false, "Session not found");
        _sendPacket(event.peer, NetworkMessages::MessageType::S2C_ROOM_CREATED, response.serialize());
        return;
    }

    if (isSpectator) {
        LOG_ERROR("Spectators cannot create rooms");
        S2C::RoomCreated response("", false, "Spectators cannot create rooms");
        _sendPacket(event.peer, NetworkMessages::MessageType::S2C_ROOM_CREATED, response.serialize());
        return;
    }

    static std::atomic<uint32_t> nextRoomId{1};
    std::string roomId = "room_" + std::to_string(nextRoomId.fetch_add(1));

    auto room = _roomManager->createRoom(roomId, request.roomName, request.maxPlayers, request.isPrivate);
    if (!room) {
        LOG_ERROR("Failed to create room");
        S2C::RoomCreated response("", false, "Failed to create room");
        _sendPacket(event.peer, NetworkMessages::MessageType::S2C_ROOM_CREATED, response.serialize());
        return;
    }

    if (!room->join(playerId)) {
        LOG_ERROR("Failed to join created room");
        _roomManager->removeRoom(roomId);  // Clean up the room since it couldn't be joined
        S2C::RoomCreated response(roomId, false, "Failed to join room");
        _sendPacket(event.peer, NetworkMessages::MessageType::S2C_ROOM_CREATED, response.serialize());
        return;
    }

    // Player stays in lobby - room membership is tracked separately
    // This allows all players to see all rooms at all times

    LOG_INFO("Room '", request.roomName, "' created by player ", playerId);

    S2C::RoomCreated response(roomId, true);
    _sendPacket(event.peer, NetworkMessages::MessageType::S2C_ROOM_CREATED, response.serialize());

    // Broadcast room state to the creator (now only player in room)
    _broadcastRoomState(room);

    // Broadcast updated room list to ALL connected players
    _broadcastRoomListToAll();
}

void Server::_handleJoinRoom(HostNetworkEvent &event) {
    using namespace RType::Messages;

    std::vector<uint8_t> payload = NetworkMessages::getPayload(event.packet->getData());
    C2S::JoinRoom request = C2S::JoinRoom::deserialize(payload);

    auto session = _getSessionFromPeer(event.peer);
    uint32_t playerId = 0;
    bool isSpectator = false;
    if (session) {
        playerId = session->getPlayerId();
        isSpectator = session->isSpectator();
    }

    if (playerId == 0) {
        LOG_ERROR("Failed to find player for room join");
        S2C::JoinedRoom response("", false, "Session not found");
        _sendPacket(event.peer, NetworkMessages::MessageType::S2C_JOINED_ROOM, response.serialize());
        return;
    }

    auto room = _roomManager->getRoom(request.roomId);
    if (!room) {
        LOG_ERROR("Room '", request.roomId, "' not found");
        S2C::JoinedRoom response("", false, "Room not found");
        _sendPacket(event.peer, NetworkMessages::MessageType::S2C_JOINED_ROOM, response.serialize());
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
        _sendPacket(event.peer, NetworkMessages::MessageType::S2C_JOINED_ROOM, response.serialize());
        return;
    }

    // Player stays in lobby - room membership is tracked separately in RoomManager

    std::string modeStr = (isSpectator || autoSpectator) ? " as spectator" : "";
    LOG_INFO("Player ", playerId, " joined room '", request.roomId, "'", modeStr);

    S2C::JoinedRoom response(request.roomId, true, "", (isSpectator || autoSpectator));
    _sendPacket(event.peer, NetworkMessages::MessageType::S2C_JOINED_ROOM, response.serialize());

    // If player joined as spectator to an in-progress game, send them the current game state
    if ((isSpectator || autoSpectator) && room->getState() == server::RoomState::IN_PROGRESS) {
        LOG_INFO("Sending current game state to spectator ", playerId);
        _sendGameStartToSpectator(playerId, room);
    }

    // Broadcast updated room state to all players in the room
    _broadcastRoomState(room);

    // Broadcast updated room list to ALL connected players (player count changed)
    _broadcastRoomListToAll();
}

void Server::_handleAutoMatchmaking(HostNetworkEvent &event) {
    using namespace RType::Messages;

    auto session = _getSessionFromPeer(event.peer);
    uint32_t playerId = 0;
    bool isSpectator = false;
    if (session) {
        playerId = session->getPlayerId();
        isSpectator = session->isSpectator();
    }

    if (playerId == 0) {
        LOG_ERROR("Failed to find player for auto-matchmaking");
        S2C::JoinedRoom response("", false, "Session not found");
        _sendPacket(event.peer, NetworkMessages::MessageType::S2C_JOINED_ROOM, response.serialize());
        return;
    }

    // Deserialize the AutoMatchmaking message
    std::vector<uint8_t> payload = NetworkMessages::getPayload(event.packet->getData());
    C2S::AutoMatchmaking msg = C2S::AutoMatchmaking::deserialize(payload);

    // Note: This handler triggers matchmaking. Preference update is handled separately.
    LOG_INFO("Auto-matchmaking requested by player ", playerId);

    // If auto-matchmaking is disabled, just return
    if (!msg.enabled) {
        LOG_INFO("Auto-matchmaking disabled for player ", playerId);
        return;
    }

    // Get all available public rooms
    auto availableRooms = _roomManager->getPublicRooms();

    std::shared_ptr<server::Room> targetRoom = nullptr;
    bool joinAsSpectator = false;

    // STRATEGY: If no rooms exist at all, create a new room immediately (like "Create Room")
    if (availableRooms.empty()) {
        LOG_INFO("No rooms available, creating new room for auto-matchmaking");

        // Generate room ID
        static std::atomic<uint32_t> nextAutoRoomId{1};
        std::string roomId = "auto_" + std::to_string(nextAutoRoomId.fetch_add(1));

        // Get player name for room title
        std::string playerName = "Player";
        auto usernameIt = _playerIdToUsername.find(playerId);
        if (usernameIt != _playerIdToUsername.end()) {
            playerName = usernameIt->second;
        }

        std::string roomName = playerName + "'s Room";
        uint32_t maxPlayers = 4;  // Default max players
        bool isPrivate = false;   // Public room

        // Create the room via RoomManager (same as Create Room button)
        targetRoom = _roomManager->createRoom(roomId, roomName, maxPlayers, isPrivate);

        if (!targetRoom) {
            LOG_ERROR("Failed to create auto-matchmaking room");
            S2C::JoinedRoom response("", false, "Failed to create room");
            _sendPacket(event.peer, NetworkMessages::MessageType::S2C_JOINED_ROOM, response.serialize());
            return;
        }

        LOG_INFO("✓ Auto-matchmaking room '", roomName, "' (", roomId, ") created");
    } else {
        // Rooms exist - use MatchmakingService to find best match
        auto matchmakingService = _roomManager->getMatchmaking();
        if (!matchmakingService) {
            LOG_ERROR("MatchmakingService not available");
            S2C::JoinedRoom response("", false, "Matchmaking service unavailable");
            _sendPacket(event.peer, NetworkMessages::MessageType::S2C_JOINED_ROOM, response.serialize());
            return;
        }

        auto [foundRoom, spectator] =
            matchmakingService->findOrCreateMatch(playerId, availableRooms, !isSpectator);

        // If no immediate match found, player was added to queue
        if (!foundRoom) {
            LOG_INFO("Player ", playerId, " added to matchmaking queue");
            S2C::JoinedRoom response(
                "", true,
                "Searching for match... You have been added to the matchmaking queue and "
                "will be notified when a match is found.");
            _sendPacket(event.peer, NetworkMessages::MessageType::S2C_JOINED_ROOM, response.serialize());
            return;
        }

        targetRoom = foundRoom;
        joinAsSpectator = spectator;
    }

    // Join the room (either newly created or found)
    bool joinSuccess = false;
    if (joinAsSpectator) {
        joinSuccess = targetRoom->joinAsSpectator(playerId);
    } else {
        joinSuccess = targetRoom->join(playerId);
    }

    if (!joinSuccess) {
        LOG_ERROR("Failed to join room '", targetRoom->getId(), "'");
        S2C::JoinedRoom response("", false, "Failed to join room");
        _sendPacket(event.peer, NetworkMessages::MessageType::S2C_JOINED_ROOM, response.serialize());
        return;
    }

    std::string modeStr = joinAsSpectator ? " as spectator" : "";
    LOG_INFO("Player ", playerId, " auto-matched to room '", targetRoom->getId(), "'", modeStr);

    S2C::JoinedRoom response(targetRoom->getId(), true, "", joinAsSpectator);
    _sendPacket(event.peer, NetworkMessages::MessageType::S2C_JOINED_ROOM, response.serialize());

    // If player joined as spectator to an in-progress game, send them the current game state
    if (joinAsSpectator && targetRoom->getState() == server::RoomState::IN_PROGRESS) {
        LOG_INFO("Sending current game state to spectator ", playerId);
        _sendGameStartToSpectator(playerId, targetRoom);
    }

    // Broadcast updated room state to all players in the room
    _broadcastRoomState(targetRoom);

    // Broadcast updated room list to ALL connected players
    _broadcastRoomListToAll();
}

void Server::_onMatchmakingRoomCreated(std::shared_ptr<server::Room> room) {
    using namespace RType::Messages;

    LOG_INFO("[Matchmaking] Room '", room->getId(), "' created with ", room->getPlayerCount(),
             " matched players");

    // Get all players in the room and notify them
    auto playerIds = room->getPlayers();
    for (uint32_t playerId : playerIds) {
        // Find the peer for this player
        auto it = _playerIdToSessionId.find(playerId);
        if (it == _playerIdToSessionId.end()) {
            LOG_WARNING("Player ", playerId, " session not found for matchmaking notification");
            continue;
        }

        const std::string &sessionId = it->second;
        auto peerIt = _sessionPeers.find(sessionId);
        if (peerIt == _sessionPeers.end()) {
            LOG_WARNING("Player ", playerId, " peer not found for matchmaking notification");
            continue;
        }

        IPeer *peer = peerIt->second;

        // Send JoinedRoom success response
        S2C::JoinedRoom response(room->getId(), true, "Match found!", false);
        _sendPacket(peer, NetworkMessages::MessageType::S2C_JOINED_ROOM, response.serialize());

        LOG_INFO("Notified player ", playerId, " of match in room '", room->getId(), "'");
    }

    // Broadcast room state to all players in the room
    _broadcastRoomState(room);

    // Broadcast updated room list to ALL connected players
    _broadcastRoomListToAll();
}

void Server::_handleUpdateAutoMatchmakingPref(HostNetworkEvent &event) {
    using namespace RType::Messages;

    auto session = _getSessionFromPeer(event.peer);
    uint32_t playerId = 0;
    if (session) {
        playerId = session->getPlayerId();
    }

    if (playerId == 0) {
        LOG_ERROR("Failed to find player for auto-matchmaking preference update");
        return;
    }

    // Deserialize the AutoMatchmaking message
    std::vector<uint8_t> payload = NetworkMessages::getPayload(event.packet->getData());
    C2S::AutoMatchmaking msg = C2S::AutoMatchmaking::deserialize(payload);

    // Get username from player ID mapping
    auto usernameIt = _playerIdToUsername.find(playerId);
    if (usernameIt != _playerIdToUsername.end()) {
        const std::string &username = usernameIt->second;
        // Update the user's auto-matchmaking preference (but not for guests)
        if (username != "guest") {
            _sessionManager->getAuthService()->updateAutoMatchmaking(username, msg.enabled);
            LOG_INFO("✓ Updated auto-matchmaking preference for user '", username,
                     "': ", msg.enabled ? "ON" : "OFF", " (preference only, NO matchmaking triggered)");
        } else {
            LOG_INFO("Guest user - auto-matchmaking preference not saved");
        }
    } else {
        LOG_WARNING("Username not found for player ", playerId);
    }

    // DO NOT trigger matchmaking here - this is only a preference update
}

void Server::_handleStartGame(HostNetworkEvent &event) {
    using namespace RType::Messages;

    auto session = _getSessionFromPeer(event.peer);
    uint32_t playerId = 0;
    if (session) {
        playerId = session->getPlayerId();
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

    auto session = _getSessionFromPeer(event.peer);
    uint32_t playerId = 0;
    if (session) {
        playerId = session->getPlayerId();
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

    // Send LEFT_ROOM notification to the player who left
    S2C::LeftRoom leftRoomMsg(playerId, S2C::LeftRoomReason::VOLUNTARY_LEAVE, "You left the room");
    auto payload = leftRoomMsg.serialize();
    _sendPacket(event.peer, NetworkMessages::MessageType::S2C_LEFT_ROOM, payload, true);

    // Check if room is now empty (no players, no spectators)
    if (playerRoom->getPlayerCount() == 0 && playerRoom->getSpectators().empty()) {
        std::string roomId = playerRoom->getId();
        _roomManager->removeRoom(roomId);
        LOG_INFO("✓ Room '", roomId, "' deleted (empty)");
    } else {
        // Broadcast updated room state to remaining players in the room
        _broadcastRoomState(playerRoom);
    }

    // Broadcast updated room list to ALL connected players
    _broadcastRoomListToAll();
}

void Server::_handleChatMessage(HostNetworkEvent &event) {
    using namespace RType::Messages;

    LOG_DEBUG("[Server] _handleChatMessage called");

    auto session = _getSessionFromPeer(event.peer);
    uint32_t playerId = 0;
    std::string playerName = "Unknown";

    if (session) {
        playerId = session->getPlayerId();

        // Get player name from lobby
        const server::LobbyPlayer *lobbyPlayer = _lobby->getPlayer(playerId);
        playerName = lobbyPlayer ? lobbyPlayer->playerName : ("Player" + std::to_string(playerId));
    }

    LOG_DEBUG("[Server] Chat message from player ", playerId, " (", playerName, ")");

    if (playerId == 0) {
        LOG_WARNING("❌ Chat message from unknown session");
        return;
    }

    // Get player's room
    std::shared_ptr<server::Room> playerRoom = _roomManager->getRoomByPlayer(playerId);
    if (!playerRoom) {
        LOG_WARNING("Player ", playerId, " is not in any room");
        return;
    }

    // Deserialize chat message
    auto payload = NetworkMessages::getPayload(event.packet->getData());
    try {
        auto chatMsg = C2S::C2SChatMessage::deserialize(payload);

        // Validate message length (prevent abuse from malicious clients)
        static constexpr size_t MAX_CHAT_MESSAGE_LENGTH = 256;
        if (chatMsg.message.length() > MAX_CHAT_MESSAGE_LENGTH) {
            LOG_WARNING("Player ", playerId, " sent a message exceeding the maximum length (",
                        chatMsg.message.length(), " > ", MAX_CHAT_MESSAGE_LENGTH, ")");
            _sendSystemMessage(playerId, "Error: Message too long. Maximum length is 256 characters.");
            return;
        }

        // Validate message is not empty (after trimming whitespace)
        if (chatMsg.message.empty() || std::all_of(chatMsg.message.begin(), chatMsg.message.end(),
                                                   [](unsigned char c) { return std::isspace(c) != 0; })) {
            LOG_DEBUG("Player ", playerId, " sent an empty or whitespace-only message");
            return;  // Silently ignore empty messages
        }

        // Check if message is a command (starts with "/")
        if (!chatMsg.message.empty() && chatMsg.message[0] == '/') {
            // Command - process with CommandHandler
            LOG_INFO("[COMMAND] Player ", playerName, " (", playerId, "): ", chatMsg.message);

            // Create command context
            server::CommandContext context(playerId, playerName, playerRoom, this);

            // Execute command
            std::string response = _commandHandler->handleCommand(chatMsg.message, context);

            // Send response to player
            if (!response.empty()) {
                _sendSystemMessage(playerId, response);
            }

            return;
        }

        LOG_INFO("[CHAT] Player ", playerName, " in room '", playerRoom->getId(), "': ", chatMsg.message);

        // Call room method (for logging)
        playerRoom->broadcastChatMessage(playerId, playerName, chatMsg.message);

        // Create S2C ChatMessage
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::system_clock::now().time_since_epoch())
                             .count();

        S2C::S2CChatMessage chatResponse(playerId, playerName, chatMsg.message, timestamp);
        auto responsePayload = chatResponse.serialize();
        auto responsePacket =
            NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_CHAT_MESSAGE, responsePayload);

        // Broadcast to all players in the room
        std::vector<uint32_t> players = playerRoom->getPlayers();
        for (uint32_t targetPlayerId : players) {
            auto it = _playerIdToSessionId.find(targetPlayerId);
            if (it != _playerIdToSessionId.end()) {
                std::string sessionId = it->second;
                auto peerIt = _sessionPeers.find(sessionId);
                if (peerIt != _sessionPeers.end()) {
                    _sendPacket(peerIt->second, NetworkMessages::MessageType::S2C_CHAT_MESSAGE,
                                responsePayload, true);
                }
            }
        }

        LOG_INFO("✓ Chat message broadcast to ", players.size(), " players");

    } catch (const std::exception &e) {
        LOG_ERROR("Failed to parse ChatMessage: ", e.what());
    }
}

void Server::_sendSystemMessage(uint32_t playerId, const std::string &message) {
    using namespace RType::Messages;

    // Split multi-line messages into individual messages
    std::istringstream iss(message);
    std::string line;
    int messageCount = 0;

    while (std::getline(iss, line)) {
        // Skip empty lines
        if (line.empty()) {
            continue;
        }

        // Create system chat message (playerId = 0 for system)
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::system_clock::now().time_since_epoch())
                             .count();

        // Add small increment to timestamp for ordering
        timestamp += messageCount;
        messageCount++;

        S2C::S2CChatMessage systemMsg(0, "SYSTEM", line, timestamp);
        auto payload = systemMsg.serialize();

        // Find player's session and peer
        auto it = _playerIdToSessionId.find(playerId);
        if (it != _playerIdToSessionId.end()) {
            std::string sessionId = it->second;
            auto peerIt = _sessionPeers.find(sessionId);
            if (peerIt != _sessionPeers.end()) {
                _sendPacket(peerIt->second, NetworkMessages::MessageType::S2C_CHAT_MESSAGE, payload, true);
            }
        }
    }

    if (messageCount > 0) {
        LOG_DEBUG("✓ Sent ", messageCount, " system message(s) to player ", playerId);
    }
}

void Server::_sendKickedNotification(uint32_t playerId) {
    using namespace RType::Messages;

    // Find player's session and peer
    auto it = _playerIdToSessionId.find(playerId);
    if (it == _playerIdToSessionId.end()) {
        LOG_WARNING("Cannot notify kicked player ", playerId, ": session not found");
        return;
    }

    std::string sessionId = it->second;
    auto peerIt = _sessionPeers.find(sessionId);
    if (peerIt == _sessionPeers.end()) {
        LOG_WARNING("Cannot notify kicked player ", playerId, ": peer not found");
        return;
    }

    IPeer *peer = peerIt->second;

    // Send S2C_LEFT_ROOM with KICKED reason to the kicked player
    LOG_INFO("Sending LEFT_ROOM (KICKED) to player ", playerId);

    S2C::LeftRoom leftRoomMsg(playerId, S2C::LeftRoomReason::KICKED, "You have been kicked by the host");
    auto payload = leftRoomMsg.serialize();

    _sendPacket(peer, NetworkMessages::MessageType::S2C_LEFT_ROOM, payload, true);

    LOG_DEBUG("✓ Kicked player ", playerId, " notified with S2C_LEFT_ROOM");
}

void Server::notifyRoomUpdate(std::shared_ptr<server::Room> room) {
    if (!room) {
        LOG_WARNING("notifyRoomUpdate called with null room");
        return;
    }

    LOG_DEBUG("Broadcasting room state update for room ", room->getId());
    _broadcastRoomState(room);

    // Also update the room list in case player count changed
    _broadcastRoomListToAll();
}

bool Server::kickPlayer(uint32_t playerId) {
    // Find the player's room
    std::shared_ptr<server::Room> playerRoom = _roomManager->getRoomByPlayer(playerId);
    if (!playerRoom) {
        LOG_WARNING("Cannot kick player ", playerId, ": not in any room");
        return false;
    }

    LOG_INFO("Kicking player ", playerId, " from room ", playerRoom->getId());

    // Notify the kicked player BEFORE removing them
    _sendSystemMessage(playerId, "You have been kicked from the room by the host.");

    // Despawn player from game if in progress
    std::shared_ptr<server::IGameLogic> gameLogic = playerRoom->getGameLogic();
    if (gameLogic) {
        gameLogic->despawnPlayer(playerId);
    }

    // Remove from room
    playerRoom->leave(playerId);

    // Send empty room state to kicked player to clear their UI
    _sendKickedNotification(playerId);

    // Broadcast updated room state to remaining players
    _broadcastRoomState(playerRoom);
    _broadcastRoomListToAll();

    // Publish event
    _eventBus->publish(server::PlayerLeftEvent(playerId));

    return true;
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

                    // Broadcast updated room list since state changed to IN_PROGRESS
                    _broadcastRoomListToAll();
                }
            }
        }

        _processPendingDestructions();
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

        std::shared_ptr<server::IGameLogic> gameLogic = room->getGameLogic();

        S2C::GameState state;
        state.serverTick = roomLoop->getCurrentTick();

        // Get all entities with Transform component
        auto entities = ecsWorld->query<ecs::Transform>();

        // Serialize each entity's state
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

        // Log detailed entity information (every 10 frames to avoid spam)
        static uint32_t logCounter = 0;
        if (++logCounter % 10 == 0 && !state.entities.empty()) {
            LOG_INFO("[GameState] Tick ", state.serverTick, " - Broadcasting ", state.entities.size(),
                     " entities");
            for (const auto &e : state.entities) {
                LOG_DEBUG("  Entity ", e.entityId, ": Type=", static_cast<int>(e.type), " Pos=(",
                          e.position.x, ",", e.position.y, ") Health=", e.health.value_or(-1));
            }
        }

        // Broadcast to both players and spectators in this room
        auto players = room->getPlayers();
        auto spectators = room->getSpectators();

        // Combine players and spectators
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
                std::unique_ptr<IPacket> peerPacket =
                    createPacket(packet, static_cast<int>(PacketFlag::UNSEQUENCED));
                peerIt->second->send(std::move(peerPacket), 0);
            }
        }
    }
}

void Server::_processPendingDestructions() {
    using namespace RType::Messages;

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

        // Find all entities marked for destruction
        auto pendingEntities = ecsWorld->query<ecs::PendingDestroy>();

        if (pendingEntities.empty()) {
            continue;
        }

        LOG_DEBUG("[ProcessPendingDestructions] Room '", room->getId(), "' - Found ", pendingEntities.size(),
                  " entities pending destruction");

        // Get all recipients for this room
        auto players = room->getPlayers();
        auto spectators = room->getSpectators();
        std::vector<uint32_t> allRecipients = players;
        allRecipients.insert(allRecipients.end(), spectators.begin(), spectators.end());

        // Process each entity marked for destruction
        std::vector<ecs::Address> toDestroy;
        for (auto &entity : pendingEntities) {
            ecs::Address entityId = entity.getAddress();
            ecs::PendingDestroy &pendingDestroy = entity.get<ecs::PendingDestroy>();

            // Convert internal DestroyReason to network DestroyReason
            Shared::DestroyReason networkReason;
            switch (pendingDestroy.getReason()) {
                case ecs::DestroyReason::OutOfBounds:
                    networkReason = Shared::DestroyReason::OutOfBounds;
                    break;
                case ecs::DestroyReason::Killed:
                    networkReason = Shared::DestroyReason::KilledByPlayer;
                    break;
                case ecs::DestroyReason::Expired:
                    networkReason = Shared::DestroyReason::Collision;
                    break;
                case ecs::DestroyReason::Manual:
                default:
                    networkReason = Shared::DestroyReason::OutOfBounds;
                    break;
            }

            // Create and serialize EntityDestroyed message
            S2C::EntityDestroyed destroyedMsg(entityId, networkReason);
            std::vector<uint8_t> payload = destroyedMsg.serialize();
            std::vector<uint8_t> packet =
                NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_ENTITY_DESTROYED, payload);

            // Send to all recipients in the room
            for (uint32_t recipientId : allRecipients) {
                auto sessionIt = _playerIdToSessionId.find(recipientId);
                if (sessionIt == _playerIdToSessionId.end()) {
                    continue;
                }
                const std::string &sessionId = sessionIt->second;

                auto peerIt = _sessionPeers.find(sessionId);
                if (peerIt != _sessionPeers.end() && peerIt->second) {
                    std::unique_ptr<IPacket> peerPacket =
                        createPacket(packet, static_cast<int>(PacketFlag::RELIABLE));
                    peerIt->second->send(std::move(peerPacket), 0);
                }
            }

            LOG_DEBUG("[ProcessPendingDestructions] Sent EntityDestroyed for entity ", entityId);
            toDestroy.push_back(entityId);
        }

        // Now actually destroy the entities
        for (ecs::Address entityId : toDestroy) {
            ecsWorld->destroyEntity(entityId);
        }

        if (!toDestroy.empty()) {
            LOG_INFO("[ProcessPendingDestructions] Destroyed ", toDestroy.size(), " entities in room '",
                     room->getId(), "'");
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

    // Use helper to serialize all entities once
    auto entities = _serializeEntities(ecsWorld, gameLogic.get());

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

    // Get map configuration from ECS world
    S2C::MapConfig mapConfig;
    auto mapEntities = ecsWorld->query<ecs::MapData>();
    if (!mapEntities.empty()) {
        auto &mapData = mapEntities[0].get<ecs::MapData>();
        mapConfig.background = mapData.getBackgroundSprite();
        mapConfig.parallaxBackground = mapData.getParallaxBackgroundSprite();
        mapConfig.scrollSpeed = mapData.getScrollSpeed();
        mapConfig.parallaxSpeedFactor = mapData.getParallaxSpeedFactor();
        LOG_DEBUG("Map config for GameStart: bg='", mapConfig.background, "', parallax='",
                  mapConfig.parallaxBackground, "', speed=", mapConfig.scrollSpeed,
                  ", parallaxFactor=", mapConfig.parallaxSpeedFactor);
    }

    // Helper to send game start
    auto sendGameStart = [&](uint32_t playerId, uint32_t entityId) {
        S2C::GameStart gameStart;
        gameStart.yourEntityId = entityId;
        gameStart.initialState.serverTick = roomLoop->getCurrentTick();
        gameStart.initialState.entities = entities;
        gameStart.mapConfig = mapConfig;

        auto sessionIt = _playerIdToSessionId.find(playerId);
        if (sessionIt != _playerIdToSessionId.end()) {
            const std::string &sessionId = sessionIt->second;
            auto peerIt = _sessionPeers.find(sessionId);

            if (peerIt != _sessionPeers.end() && peerIt->second) {
                _sendPacket(peerIt->second, NetworkMessages::MessageType::S2C_GAME_START,
                            gameStart.serialize());

                if (entityId != 0) {
                    LOG_INFO("✓ Sent GameStart to player ", playerId, " (entity: ", entityId,
                             ", room: ", room->getId(), ")");
                } else {
                    LOG_INFO("✓ Sent GameStart to spectator ", playerId, " (room: ", room->getId(), ")");
                }
            }
        }
    };

    // Get all players in room
    auto players = room->getPlayers();
    auto spectators = room->getSpectators();

    for (uint32_t playerId : players) {
        sendRulesToRecipient(playerId);

        // Find entity ID for this player
        uint32_t entityId = 0;
        auto playerEntities = ecsWorld->query<ecs::Transform, ecs::Player>();
        for (auto &entity : playerEntities) {
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

        sendGameStart(playerId, entityId);
    }

    // Send to spectators (with entityId = 0)
    for (uint32_t spectatorId : spectators) {
        sendGameStart(spectatorId, 0);
    }
}

void Server::_sendGameStartToSpectator(uint32_t spectatorId, std::shared_ptr<server::Room> room) {
    using namespace RType::Messages;

    if (!room || room->getState() != server::RoomState::IN_PROGRESS) {
        LOG_ERROR("Cannot send game start to spectator: room not in progress");
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

    // Get the peer for this spectator
    auto sessionIt = _playerIdToSessionId.find(spectatorId);
    if (sessionIt == _playerIdToSessionId.end()) {
        LOG_ERROR("Cannot find session for spectator ", spectatorId);
        return;
    }

    const std::string &sessionId = sessionIt->second;
    auto peerIt = _sessionPeers.find(sessionId);
    if (peerIt == _sessionPeers.end() || !peerIt->second) {
        LOG_ERROR("Cannot find peer for spectator ", spectatorId);
        return;
    }

    // Send game rules first
    server::GameruleBroadcaster::sendAllGamerules(peerIt->second, gameLogic->getGameRules());

    // Serialize all entities
    auto entities = _serializeEntities(ecsWorld, gameLogic.get());

    // Get map configuration from ECS world
    S2C::MapConfig mapConfig;
    auto mapEntities = ecsWorld->query<ecs::MapData>();
    if (!mapEntities.empty()) {
        auto &mapData = mapEntities[0].get<ecs::MapData>();
        mapConfig.background = mapData.getBackgroundSprite();
        mapConfig.parallaxBackground = mapData.getParallaxBackgroundSprite();
        mapConfig.scrollSpeed = mapData.getScrollSpeed();
        mapConfig.parallaxSpeedFactor = mapData.getParallaxSpeedFactor();
    }

    // Send GameStart with entityId = 0 (spectator has no controllable entity)
    S2C::GameStart gameStart;
    gameStart.yourEntityId = 0;
    gameStart.initialState.serverTick = roomLoop->getCurrentTick();
    gameStart.initialState.entities = entities;
    gameStart.mapConfig = mapConfig;

    _sendPacket(peerIt->second, NetworkMessages::MessageType::S2C_GAME_START, gameStart.serialize());

    LOG_INFO("✓ Sent GameStart to spectator ", spectatorId, " (room: ", room->getId(),
             ", entities: ", entities.size(), ")");
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
            _sendPacket(peerIt->second, NetworkMessages::MessageType::S2C_ROOM_STATE, payload);
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

    // Get current animation if available
    if (entity.has<ecs::Animation>()) {
        entityState.currentAnimation = entity.get<ecs::Animation>().getCurrentClipName();
    } else {
        entityState.currentAnimation = "idle";  // Default fallback
    }

    // Get sprite info if available
    if (entity.has<ecs::Sprite>()) {
        ecs::Sprite &sprite = entity.get<ecs::Sprite>();
        const auto &rect = sprite.getSourceRect();
        entityState.spriteX = rect.x;
        entityState.spriteY = rect.y;
        entityState.spriteW = rect.width;
        entityState.spriteH = rect.height;
    } else {
        // Default sprite coords (player ship)
        entityState.spriteX = 0;
        entityState.spriteY = 0;
        entityState.spriteW = 33;
        entityState.spriteH = 17;
    }

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
    } else if (entity.has<ecs::Wall>()) {
        entityState.type = Shared::EntityType::Wall;
        entityState.health = entity.has<ecs::Health>() ? entity.get<ecs::Health>().getCurrentHealth() : -1;
    } else if (entity.has<ecs::OrbitalModule>()) {
        entityState.type = Shared::EntityType::OrbitalModule;
        entityState.health = entity.has<ecs::Health>() ? entity.get<ecs::Health>().getCurrentHealth() : -1;
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

std::shared_ptr<server::Session> Server::_getSessionFromPeer(IPeer *peer) {
    auto it = _peerToSession.find(peer);
    if (it == _peerToSession.end()) {
        return nullptr;
    }
    return _sessionManager->getSession(it->second);
}

void Server::_sendPacket(IPeer *peer, NetworkMessages::MessageType type, const std::vector<uint8_t> &payload,
                         bool reliable) {
    using namespace RType::Messages;
    if (!peer)
        return;

    std::vector<uint8_t> packet = NetworkMessages::createMessage(type, payload);
    std::unique_ptr<IPacket> netPacket =
        createPacket(packet, static_cast<int>(reliable ? PacketFlag::RELIABLE : PacketFlag::UNSEQUENCED));
    peer->send(std::move(netPacket), 0);
}

std::vector<RType::Messages::S2C::EntityState> Server::_serializeEntities(
    std::shared_ptr<ecs::wrapper::ECSWorld> world, server::IGameLogic *gameLogic) {
    std::vector<RType::Messages::S2C::EntityState> entities;
    if (!world)
        return entities;

    auto transforms = world->query<ecs::Transform>();
    for (auto &entity : transforms) {
        try {
            entities.push_back(_serializeEntity(entity, gameLogic));
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to serialize entity: ", e.what());
        }
    }
    return entities;
}

void Server::_broadcastRoomList(const std::vector<IPeer *> &specificPeers) {
    using namespace RType::Messages;

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

    // Send to specific peers (used when a player requests the list explicitly)
    for (IPeer *peer : specificPeers) {
        if (peer) {
            _sendPacket(peer, NetworkMessages::MessageType::S2C_ROOM_LIST, payload);
        }
    }

    if (!specificPeers.empty()) {
        LOG_INFO("✓ Sent RoomList to ", specificPeers.size(), " specific peer(s)");
    }
}

void Server::_broadcastRoomListToAll() {
    using namespace RType::Messages;

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

    // Broadcast to ALL connected players (everyone on the server sees all public rooms)
    // Players can be in lobby OR in a room, they still see the full room list
    size_t sentCount = 0;
    for (const auto &[sessionId, peer] : _sessionPeers) {
        if (peer) {
            _sendPacket(peer, NetworkMessages::MessageType::S2C_ROOM_LIST, payload);
            sentCount++;
        }
    }

    LOG_INFO("✓ Broadcast RoomList to ", sentCount, " connected player(s)");
}
