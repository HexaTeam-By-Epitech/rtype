/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** Server.hpp
*/

#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include "../Network/ServerNetworkManager.hpp"

// Forward declarations
namespace server {
    class DeterministicGameLoop;
    class EventBus;
}  // namespace server

namespace RType::Messages::Shared {
    enum class Action;
}

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

    /**
     * @brief Broadcast game state to all connected clients
     */
    void _broadcastGameState();

    /**
     * @brief Convert Action enum to directional input (dx, dy)
     * @param action The action to convert
     * @param dx Output: X direction (-1, 0, 1)
     * @param dy Output: Y direction (-1, 0, 1)
     * @param shoot Output: true if shooting
     */
    void _actionToInput(RType::Messages::Shared::Action action, int &dx, int &dy, bool &shoot);

    uint16_t _port;
    size_t _maxClients;

    std::unique_ptr<ServerNetworkManager> _networkManager;
    std::shared_ptr<server::EventBus> _eventBus;
    std::unique_ptr<server::DeterministicGameLoop> _gameLoop;

    // Track player ID to peer mapping for broadcasting
    std::unordered_map<uint32_t, IPeer *> _playerPeers;

    bool _initialized = false;
    bool _running = false;
};