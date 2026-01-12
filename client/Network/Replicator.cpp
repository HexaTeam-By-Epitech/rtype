/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** Replicator.cpp
*/

#include "Replicator.hpp"
#include <chrono>
#include "Capnp/ConnectionMessages.hpp"

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
                        messageContent = NetworkMessages::parseConnectResponse(event.packet->getData());

                        // Check if authentication succeeded
                        if (messageContent.find("Authentication successful") != std::string::npos) {
                            _authenticated.store(true);
                        } else {
                            _authenticated.store(false);
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

bool Replicator::sendCreateRoom(const std::string &roomName, uint32_t maxPlayers, bool isPrivate) {
    if (!_serverPeer || !_connected.load()) {
        return false;
    }

    using namespace RType::Messages;

    // Create CreateRoom message
    C2S::CreateRoom request(roomName, maxPlayers, isPrivate);
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

void Replicator::onInputEvent(const InputEvent &event) {
    (void)event;
    // TODO: Implement input event handling
}

void Replicator::processIncomingPacket(const std::vector<uint8_t> &packet) {
    (void)packet;
    // This method is now handled by networkThreadLoop
    // Kept for backwards compatibility if needed
}
