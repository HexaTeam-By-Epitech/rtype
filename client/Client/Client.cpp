/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** Client.cpp
*/

#include "Client.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#include "NetworkFactory.hpp"

Client::Client(const std::string &playerName, const std::string &host, uint16_t port)
    : _playerName(playerName), _serverHost(host), _serverPort(port) {}

Client::~Client() {
    std::cout << "[Client] Shutting down..." << std::endl;
    if (_gameLoop) {
        _gameLoop->shutdown();
    }
    if (_replicator) {
        _replicator->disconnect();
    }
    deinitializeNetworking();
}

bool Client::initialize() {
    if (_initialized) {
        return true;
    }

    std::cout << "[Client] Initializing R-Type client..." << std::endl;

    // Initialize networking
    if (!initializeNetworking()) {
        std::cerr << "[Client] Failed to initialize networking" << std::endl;
        return false;
    }

    // Create EventBus
    _eventBus = std::make_unique<EventBus>();
    std::cout << "[Client] ✓ EventBus created" << std::endl;

    // Create Replicator
    _replicator = std::make_unique<Replicator>(*_eventBus);
    std::cout << "[Client] ✓ Replicator created" << std::endl;

    // Create GameLoop
    _gameLoop = std::make_unique<GameLoop>();
    if (!_gameLoop->initialize()) {
        std::cerr << "[Client] Failed to initialize GameLoop" << std::endl;
        return false;
    }
    std::cout << "[Client] ✓ GameLoop initialized" << std::endl;

    _initialized = true;
    std::cout << "[Client] Initialization complete!" << std::endl;

    return true;
}

bool Client::connectToServer() {
    std::cout << "[Client] Connecting to " << _serverHost << ":" << _serverPort << "..." << std::endl;

    if (!_replicator->connect(_serverHost, _serverPort)) {
        std::cerr << "[Client] Failed to initiate connection" << std::endl;
        return false;
    }

    // Wait for connection to establish (max 5 seconds)
    std::cout << "[Client] Waiting for connection..." << std::endl;
    bool connected = false;
    for (int i = 0; i < 50 && !connected; ++i) {
        _replicator->processMessages();
        connected = _replicator->isConnected();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!connected) {
        std::cerr << "[Client] Connection timeout" << std::endl;
        return false;
    }

    std::cout << "[Client] ✓ Connected to server!" << std::endl;

    // Send connect request with player name
    std::cout << "[Client] Sending connect request (player: " << _playerName << ")..." << std::endl;
    if (!_replicator->sendConnectRequest(_playerName)) {
        std::cerr << "[Client] Failed to send connect request" << std::endl;
        return false;
    }

    // Wait for server response
    std::cout << "[Client] Waiting for server response..." << std::endl;
    for (int i = 0; i < 30; ++i) {
        _replicator->processMessages();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "[Client] ✓ Handshake complete!" << std::endl;
    return true;
}

void Client::run() {
    if (!_initialized) {
        std::cerr << "[Client] Cannot run: not initialized" << std::endl;
        return;
    }

    // Connect to server
    if (!connectToServer()) {
        std::cerr << "[Client] Failed to connect to server" << std::endl;
        return;
    }

    std::cout << "[Client] Starting game loop..." << std::endl;
    std::cout << "[Client] ========================================" << std::endl;
    std::cout << "[Client] R-Type client running!" << std::endl;
    std::cout << "[Client] Player: " << _playerName << std::endl;
    std::cout << "[Client] Server: " << _serverHost << ":" << _serverPort << std::endl;
    std::cout << "[Client] ========================================" << std::endl;

    // Run game loop (blocking)
    _gameLoop->run();

    std::cout << "[Client] Game loop stopped." << std::endl;
}

void Client::stop() {
    std::cout << "[Client] Stop requested..." << std::endl;
    if (_gameLoop) {
        _gameLoop->stop();
    }
}
