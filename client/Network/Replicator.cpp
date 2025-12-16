/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** Replicator.cpp
*/

#include "Replicator.hpp"

Replicator::Replicator(EventBus &eventBus) : _eventBus(eventBus), _host(createClientHost()) {}

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
        _serverPeer->disconnect();
        _serverPeer = nullptr;
    }
    _connected.store(false);
}

bool Replicator::isConnected() const {
    return _connected.load();
}

void Replicator::startNetworkThread() {
    bool expected = false;
    // Atomically check if _running is false and set it to true
    // This prevents multiple threads from creating the network thread
    if (!_running.compare_exchange_strong(expected, true)) {
        return;  // Thread already running or being started
    }

    _networkThread = std::thread(&Replicator::networkThreadLoop, this);
}

void Replicator::stopNetworkThread() {
    if (!_running.load()) {
        return;  // Thread not running
    }

    _running.store(false);
    if (_networkThread.joinable()) {
        _networkThread.join();
    }
}

void Replicator::networkThreadLoop() {
    while (_running.load()) {
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
                    // Decode message type and push to queue
                    auto messageType = NetworkMessages::getMessageType(event.packet->getData());
                    std::string messageContent;

                    // Parse based on message type
                    if (messageType == NetworkMessages::MessageType::HANDSHAKE_RESPONSE) {
                        messageContent = NetworkMessages::parseConnectResponse(event.packet->getData());
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

bool Replicator::sendConnectRequest(const std::string &playerName) {
    if (!_serverPeer || !_connected.load()) {
        return false;
    }

    // Create Cap'n Proto ConnectRequest
    auto requestData = NetworkMessages::createConnectRequest(playerName);

    // Send via ENet
    auto packet = createPacket(requestData, static_cast<uint32_t>(PacketFlag::RELIABLE));
    return _serverPeer->send(std::move(packet), 0);
}

uint32_t Replicator::getLatency() const {
    return _latency;
}

uint32_t Replicator::getPacketLoss() const {
    return _packetLoss;
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
