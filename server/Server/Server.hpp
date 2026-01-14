/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** Server.hpp
*/

#pragma once

#include <cstdint>
#include <memory>
#include "server/Core/Clock/FrameTimer.hpp"
#include "server/Network/ServerNetworkManager.hpp"
#include "server/Rooms/Lobby/Lobby.hpp"
#include "server/Rooms/RoomManager/RoomManager.hpp"
#include "server/Sessions/SessionManager/SessionManager.hpp"

// Forward declarations
namespace server {
    class ServerLoop;
    class EventBus;
    class CommandHandler;
    struct CommandContext;
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
     * @brief Handle chat message from client
     * @param event Network event with packet data
     */
    void _handleChatMessage(HostNetworkEvent &event);

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
     * @brief Broadcast room state (player list) to all players in a room
     * @param room Room to broadcast state for
     */
    void _broadcastRoomState(std::shared_ptr<server::Room> room);

    /**
     * @brief Broadcast room list to all players in the lobby
     */
    void _broadcastRoomList();

    /**
     * @brief Broadcast room list to specific peers (or all lobby players if empty)
     * @param specificPeers Vector of peers to send to (empty = all lobby players)
     */
    void _broadcastRoomList(const std::vector<IPeer *> &specificPeers);

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

    /**
     * @brief Send a system message to a specific player
     * @param playerId Player ID to send to
     * @param message Message text
     */
    void _sendSystemMessage(uint32_t playerId, const std::string &message);

    /**
     * @brief Send room list to kicked player to return them to lobby
     * @param playerId Player ID that was kicked
     */
    void _sendKickedNotification(uint32_t playerId);

   public:
    /**
     * @brief Get the lobby instance (for commands)
     * @return Shared pointer to Lobby
     */
    std::shared_ptr<server::Lobby> getLobby() const { return _lobby; }

    /**
     * @brief Notify all players in a room of state changes (for commands)
     * @param room Room to broadcast state for
     */
    void notifyRoomUpdate(std::shared_ptr<server::Room> room);

    /**
     * @brief Kick a player from their current room (for commands)
     * @param playerId Player ID to kick
     * @return true if player was kicked successfully
     */
    bool kickPlayer(uint32_t playerId);

   private:
    uint16_t _port;
    size_t _maxClients;

    std::unique_ptr<ServerNetworkManager> _networkManager;
    std::shared_ptr<server::EventBus> _eventBus;

    // Architecture components
    std::shared_ptr<server::SessionManager> _sessionManager;
    std::shared_ptr<server::RoomManager> _roomManager;
    std::shared_ptr<server::Lobby> _lobby;
    std::unique_ptr<server::CommandHandler> _commandHandler;

    // Default room for development mode
    std::shared_ptr<server::Room> _defaultRoom;
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
