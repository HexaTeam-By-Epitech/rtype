/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** ServerNetworkManager.cpp
*/

#include "ServerNetworkManager.hpp"
#include "../../common/Logger/Logger.hpp"

ServerNetworkManager::ServerNetworkManager(uint16_t port, size_t maxClients)
    : _port(port), _maxClients(maxClients), _packetHandler(nullptr) {}

ServerNetworkManager::~ServerNetworkManager() {
    stop();
}

bool ServerNetworkManager::start() {
    // Atomically check if _running is false and set it to true
    // This prevents multiple threads from starting the server simultaneously
    bool expected = false;
    if (!_running.compare_exchange_strong(expected, true)) {
        LOG_ERROR("Server is already running");
        return false;
    }

    try {
        // Create server host
        auto address = createAddress("0.0.0.0", _port);
        _host = createServerHost(*address, _maxClients, 2);

        LOG_INFO("Server listening on port ", _port);

        // Start network thread
        _networkThread = std::thread(&ServerNetworkManager::networkThreadLoop, this);

        return true;

    } catch (const std::exception &e) {
        LOG_ERROR("Failed to start: ", e.what());
        // Reset _running to false since we failed to start
        _running.store(false);
        return false;
    }
}

void ServerNetworkManager::stop() {
    if (!_running.load()) {
        return;
    }

    LOG_INFO("Stopping network thread...");

    _running.store(false);

    if (_networkThread.joinable()) {
        _networkThread.join();
    }

    _host.reset();

    LOG_INFO("Stopped.");
}

void ServerNetworkManager::networkThreadLoop() {
    LOG_INFO("Network thread started");

    while (_running.load()) {
        if (!_host) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        // Poll for network events (10ms timeout)
        auto eventOpt = _host->service(10);
        if (!eventOpt) {
            continue;
        }

        // Push event to queue for game thread to process
        _eventQueue.push(std::move(*eventOpt));
    }

    LOG_INFO("Network thread stopped");
}

void ServerNetworkManager::processMessages() {
    // Process all available events from network thread
    while (auto eventOpt = _eventQueue.tryPop()) {
        auto &event = *eventOpt;

        switch (event.type) {
            case NetworkEventType::CONNECT:
                LOG_INFO("New client connected!");
                break;

            case NetworkEventType::RECEIVE:
                if (_packetHandler && event.packet) {
                    _packetHandler(event);
                }
                break;

            case NetworkEventType::DISCONNECT:
                LOG_INFO("Client disconnected");
                break;

            default:
                break;
        }
    }
}