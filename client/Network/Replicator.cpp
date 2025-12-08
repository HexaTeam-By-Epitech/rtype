/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** Replicator.cpp
*/

#include "Replicator.hpp"
#include <iostream>
#include "../../common/Networking/NetworkFactory.hpp"

Replicator::Replicator(EventBus &eventBus) : _eventBus(eventBus) {
    _host = createClientHost();
}

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
    auto address = createAddress(host, port);

    // Connect to server (asynchronous - will get CONNECT event later)
    _serverPeer = _host->connect(*address, 2, 0);

    if (!_serverPeer) {
        return false;
    }

    // Start dedicated network thread
    startNetworkThread();

    // Don't set _connected yet - wait for CONNECT event in network thread
    _connected = false;
    return true;
}

void Replicator::disconnect() {
    // Stop network thread first
    stopNetworkThread();

    if (_serverPeer) {
        _serverPeer->disconnect();
        _serverPeer = nullptr;
    }
    _connected = false;
}

bool Replicator::isConnected() const {
    return _connected;
}

void Replicator::startNetworkThread() {
    if (_running) {
        return;  // Thread already running
    }

    _running = true;
    _networkThread = std::thread(&Replicator::networkThreadLoop, this);
}

void Replicator::stopNetworkThread() {
    if (!_running) {
        return;  // Thread not running
    }

    _running = false;
    if (_networkThread.joinable()) {
        _networkThread.join();
    }
}

void Replicator::networkThreadLoop() {
    while (_running) {
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
                    // Decode message and push to queue
                    std::string messageContent =
                        NetworkMessages::parseConnectResponse(event.packet->getData());

                    NetworkEvent netEvent(NetworkMessageType::WORLD_STATE, event.packet->getData());
                    netEvent.setMessageContent(messageContent);
                    _incomingMessages.push(std::move(netEvent));
                }
                break;

            case NetworkEventType::CONNECT:
                _connected = true;
                // Publish connection event
                {
                    NetworkEvent netEvent(NetworkMessageType::CONNECT, {});
                    _incomingMessages.push(std::move(netEvent));
                }
                break;

            case NetworkEventType::DISCONNECT:
                _connected = false;
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
    // Process all available messages from network thread
    while (auto eventOpt = _incomingMessages.tryPop()) {
        auto &netEvent = *eventOpt;

        // Log received messages
        if (!netEvent.getMessageContent().empty()) {
            std::cout << "[CLIENT] Received from server: " << netEvent.getMessageContent() << std::endl;
        }

        // Publish on EventBus for game systems to process
        _eventBus.publish(netEvent);
    }
}

void Replicator::sendPacket(NetworkMessageType type, const std::vector<uint8_t> &data) {
    if (!_serverPeer || !_connected) {
        return;
    }

    auto packet = createPacket(data, static_cast<uint32_t>(PacketFlag::RELIABLE));
    _serverPeer->send(std::move(packet), 0);
}

bool Replicator::sendConnectRequest(const std::string &playerName) {
    if (!_serverPeer || !_connected) {
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
    // TODO: Implement input event handling
}

void Replicator::processIncomingPacket(const std::vector<uint8_t> &packet) {
    // This method is now handled by networkThreadLoop
    // Kept for backwards compatibility if needed
}