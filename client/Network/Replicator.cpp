/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** Replicator.cpp
*/

#include "Replicator.hpp"
#include <chrono>
#include "Capnp/ConnectionMessages.hpp"
#include "Events/UIEvent.hpp"

Replicator::Replicator(EventBus &eventBus, bool isSpectator)
    : _eventBus(eventBus), _isSpectator(isSpectator), _host(createClientHost()) {}
Replicator::~Replicator() {
    disconnect();
}

bool Replicator::connect(const std::string &host, uint16_t port) {
    if (!_host) {
        return false;
    }

    _serverHost = host;
    _serverPort = port;

    // Create address for server
    std::unique_ptr<IAddress> address = createAddress(host, port);

    // Connect to server (asynchronous - will get CONNECT event later)
    _serverPeer = _host->connect(*address, 2, 0);

    if (!_serverPeer) {
        return false;
    }

    // Start dedicated network thread
    startNetworkThread();

    // Don't set _connected yet - wait for CONNECT event in network thread
    _connected.store(false);
    return true;
}

void Replicator::disconnect() {
    // Stop network thread first
    stopNetworkThread();

    if (_serverPeer) {
        // Only disconnect if peer is still valid and connected
        auto state = _serverPeer->getState();
        if (state == PeerState::CONNECTED || state == PeerState::CONNECTION_SUCCEEDED ||
            state == PeerState::CONNECTING) {
            // Use disconnectNow() for instant disconnect instead of graceful disconnect()
            _serverPeer->disconnectNow();
        }
        _serverPeer = nullptr;
    }
    _connected.store(false);
    _authenticated.store(false);
}

bool Replicator::isConnected() const {
    return _connected.load();
}

bool Replicator::isAuthenticated() const {
    return _authenticated.load();
}

void Replicator::startNetworkThread() {
    if (_networkThread.joinable()) {
        return;  // Thread already running
    }

    // Create jthread with lambda that captures 'this' and receives stop_token from jthread
    // The stop_token is automatically passed by jthread when the lambda signature accepts it
    _networkThread = std::jthread([this](std::stop_token st) { networkThreadLoop(st); });
}

void Replicator::stopNetworkThread() {
    _networkThread.request_stop();
    // jthread joins automatically in destructor, but we can join explicitly if needed
    if (_networkThread.joinable()) {
        _networkThread.join();
    }
}

