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
#include "server/Core/Clock/FrameTimer.hpp"
#include "server/Network/ServerNetworkManager.hpp"
#include "server/Rooms/Lobby/Lobby.hpp"
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
    class EntityState;
}

namespace NetworkMessages {
    enum class MessageType : uint16_t;
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
     * @brief Handle player registration request
     * @param event Network event with packet data
     */
    void _handleRegisterRequest(HostNetworkEvent &event);

    /**
     * @brief Handle player login request
     * @param event Network event with packet data
     */
    void _handleLoginRequest(HostNetworkEvent &event);

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
     * @brief Handle list rooms request
     * @param event Network event with packet data
     */
    void _handleListRooms(HostNetworkEvent &event);

    /**
     * @brief Handle create room request
     * @param event Network event with packet data
     */
    void _handleCreateRoom(HostNetworkEvent &event);

    /**
     * @brief Handle join room request
     * @param event Network event with packet data
     */
    void _handleJoinRoom(HostNetworkEvent &event);

    /**
     * @brief Handle leave room request
     * @param event Network event with packet data
     */
    void _handleLeaveRoom(HostNetworkEvent &event);

    /**
     * @brief Handle start game request (from room host)
     * @param event Network event with packet data
     */
    void _handleStartGame(HostNetworkEvent &event);

    /**
     * @brief Broadcast game state to all connected clients
     */
    void _broadcastGameState();

    /**
     * @brief Send GameStart message to all players in a room
     * @param room Room that just started
     */
    void _sendGameStartToRoom(std::shared_ptr<server::Room> room);

    /**
     * @brief Send GameStart message to a spectator joining an in-progress game
     * @param spectatorId ID of the spectator
     * @param room Room to get current game state from
     */
    void _sendGameStartToSpectator(uint32_t spectatorId, std::shared_ptr<server::Room> room);

    /**
     * @brief Broadcast room state (player list) to all players in a room
     * @param room Room to broadcast state for
     */
    void _broadcastRoomState(std::shared_ptr<server::Room> room);

    /**
     * @brief Broadcast room list to specific peers only
     * @param specificPeers Vector of peers to send to (used when player requests list)
     */
    void _broadcastRoomList(const std::vector<IPeer *> &specificPeers);

    /**
     * @brief Broadcast room list to ALL connected players on the server
     * 
     * This sends the room list to everyone connected, regardless of whether
     * they are in the lobby or already in a room. All players should be able
     * to see all public rooms at all times.
     */
    void _broadcastRoomListToAll();

    /**
     * @brief Serialize a single entity to network format
     * @param entity The entity to serialize
     * @return EntityState structure ready for network transmission
     */
    /**
     * @brief Serialize entity to EntityState
     * @param entity ECS Entity
     * @param gameLogic Optional pointer to GameLogic (to retrieve player input state)
     */
    RType::Messages::S2C::EntityState _serializeEntity(ecs::wrapper::Entity &entity,
                                                       server::IGameLogic *gameLogic = nullptr);

    /**
     * @brief Convert Action enum to directional input (dx, dy)
     * @param action The action to convert
     * @param dx Output: X direction (-1, 0, 1)
     * @param dy Output: Y direction (-1, 0, 1)
     * @param shoot Output: true if shooting
     */
    void _actionToInput(RType::Messages::Shared::Action action, int &dx, int &dy, bool &shoot);

    /**
     * @brief Helper to get session from peer
     */
    std::shared_ptr<server::Session> _getSessionFromPeer(IPeer *peer);

    /**
     * @brief Helper to send a packet to a peer
     */
    void _sendPacket(IPeer *peer, NetworkMessages::MessageType type, const std::vector<uint8_t> &payload,
                     bool reliable = true);

    /**
     * @brief Helper to serialize all entities in a world
     */
    std::vector<RType::Messages::S2C::EntityState> _serializeEntities(
        std::shared_ptr<ecs::wrapper::ECSWorld> world, server::IGameLogic *gameLogic = nullptr);

    uint16_t _port;
    size_t _maxClients;

    std::unique_ptr<ServerNetworkManager> _networkManager;
    std::shared_ptr<server::EventBus> _eventBus;

    // Architecture components
    std::shared_ptr<server::SessionManager> _sessionManager;
    std::shared_ptr<server::RoomManager> _roomManager;
    std::shared_ptr<server::Lobby> _lobby;
    // NOTE: No global _gameLoop anymore - each Room has its own

    // Track session ID to peer mapping for network communication
    std::unordered_map<std::string, IPeer *> _sessionPeers;
    // Reverse lookup: peer -> session ID for fast disconnect handling
    std::unordered_map<IPeer *, std::string> _peerToSession;
    // Lookup: Player ID -> Session ID for broadcasting
    std::unordered_map<uint32_t, std::string> _playerIdToSessionId;

    bool _initialized = false;
    bool _running = false;

    // Frame timing for accurate deltaTime calculation
    server::FrameTimer _frameTimer;
};
