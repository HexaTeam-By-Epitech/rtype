/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** Client.cpp
*/

#include "Client.hpp"

Client::Client(const std::string &playerName, const std::string &host, uint16_t port, bool isSpectator)
    : _playerName(playerName),
      _username(""),
      _password(""),
      _serverHost(host),
      _serverPort(port),
      _isSpectator(isSpectator) {}

Client::Client(const std::string &playerName, const std::string &username, const std::string &password,
               const std::string &host, uint16_t port)
    : _playerName(playerName),
      _username(username),
      _password(password),
      _serverHost(host),
      _serverPort(port),
      _isSpectator(false) {}

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
    _replicator = std::make_unique<Replicator>(*_eventBus, _isSpectator);
    LOG_INFO("✓ Replicator created", _isSpectator ? " (Spectator mode)" : "");

    // Create GameLoop (pass shared EventBus and Replicator)
    _gameLoop = std::make_unique<GameLoop>(*_eventBus, *_replicator);

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

    // Send connect request with authentication
    LOG_INFO("Sending authentication request...");

    // Use default credentials if not provided
    std::string username = _username.empty() ? "guest" : _username;
    std::string password = _password.empty() ? "guest" : _password;

    if (!_replicator->sendConnectRequest(_playerName, username, password)) {
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

    // Automatically join the default room
    LOG_INFO("Joining default room...");
    if (!_replicator->sendJoinRoom("default")) {
        LOG_ERROR("Failed to send join room request");
        return false;
    }

    // Wait for room join confirmation
    for (int i = 0; i < 20; ++i) {
        _replicator->processMessages();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    LOG_INFO("✓ Joined room!");

    // Start the game
    LOG_INFO("Starting game...");
    if (!_replicator->sendStartGame()) {
        LOG_ERROR("Failed to send start game request");
        return false;
    }

    // Wait for game start
    for (int i = 0; i < 20; ++i) {
        _replicator->processMessages();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    LOG_INFO("✓ Game started!");

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
    LOG_INFO(_isSpectator ? "Spectator: " : "Player: ", _playerName);
    LOG_INFO("Server: ", _serverHost, ":", _serverPort);
    if (_isSpectator) {
        LOG_INFO("Mode: SPECTATOR (read-only)");
    }
    LOG_INFO("========================================");

    // Connect to server BEFORE starting game loop
    if (!connectToServer()) {
        LOG_ERROR("Failed to connect to server");
        return;
    }

    // Run game loop (blocking)
    _gameLoop->run();

    LOG_INFO("Game loop stopped.");
}

void Client::stop() {
    LOG_INFO("Stop requested...");
    if (_gameLoop) {
        _gameLoop->stop();
    }
}