void Replicator::networkThreadLoop(std::stop_token stopToken) {
    while (!stopToken.stop_requested()) {
        if (!_host) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        // Poll for network events (non-blocking)
        auto eventOpt = _host->service(0);
        if (!eventOpt) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        auto &event = *eventOpt;

        switch (event.type) {
            case NetworkEventType::RECEIVE:
                if (event.packet) {
                    auto messageType = NetworkMessages::getMessageType(event.packet->getData());
                    if (messageType != NetworkMessages::MessageType::S2C_GAME_STATE) {
                        LOG_DEBUG("[Replicator] Network thread received packet type: ",
                                  static_cast<int>(messageType));
                    }

                    // Update latency from ENet's built-in RTT calculation
                    if (_serverPeer) {
                        uint32_t enetRtt = _serverPeer->getRoundTripTime();

                        // Apply exponential moving average for smoother ping display
                        float smoothed = _smoothedLatency.load();
                        if (smoothed == 0.0f) {
                            // First measurement
                            smoothed = static_cast<float>(enetRtt);
                        } else {
                            // Smooth with previous values (30% new, 70% old)
                            smoothed = smoothed * (1.0f - PING_SMOOTHING_FACTOR) +
                                       static_cast<float>(enetRtt) * PING_SMOOTHING_FACTOR;
                        }
                        _smoothedLatency.store(smoothed);
                        _latency.store(static_cast<uint32_t>(smoothed));
                    }

                    // Decode message type and push to queue
                    std::string messageContent;

                    // Parse based on message type
                    if (messageType == NetworkMessages::MessageType::HANDSHAKE_RESPONSE) {
                        // Parse using proper HandshakeResponse wrapper
                        auto payload = NetworkMessages::getPayload(event.packet->getData());
                        try {
                            auto handshakeResp =
                                RType::Messages::Connection::HandshakeResponse::deserialize(payload);
                            messageContent = handshakeResp.message;

                            if (handshakeResp.accepted) {
                                _authenticated.store(true);
                                _myPlayerId.store(handshakeResp.playerId);
                                LOG_INFO("✓ Authenticated! Player ID: ", handshakeResp.playerId,
                                         ", Name: ", handshakeResp.playerName);

                                // Publish AUTH_SUCCESS with displayName from server
                                _eventBus.publish(
                                    UIEvent(UIEventType::AUTH_SUCCESS, handshakeResp.playerName));
                            } else {
                                _authenticated.store(false);
                                LOG_ERROR("✗ Authentication failed: ", handshakeResp.message);
                            }
                        } catch (const std::exception &e) {
                            LOG_ERROR("Failed to parse HandshakeResponse: ", e.what());
                            messageContent = "Authentication error";
                            _authenticated.store(false);
                        }
                    } else if (messageType == NetworkMessages::MessageType::REGISTER_RESPONSE) {
                        // Parse RegisterResponse
                        auto payload = NetworkMessages::getPayload(event.packet->getData());
                        try {
                            auto registerResp = RType::Messages::S2C::RegisterResponse::deserialize(payload);
                            if (registerResp.success) {
                                LOG_INFO("✓ Registration successful: ", registerResp.message);
                                messageContent = "Registration successful: " + registerResp.message;
                                _eventBus.publish(
                                    UIEvent(UIEventType::REGISTER_SUCCESS, registerResp.message));
                            } else {
                                LOG_WARNING("✗ Registration failed: ", registerResp.message);
                                messageContent = "Registration failed: " + registerResp.message;
                                _eventBus.publish(
                                    UIEvent(UIEventType::REGISTER_FAILED, registerResp.message));
                            }
                        } catch (const std::exception &e) {
                            LOG_ERROR("Failed to parse RegisterResponse: ", e.what());
                            messageContent = "Registration error";
                            _eventBus.publish(UIEvent(UIEventType::REGISTER_FAILED, "Registration error"));
                        }
                    } else if (messageType == NetworkMessages::MessageType::LOGIN_RESPONSE) {
                        // Parse LoginResponse
                        auto payload = NetworkMessages::getPayload(event.packet->getData());
                        try {
                            auto loginResp = RType::Messages::S2C::LoginResponse::deserialize(payload);
                            if (loginResp.success) {
                                LOG_INFO("✓ Login successful: ", loginResp.message);
                                messageContent = "Login successful: " + loginResp.message;
                                // Optionally store session token
                                // _sessionToken = loginResp.sessionToken;

                                // Publish AUTH_SUCCESS event with username (extract from message or store separately)
                                // For now, we'll extract it from the stored username during sendLoginAccount
                                // This is handled by storing _lastLoginUsername in sendLoginAccount
                                if (!_lastLoginUsername.empty()) {
                                    _eventBus.publish(UIEvent(UIEventType::AUTH_SUCCESS, _lastLoginUsername));
                                }
                            } else {
                                LOG_WARNING("✗ Login failed: ", loginResp.message);
                                messageContent = "Login failed: " + loginResp.message;
                                _eventBus.publish(UIEvent(UIEventType::LOGIN_FAILED, loginResp.message));
                            }
                        } catch (const std::exception &e) {
                            LOG_ERROR("Failed to parse LoginResponse: ", e.what());
                            messageContent = "Login error";
                            _eventBus.publish(UIEvent(UIEventType::LOGIN_FAILED, "Login error"));
                        }
                    } else if (messageType == NetworkMessages::MessageType::S2C_GAME_START) {
                        messageContent = "GameStart received";
                    }

                    NetworkEvent netEvent(NetworkMessageType::WORLD_STATE, event.packet->getData());
                    netEvent.setMessageContent(messageContent);
                    _incomingMessages.push(std::move(netEvent));
                }
                break;

            case NetworkEventType::CONNECT:
                _connected.store(true);
                // Publish connection event
                {
                    NetworkEvent netEvent(NetworkMessageType::CONNECT, {});
                    _incomingMessages.push(std::move(netEvent));
                }
                break;

            case NetworkEventType::DISCONNECT:
                _connected.store(false);
                _authenticated.store(false);
                _serverPeer = nullptr;
                // Publish disconnection event
                {
                    NetworkEvent netEvent(NetworkMessageType::DISCONNECT, {});
                    _incomingMessages.push(std::move(netEvent));
                }
                break;

            default:
                break;
        }
    }
}

