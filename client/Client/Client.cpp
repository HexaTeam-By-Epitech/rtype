/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** Client.cpp
*/

#include "Client.hpp"

Client::Client(const std::string &playerName, const std::string &host, uint16_t port)
    : _playerName(playerName), _serverHost(host), _serverPort(port) {}

Client::~Client() {
    LOG_INFO("Client shutting down...");
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

    LOG_INFO("Initializing R-Type client...");

    // Initialize networking
    if (!initializeNetworking()) {
        LOG_ERROR("Failed to initialize networking");
        return false;
    }

    // Create EventBus
    _eventBus = std::make_unique<EventBus>();
    LOG_INFO("✓ EventBus created");

    // Create Replicator
    _replicator = std::make_unique<Replicator>(*_eventBus);
    LOG_INFO("✓ Replicator created");

    // Create GameLoop
    _gameLoop = std::make_unique<GameLoop>();

    if (!_gameLoop->initialize()) {
        LOG_ERROR("Failed to initialize GameLoop");
        return false;
    }
    LOG_INFO("✓ GameLoop initialized");

    _initialized = true;
    LOG_INFO("Initialization complete!");

    return true;
}

bool Client::connectToServer() {
    LOG_INFO("Connecting to ", _serverHost, ":", _serverPort, "...");

    if (!_replicator->connect(_serverHost, _serverPort)) {
        LOG_ERROR("Failed to initiate connection");
        return false;
    }

    // Wait for connection to establish (max 5 seconds)
    LOG_INFO("Waiting for connection...");
    bool connected = false;
    for (int i = 0; i < 50 && !connected; ++i) {
        _replicator->processMessages();
        connected = _replicator->isConnected();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!connected) {
        LOG_ERROR("Connection timeout");
        return false;
    }

    LOG_INFO("✓ Connected to server!");

    // Send connect request with player name
    LOG_INFO("Sending connect request (player: ", _playerName, ")...");
    if (!_replicator->sendConnectRequest(_playerName)) {
        LOG_ERROR("Failed to send connect request");
        return false;
    }

    // Wait for server response
    LOG_INFO("Waiting for server response...");
    for (int i = 0; i < 30; ++i) {
        _replicator->processMessages();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    LOG_INFO("✓ Handshake complete!");
    return true;
}

void Client::run() {
    if (!_initialized) {
        LOG_ERROR("Cannot run: not initialized");
        return;
    }

    LOG_INFO("Starting game loop...");
    LOG_INFO("========================================");
    LOG_INFO("R-Type client running!");
    LOG_INFO("Player: ", _playerName);
    LOG_INFO("Server: ", _serverHost, ":", _serverPort);
    LOG_INFO("========================================");

    // Run game loop (blocking)
    _gameLoop->run();

    // Connect to server
    if (!connectToServer()) {
        LOG_ERROR("Failed to connect to server");
        return;
    }

    LOG_INFO("Game loop stopped.");
}

void Client::stop() {
    LOG_INFO("Stop requested...");
    if (_gameLoop) {
        _gameLoop->stop();
    }
}
