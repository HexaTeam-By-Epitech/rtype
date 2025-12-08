/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** ServerNetworkManager.cpp
*/

#include "ServerNetworkManager.hpp"
#include <iostream>

ServerNetworkManager::ServerNetworkManager(uint16_t port, size_t maxClients)
    : _port(port), _maxClients(maxClients), _packetHandler(nullptr) {}

ServerNetworkManager::~ServerNetworkManager() {
    stop();
}

bool ServerNetworkManager::start() {
    try {
        // Create server host
        auto address = createAddress("0.0.0.0", _port);
        _host = createServerHost(*address, _maxClients, 2);

        std::cout << "[ServerNetworkManager] Server listening on port " << _port << std::endl;

        // Start network thread
        _running = true;
        _networkThread = std::thread(&ServerNetworkManager::networkThreadLoop, this);

        return true;

    } catch (const std::exception &e) {
        std::cerr << "[ServerNetworkManager] Failed to start: " << e.what() << std::endl;
        return false;
    }
}

void ServerNetworkManager::stop() {
    if (!_running) {
        return;
    }

    std::cout << "[ServerNetworkManager] Stopping network thread..." << std::endl;

    _running = false;

    if (_networkThread.joinable()) {
        _networkThread.join();
    }

    _host.reset();

    std::cout << "[ServerNetworkManager] Stopped." << std::endl;
}

void ServerNetworkManager::networkThreadLoop() {
    std::cout << "[ServerNetworkManager] Network thread started" << std::endl;

    while (_running) {
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

    std::cout << "[ServerNetworkManager] Network thread stopped" << std::endl;
}

void ServerNetworkManager::processMessages() {
    // Process all available events from network thread
    while (auto eventOpt = _eventQueue.tryPop()) {
        auto &event = *eventOpt;

        switch (event.type) {
            case NetworkEventType::CONNECT:
                std::cout << "[ServerNetworkManager] New client connected!" << std::endl;
                break;

            case NetworkEventType::RECEIVE:
                if (_packetHandler && event.packet) {
                    _packetHandler(event);
                }
                break;

            case NetworkEventType::DISCONNECT:
                std::cout << "[ServerNetworkManager] Client disconnected" << std::endl;
                break;

            default:
                break;
        }
    }
}