void Replicator::processMessages() {
    using namespace RType::Messages;

    // Process all available messages from network thread
    while (auto eventOpt = _incomingMessages.tryPop()) {
        auto &netEvent = *eventOpt;

        // Decode and log specific message types
        auto messageType = NetworkMessages::getMessageType(netEvent.getData());

        if (messageType != NetworkMessages::MessageType::S2C_GAME_STATE) {
            LOG_DEBUG("[Replicator] Popped message type: ", static_cast<int>(messageType));
        }

        if (messageType == NetworkMessages::MessageType::S2C_GAME_START) {
            // Decode GameStart message
            auto payload = NetworkMessages::getPayload(netEvent.getData());
            try {
                auto gameStart = S2C::GameStart::deserialize(payload);

                LOG_INFO("✓ GameStart received!");
                LOG_INFO("  - Your entity ID: ", gameStart.yourEntityId);
                LOG_INFO("  - Server tick: ", gameStart.initialState.serverTick);
                LOG_INFO("  - Total entities: ", gameStart.initialState.entities.size());

                // Count entities by type
                int players = 0, enemies = 0, bullets = 0;
                for (const auto &entity : gameStart.initialState.entities) {
                    if (entity.type == Shared::EntityType::Player)
                        players++;
                    else if (entity.type == Shared::EntityType::EnemyType1)
                        enemies++;
                    else if (entity.type == Shared::EntityType::PlayerBullet ||
                             entity.type == Shared::EntityType::EnemyBullet)
                        bullets++;
                }

                LOG_INFO("  - Players: ", players);
                LOG_INFO("  - Enemies: ", enemies);
                LOG_INFO("  - Bullets: ", bullets);
            } catch (const std::exception &e) {
                LOG_ERROR("Error decoding GameStart: ", e.what());
            }
        } else if (messageType == NetworkMessages::MessageType::S2C_ROOM_LIST) {
            // Decode RoomList message
            auto payload = NetworkMessages::getPayload(netEvent.getData());
            try {
                auto roomList = S2C::RoomList::deserialize(payload);

                LOG_INFO("✓ RoomList received with ", roomList.rooms.size(), " rooms");

                // The NetworkEvent will be published on EventBus with the room list data
                // GameLoop will handle it and update Rendering
            } catch (const std::exception &e) {
                LOG_ERROR("Error decoding RoomList: ", e.what());
            }
        } else if (messageType == NetworkMessages::MessageType::S2C_ROOM_STATE) {
            // Decode RoomState message
            auto payload = NetworkMessages::getPayload(netEvent.getData());
            try {
                auto roomState = S2C::RoomState::deserialize(payload);

                LOG_INFO("✓ RoomState received: ", roomState.roomName, " with ", roomState.players.size(),
                         " players");

                // NetworkEvent will be published on EventBus
                // GameLoop will handle it and update WaitingRoom
            } catch (const std::exception &e) {
                LOG_ERROR("Error decoding RoomState: ", e.what());
            }
        } else if (!netEvent.getMessageContent().empty()) {
            LOG_DEBUG("Received from server: ", netEvent.getMessageContent());
        }

        // Publish on EventBus for game systems to process
        _eventBus.publish(netEvent);
    }
}

