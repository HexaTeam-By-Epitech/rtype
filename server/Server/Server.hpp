/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** Server.hpp
*/

#pragma once

#include <cstdint>
#include <memory>
#include "../Network/ServerNetworkManager.hpp"

/**
 * @class Server
 * @brief R-Type server application
 * 
 * Encapsulates the entire server:
 * - Network communication (ServerNetworkManager)
 * - Game logic (ECS)
 * - World synchronization
 * 
 * Architecture:
 * - THREAD 1: Network (ServerNetworkManager)
 * - THREAD 2: Game loop (ECS + broadcast)
 * 
 * Usage:
 * @code
 * Server server(4242);
 * if (server.initialize()) {
 *     server.run();
 * }
 * @endcode
 */
class Server {
   public:
    /**
     * @brief Constructor
     * @param port Port to listen on
     * @param maxClients Maximum number of clients
     */
    explicit Server(uint16_t port, size_t maxClients = 32);

    /**
     * @brief Destructor - clean shutdown
     */
    ~Server();

    /**
     * @brief Initialize server systems
     * @return true if initialization successful
     */
    bool initialize();

    /**
     * @brief Run the server (blocking until exit)
     */
    void run();

    /**
     * @brief Stop the server
     */
    void stop();

   private:
    /**
     * @brief Handle incoming packet from client
     * @param event Network event with packet data
     */
    void handlePacket(HostNetworkEvent &event);

    uint16_t _port;
    size_t _maxClients;

    std::unique_ptr<ServerNetworkManager> _networkManager;

    bool _initialized = false;
    bool _running = false;
};