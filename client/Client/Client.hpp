/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** Client.hpp
*/

#pragma once

#include <chrono>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include "../common/Logger/Logger.hpp"
#include "Core/EventBus/EventBus.hpp"
#include "Core/GameLoop/GameLoop.hpp"
#include "Network/Replicator.hpp"
#include "NetworkFactory.hpp"

/**
 * @class Client
 * @brief R-Type client application
 * 
 * Encapsulates the entire client:
 * - Authentication (Login/Register)
 * - Network communication (Replicator)
 * - Game loop (GameLoop with ECS + Render)
 * - Event system (EventBus)
 * 
 * Usage:
 * @code
 * Client client("PlayerName", "127.0.0.1", 4242);
 * if (client.initialize()) {
 *     client.run();
 * }
 * @endcode
 */
class Client {
   public:
    /**
     * @brief Constructor
     * @param playerName Player's name
     * @param host Server hostname/IP
     * @param port Server port
     * @param isSpectator Whether this client is in spectator mode
     */
    Client(const std::string &playerName, const std::string &host, uint16_t port, bool isSpectator = false);

    /**
     * @brief Constructor with authentication
     * @param playerName Player's display name
     * @param username Username for authentication
     * @param password Password for authentication
     * @param host Server hostname/IP
     * @param port Server port
     */
    Client(const std::string &playerName, const std::string &username, const std::string &password,
           const std::string &host, uint16_t port);

    /**
     * @brief Destructor - clean shutdown
     */
    ~Client();

    /**
     * @brief Initialize client systems
     * @return true if initialization successful
     */
    bool initialize();

    /**
     * @brief Run the client (Start Game Loop directly)
     */
    void run();

    /**
     * @brief Stop the client
     */
    void stop();

    /**
     * @brief Update credentials for next connection attempt
     */
    void SetCredentials(const std::string &username, const std::string &password);

   private:
    /**
     * @brief Connect to server
     * @return true if connection successful
     */
    bool connectToServer();

    std::string _playerName;
    std::string _username;
    std::string _password;
    std::string _serverHost;
    uint16_t _serverPort;
    bool _isSpectator;

    std::unique_ptr<EventBus> _eventBus;
    std::unique_ptr<Replicator> _replicator;
    std::unique_ptr<GameLoop> _gameLoop;

    bool _initialized = false;
};