void Replicator::sendPacket(NetworkMessageType type, const std::vector<uint8_t> &data) {
    (void)type;
    if (!_serverPeer || !_connected.load()) {
        return;
    }

    auto packet = createPacket(data, static_cast<uint32_t>(PacketFlag::RELIABLE));
    _serverPeer->send(std::move(packet), 0);
}

bool Replicator::sendConnectRequest(const std::string &playerName, const std::string &username,
                                    const std::string &password) {
    if (!_serverPeer || !_connected.load()) {
        return false;
    }

    // Create HandshakeRequest with authentication credentials
    using namespace ConnectionMessages;
    HandshakeRequestData handshakeData;
    handshakeData.clientVersion = "1.0.0";
    handshakeData.playerName = playerName;
    handshakeData.username = username;
    handshakeData.password = password;
    handshakeData.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                                  std::chrono::system_clock::now().time_since_epoch())
                                  .count();

    std::vector<uint8_t> payload = createHandshakeRequest(handshakeData);

    // Wrap in network protocol
    auto requestData =
        NetworkMessages::createMessage(NetworkMessages::MessageType::HANDSHAKE_REQUEST, payload);

    // Send via ENet
    auto packet = createPacket(requestData, static_cast<uint32_t>(PacketFlag::RELIABLE));
    return _serverPeer->send(std::move(packet), 0);
}

bool Replicator::sendRegisterAccount(const std::string &username, const std::string &password) {
    if (!_serverPeer || !_connected.load()) {
        LOG_ERROR("Cannot send RegisterAccount: Not connected");
        return false;
    }

    using namespace RType::Messages;

    // Create RegisterAccount message
    C2S::RegisterAccount request(username, password);
    auto payload = request.serialize();

    // Wrap in network protocol
    auto requestData =
        NetworkMessages::createMessage(NetworkMessages::MessageType::REGISTER_REQUEST, payload);

    // Send via ENet
    auto packet = createPacket(requestData, static_cast<uint32_t>(PacketFlag::RELIABLE));
    return _serverPeer->send(std::move(packet), 0);
}

bool Replicator::sendLoginAccount(const std::string &username, const std::string &password) {
    if (!_serverPeer || !_connected.load()) {
        LOG_ERROR("Cannot send LoginAccount: Not connected");
        return false;
    }

    // Store username for AUTH_SUCCESS event when response arrives
    _lastLoginUsername = username;

    using namespace RType::Messages;

    // Create LoginAccount message
    C2S::LoginAccount request(username, password);
    auto payload = request.serialize();

    // Wrap in network protocol
    auto requestData = NetworkMessages::createMessage(NetworkMessages::MessageType::LOGIN_REQUEST, payload);

    // Send via ENet
    auto packet = createPacket(requestData, static_cast<uint32_t>(PacketFlag::RELIABLE));
    return _serverPeer->send(std::move(packet), 0);
}

bool Replicator::sendListRooms() {
    if (!_serverPeer || !_connected.load()) {
        return false;
    }

    using namespace RType::Messages;

    // Create ListRooms message
    C2S::ListRooms request;
    auto payload = request.serialize();

    // Wrap in network protocol
    auto requestData = NetworkMessages::createMessage(NetworkMessages::MessageType::C2S_LIST_ROOMS, payload);

    // Send via ENet
    auto packet = createPacket(requestData, static_cast<uint32_t>(PacketFlag::RELIABLE));
    return _serverPeer->send(std::move(packet), 0);
}

bool Replicator::sendCreateRoom(const std::string &roomName, uint32_t maxPlayers, bool isPrivate,
                                float gameSpeedMultiplier) {
    if (!_serverPeer || !_connected.load()) {
        return false;
    }

    using namespace RType::Messages;

    // Create CreateRoom message with game speed multiplier
    C2S::CreateRoom request(roomName, maxPlayers, isPrivate, gameSpeedMultiplier);
    auto payload = request.serialize();

    // Wrap in network protocol
    auto requestData = NetworkMessages::createMessage(NetworkMessages::MessageType::C2S_CREATE_ROOM, payload);

    // Send via ENet
    auto packet = createPacket(requestData, static_cast<uint32_t>(PacketFlag::RELIABLE));
    return _serverPeer->send(std::move(packet), 0);
}

