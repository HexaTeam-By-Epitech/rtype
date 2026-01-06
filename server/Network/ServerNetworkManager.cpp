/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** ServerNetworkManager.cpp
*/

#include "server/Network/ServerNetworkManager.hpp"
#include "common/Logger/Logger.hpp"
#include "server/Core/Clock/FrameTimer.hpp"

ServerNetworkManager::ServerNetworkManager(uint16_t port, size_t maxClients)
    : _port(port), _maxClients(maxClients), _packetHandler(nullptr) {}

ServerNetworkManager::~ServerNetworkManager() {
    stop();
    // jthread automatically requests stop and joins in its destructor
}

bool ServerNetworkManager::start() {
    if (_networkThread.joinable()) {
        LOG_ERROR("Server is already running");
        return false;
    }

    try {
        // Create server host
        std::unique_ptr<IAddress> address = createAddress("0.0.0.0", _port);
        _host = createServerHost(*address, _maxClients, 2);

        LOG_INFO("Server listening on port ", _port);

        // Start network thread with jthread (automatically passes stop_token)
        _networkThread = std::jthread([this](std::stop_token stopToken) { networkThreadLoop(stopToken); });

        return true;

    } catch (const std::exception &e) {
        LOG_ERROR("Failed to start: ", e.what());
        return false;
    }
}

void ServerNetworkManager::stop() {
    if (!_networkThread.joinable()) {
        return;
    }

    LOG_INFO("Stopping network thread...");

    _networkThread.request_stop();

    // jthread joins automatically, but we can join explicitly for synchronous shutdown
    if (_networkThread.joinable()) {
        _networkThread.join();
    }

    _host.reset();

    LOG_INFO("Stopped.");
}

void ServerNetworkManager::networkThreadLoop(std::stop_token stopToken) {
    LOG_INFO("Network thread started");

    while (!stopToken.stop_requested()) {
        if (!_host) {
            server::FrameTimer::sleepMilliseconds(10);
            continue;
        }

        // Poll for network events (1ms timeout for faster disconnect detection)
        auto eventOpt = _host->service(1);
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
                // Forward disconnect event to handler so Server can clean up
                if (_packetHandler) {
                    _packetHandler(event);
                }
                break;

            default:
                break;
        }
    }
}