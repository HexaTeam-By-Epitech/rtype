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
#include "server/Network/ServerNetworkManager.hpp"
#include "server/Rooms/RoomManager/RoomManager.hpp"
#include "server/Sessions/SessionManager/SessionManager.hpp"

// Forward declarations
namespace server {
    class ServerLoop;
    class EventBus;
}  // namespace server

namespace ecs::wrapper {
    class Entity;
}

namespace RType::Messages::Shared {
    enum class Action;
}

namespace RType::Messages::S2C {
    struct EntityState;
}

/**
 * @class Server
 * @brief R-Type server application
 * 
 * Encapsulates the entire server with proper architecture:
 * - Session management (player authentication and tracking)
 * - Room management (game lobbies and instances)
 * - Network communication (ServerNetworkManager)
 * - Game logic per room (ECS + ServerLoop)
 * - World synchronization
 * 
 * Architecture:
 * Server
 *   ├── SessionManager (track connected players)
 *   ├── RoomManager (manage game instances)
 *   │   └── Room (contains players + game loop)
 *   │       └── ServerLoop (60 Hz)
 *   │           └── World (wraps ECS Registry)
 *   │               └── GameLogic (8 systems)
 *   └── EventBus (global events)
 * 
 * THREAD 1: Network (ServerNetworkManager)
 * THREAD 2: Game loop per room (ECS + broadcast)
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
     * @brief Handle player handshake/connection request
     * @param event Network event with packet data
     */
    void _handleHandshakeRequest(HostNetworkEvent &event);

    /**
     * @brief Handle player input packet
     * @param event Network event with packet data
     */
    void _handlePlayerInput(HostNetworkEvent &event);

    /**
     * @brief Handle player disconnect
     * @param event Network event with peer info
     */
    void _handleDisconnect(HostNetworkEvent &event);

    /**
     * @brief Broadcast game state to all connected clients
     */
    void _broadcastGameState();

    /**
     * @brief Serialize a single entity to network format
     * @param entity The entity to serialize
     * @return EntityState structure ready for network transmission
     */
    RType::Messages::S2C::EntityState _serializeEntity(ecs::wrapper::Entity &entity);

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

    // Architecture components
    std::shared_ptr<server::SessionManager> _sessionManager;
    std::shared_ptr<server::RoomManager> _roomManager;

    // Default room (TODO: Multiple rooms with matchmaking)
    std::shared_ptr<server::Room> _defaultRoom;
    std::unique_ptr<server::ServerLoop> _gameLoop;

    // Track session ID to peer mapping for network communication
    std::unordered_map<std::string, IPeer *> _sessionPeers;
    // Reverse lookup: peer -> session ID for fast disconnect handling
    std::unordered_map<IPeer *, std::string> _peerToSession;

    bool _initialized = false;
    bool _running = false;
};