bool Replicator::sendJoinRoom(const std::string &roomId) {
    if (!_serverPeer || !_connected.load()) {
        LOG_ERROR("Cannot send JoinRoom: Not connected");
        return false;
    }

    LOG_INFO("Sending JoinRoom request for room: ", roomId);

    using namespace RType::Messages;

    // Create JoinRoom message
    C2S::JoinRoom request(roomId);
    auto payload = request.serialize();

    // Wrap in network protocol
    auto requestData = NetworkMessages::createMessage(NetworkMessages::MessageType::C2S_JOIN_ROOM, payload);

    // Send via ENet
    auto packet = createPacket(requestData, static_cast<uint32_t>(PacketFlag::RELIABLE));
    return _serverPeer->send(std::move(packet), 0);
}

bool Replicator::sendStartGame() {
    if (!_serverPeer || !_connected.load()) {
        LOG_ERROR("Cannot send StartGame: Not connected");
        return false;
    }

    LOG_INFO("Sending StartGame request");

    using namespace RType::Messages;

    // Create StartGame message
    C2S::StartGame request;
    auto payload = request.serialize();

    // Wrap in network protocol
    auto requestData = NetworkMessages::createMessage(NetworkMessages::MessageType::C2S_START_GAME, payload);

    // Send via ENet
    auto packet = createPacket(requestData, static_cast<uint32_t>(PacketFlag::RELIABLE));
    return _serverPeer->send(std::move(packet), 0);
}

uint32_t Replicator::getLatency() const {
    return _latency.load();
}

uint32_t Replicator::getPacketLoss() const {
    return _packetLoss;
}

bool Replicator::isSpectator() const {
    return _isSpectator;
}

bool Replicator::sendRequestRoomList() {
    LOG_INFO("[Replicator] Requesting room list from server");
    return sendListRooms();
}

bool Replicator::sendLeaveRoom() {
    if (!_serverPeer || !_connected.load()) {
        return false;
    }

    using namespace RType::Messages;

    LOG_INFO("[Replicator] Leaving current room");

    // Create LeaveRoom message
    C2S::LeaveRoom request;
    auto payload = request.serialize();

    // Wrap in network protocol
    auto requestData = NetworkMessages::createMessage(NetworkMessages::MessageType::C2S_LEAVE_ROOM, payload);

    // Send via ENet
    auto packet = createPacket(requestData, static_cast<uint32_t>(PacketFlag::RELIABLE));
    return _serverPeer->send(std::move(packet), 0);
}

bool Replicator::sendChatMessage(const std::string &message) {
    if (!_serverPeer || !_connected.load()) {
        LOG_ERROR("[Replicator] Cannot send chat message: Not connected");
        return false;
    }

    using namespace RType::Messages;

    LOG_INFO("[Replicator] Sending chat message: '", message, "'");

    // Create ChatMessage
    C2S::C2SChatMessage chatMsg(message);
    auto payload = chatMsg.serialize();

    LOG_DEBUG("[Replicator] Chat message serialized, payload size: ", payload.size());

    // Wrap in network protocol
    auto requestData =
        NetworkMessages::createMessage(NetworkMessages::MessageType::C2S_CHAT_MESSAGE, payload);

    LOG_DEBUG("[Replicator] Network packet created, total size: ", requestData.size());

    // Send via ENet
    auto packet = createPacket(requestData, static_cast<uint32_t>(PacketFlag::RELIABLE));
    bool sent = _serverPeer->send(std::move(packet), 0);

    LOG_INFO("[Replicator] Chat message sent: ", (sent ? "SUCCESS" : "FAILED"));

    return sent;
}

void Replicator::onInputEvent(const InputEvent &event) {
    (void)event;
    // TODO: Implement input event handling
}

void Replicator::processIncomingPacket(const std::vector<uint8_t> &packet) {
    (void)packet;
    // This method is now handled by networkThreadLoop
    // Kept for backwards compatibility if needed
}
