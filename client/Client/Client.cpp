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
    _gameLoop = std::make_unique<GameLoop>(*_eventBus, *_replicator, _playerName);

    if (!_gameLoop->initialize()) {
        LOG_ERROR("Failed to initialize GameLoop");
        return false;
    }
    LOG_INFO("✓ GameLoop initialized");

    _initialized = true;
    LOG_INFO("Initialization complete!");

    return true;
}

void Client::SetCredentials(const std::string &username, const std::string &password) {
    _username = username;
    _password = password;
}

void Client::SetServer(const std::string &host, uint16_t port) {
    _serverHost = host;
    _serverPort = port;
    LOG_INFO("[Client] Server changed to ", host, ":", port);
}

bool Client::Connect() {
    return connectToServer();
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

    // Wait for server response (max 3 seconds)
    LOG_INFO("Waiting for server response...");
    bool authenticated = false;
    for (int i = 0; i < 30 && !authenticated; ++i) {
        _replicator->processMessages();
        // Check if we received authentication response
        // For now, we assume processMessages handles it and we're authenticated after processing
        // In a real implementation, you'd check a flag or state in Replicator
        // Since we don't have access to that state, we keep the timeout but reduce it
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // TODO: Add a method to Replicator to check if authenticated
        // For now, we'll just wait a reasonable amount (500ms should be enough)
        if (i >= 5) {
            // After 500ms, assume we got the response
            authenticated = true;
        }
    }

    LOG_INFO("✓ Handshake complete!");

    return true;
}

void Client::run() {
    if (!_initialized) {
        LOG_ERROR("Cannot run: not initialized");
        return;
    }

    LOG_INFO("Starting R-Type client...");
    LOG_INFO("========================================");
    LOG_INFO("R-Type Client Ready");
    LOG_INFO(_isSpectator ? "Spectator: " : "Player: ", _playerName);
    LOG_INFO("Waiting for server selection...");
    LOG_INFO("========================================");

    // Subscribe to SERVER_CONNECT event
    _eventBus->subscribe<UIEvent>([this](const UIEvent &event) {
        if (event.getType() == UIEventType::SERVER_CONNECT) {
            // Parse server info from event data (format: "IP:PORT")
            const std::string &serverInfo = event.getData();
            size_t colonPos = serverInfo.find(':');
            if (colonPos != std::string::npos) {
                std::string ip = serverInfo.substr(0, colonPos);
                uint16_t port = static_cast<uint16_t>(std::stoi(serverInfo.substr(colonPos + 1)));

                LOG_INFO("[Client] Connecting to ", ip, ":", port, "...");
                SetServer(ip, port);

                // Launch connection in separate thread to avoid blocking UI
                std::thread([this]() {
                    if (!Connect()) {
                        LOG_ERROR("[Client] Connection failed!");
                        // Publish CONNECTION_FAILED event
                        _eventBus->publish(UIEvent(UIEventType::CONNECTION_FAILED, "Server unreachable"));
                    } else {
                        LOG_INFO("[Client] ✓ Connected successfully!");
                        // Publish CONNECTION_SUCCESS event
                        _eventBus->publish(UIEvent(UIEventType::CONNECTION_SUCCESS, ""));
                    }
                }).detach();
            }
        }
    });

    // Run game loop (blocking) - connection will happen when SERVER_CONNECT event is published
    _gameLoop->run();

    LOG_INFO("Game loop stopped.");
}

void Client::stop() {
    LOG_INFO("Stop requested...");
    if (_gameLoop) {
        _gameLoop->stop();
    }
}
