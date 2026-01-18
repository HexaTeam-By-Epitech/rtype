/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** Replicator.hpp
*/

#ifndef REPLICATOR_HPP
#define REPLICATOR_HPP

#include <atomic>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "../common/Logger/Logger.hpp"
#include "Capnp/Messages/Messages.hpp"
#include "Capnp/NetworkMessages.hpp"
#include "Core/EventBus/EventBus.hpp"
#include "Events/InputEvent/InputEvent.hpp"
#include "Events/NetworkEvent/NetworkEvent.hpp"
#include "IHost.hpp"
#include "IPeer.hpp"
#include "NetworkFactory.hpp"
#include "ThreadSafeQueue.hpp"

/**
 * @class Replicator
 * @brief Client-server network replication manager with dedicated network thread
 * 
 * The Replicator is responsible for:
 * - UDP/TCP communication with the server (dedicated thread)
 * - Sending player inputs to the server
 * - Receiving world state from the server
 * - Measuring latency and packet loss
 * - Publishing network events on the EventBus
 * 
 * Multi-threading architecture:
 * ```
 * NETWORK THREAD                    GAME THREAD (ECS)
 * ┌──────────────────┐              ┌──────────────────┐
 * │ while(running) { │              │ GameLoop {       │
 * │   service()      │              │   processNetwork │
 * │   receive packet │──[Queue]────>│   update ECS     │
 * │   push to queue  │              │   render         │
 * │ }                │              │ }                │
 * └──────────────────┘              └──────────────────┘
 * ```
 * 
 * Communication flow:
 * ```
 * CLIENT                          SERVER
 *   |                               |
 *   |---[InputEvent]--------------->| (player inputs)
 *   |                               |
 *   |<--[WorldState]----------------| (authoritative state)
 *   |                               |
 *   |---[PING]--------------------->|
 *   |<--[PONG]----------------------| (latency measurement)
 * ```
 * 
 * EventBus integration:
 * - Listens to InputEvent to send them to server
 * - Publishes NetworkEvent when data arrives (from game thread)
 * - Publishes ConnectionEvent on state changes
 */
class Replicator {
   public:
    /**
     * @brief Constructor with EventBus reference
     * 
     * Initializes the Replicator and subscribes to necessary events.
     * 
     * @param eventBus Event bus for inter-component communication
     * @param isSpectator Whether this client is in spectator mode (read-only)
     * 
     * @note The Replicator automatically subscribes to InputEvent (unless spectator)
     */
    explicit Replicator(EventBus &eventBus, bool isSpectator = false);

    /**
     * @brief Destructor
     * 
     * Properly disconnects from server and frees network resources.
     */
    ~Replicator();

    /**
     * @brief Connect to the game server
     * 
     * Establishes a UDP/TCP connection with the specified server.
     * Publishes a ConnectionEvent based on the result.
     * 
     * @param host Server IP address or hostname
     * @param port Server port (typically 4242)
     * @return true if connection is established, false otherwise
     * 
     * @note Blocking until timeout (recommended: 5 seconds)
     * @note Publishes ConnectionEvent::CONNECTED or FAILED
     */
    bool connect(const std::string &host, uint16_t port);

    /**
     * @brief Disconnect from the server
     * 
     * Closes the connection cleanly and cleans up resources.
     * Publishes a ConnectionEvent::DISCONNECTED.
     */
    void disconnect();

    /**
     * @brief Check if connected to server
     * 
     * @return true if connection is active, false otherwise
     */
    bool isConnected() const;

    /**
     * @brief Check if authenticated with server
     * 
     * @return true if authentication succeeded, false otherwise
     */
    bool isAuthenticated() const;

    /**
     * @brief Get the player ID assigned by server
     * 
     * @return Player ID (0 if not yet authenticated)
     */
    uint32_t getMyPlayerId() const { return _myPlayerId.load(); }

    /**
     * @brief Process incoming network messages
     * 
     * Processes messages from the network thread queue and publishes them
     * on the EventBus. Must be called every frame from the game thread.
     * 
     * Actions performed:
     * - Pop messages from thread-safe queue
     * - Decode message content
     * - Publish NetworkEvent on EventBus
     * 
     * @note Non-blocking, processes all available messages
     * @note Thread-safe: called from game thread, reads from network thread queue
     */
    void processMessages();

    /**
     * @brief Start the dedicated network thread
     * 
     * Launches a background thread that continuously polls the network
     * for incoming packets and pushes them to the message queue.
     * 
     * @note Automatically called by connect()
     * @note Thread runs until disconnect() is called
     */
    void startNetworkThread();

    /**
     * @brief Stop the dedicated network thread
     * 
     * Signals the network thread to stop and waits for it to join.
     * 
     * @note Automatically called by disconnect()
     */
    void stopNetworkThread();

    /**
     * @brief Send a packet to the server
     * 
     * Serializes and sends a message to the server asynchronously.
     * 
     * @param type Message type (PLAYER_INPUT, PING, etc.)
     * @param data Packet data (serialized)
     * 
     * @note Non-blocking
     * @note Automatically adds a timestamp for latency measurement
     */
    void sendPacket(NetworkMessageType type, const std::vector<uint8_t> &data);

    /**
     * @brief Get current latency in milliseconds
     * 
     * Calculates average RTT (Round-Trip Time) based on PING/PONG.
     * 
     * @return Latency in milliseconds
     * 
     * @note Updated every 1 second via heartbeat
     * @note Rolling average over the last 10 pings
     */
    uint32_t getLatency() const;

    /**
     * @brief Get packet loss rate as percentage
     * 
     * Calculates the ratio of lost packets over the last 100 sent.
     * 
     * @return Loss percentage (0-100)
     * 
     * @note Based on missing ACKs
     * @note Real-time update
     */
    uint32_t getPacketLoss() const;

    /**
     * @brief Check if in spectator mode
     * 
     * @return true if this client is a spectator (read-only)
     */
    bool isSpectator() const;

    /**
     * @brief Send connect request to server with player name.
     * 
     * @param playerName The player's name/pseudo
     * @return true if the packet was sent successfully
     */
    bool sendConnectRequest(const std::string &playerName, const std::string &username,
                            const std::string &password);

    /**
     * @brief Send register account request to server.
     * 
     * @param username Username for new account
     * @param password Password for new account
     * @return true if the packet was sent successfully
     */
    bool sendRegisterAccount(const std::string &username, const std::string &password);

    /**
     * @brief Send login request to server.
     * 
     * @param username Account username
     * @param password Account password
     * @return true if the packet was sent successfully
     */
    bool sendLoginAccount(const std::string &username, const std::string &password);

    /**
     * @brief Send list rooms request to server.
     * 
     * @return true if the packet was sent successfully
     */
    bool sendListRooms();

    /**
     * @brief Send create room request to server.
     * 
     * @param roomName Name of the room to create
     * @param maxPlayers Maximum number of players
     * @param isPrivate Whether the room is private
     * @param gameSpeedMultiplier Game speed multiplier (0.25 to 1.0, default 1.0)
     * @return true if the packet was sent successfully
     */
    bool sendCreateRoom(const std::string &roomName, uint32_t maxPlayers, bool isPrivate,
                        float gameSpeedMultiplier = 1.0f);

    /**
     * @brief Send join room request to server.
     * 
     * @param roomId ID of the room to join
     * @return true if the packet was sent successfully
     */
    bool sendJoinRoom(const std::string &roomId);

    /**
     * @brief Send auto-matchmaking request to server.
     * 
     * Automatically joins the first available room or creates a new one.
     * @return true if the packet was sent successfully
     */
    bool sendAutoMatchmaking();

    /**
     * @brief Update auto-matchmaking preference on server.
     * 
     * Sends the user's auto-matchmaking preference to be saved on the server.
     * Does not trigger immediate matchmaking.
     * @param enabled Whether auto-matchmaking should be enabled
     * @return true if the packet was sent successfully
     */
    bool updateAutoMatchmakingPreference(bool enabled);

    /**
     * @brief Send start game request to server.
     * 
     * Only the host of the room can start the game.
     * @return true if the packet was sent successfully
     */
    bool sendStartGame();

    /**
     * @brief Request the list of available rooms from server
     * @return true if the packet was sent successfully
     */
    bool sendRequestRoomList();

    /**
     * @brief Send request to leave current room
     * @return true if the packet was sent successfully
     */
    bool sendLeaveRoom();

    /**
     * @brief Send chat message to server
     * 
     * Sends a chat message to be broadcast to other players in the room.
     * Messages starting with "/" are treated as commands.
     * 
     * @param message The message text to send
     * @return true if the packet was sent successfully
     */
    bool sendChatMessage(const std::string &message);

    /**
     * @brief Get the user's auto-matchmaking preference from server
     * 
     * Returns the auto-matchmaking preference loaded from the server after login.
     * This preference is persisted in the user's account on the server.
     * 
     * @return true if auto-matchmaking is enabled for this user
     */
    bool getAutoMatchmakingPreference() const { return _autoMatchmakingPreference; }

   private:
    /**
     * @brief Network thread main loop
     * 
     * Continuously polls the network for incoming packets.
     * Runs in a dedicated thread until stop is requested.
     * 
     * @param stopToken Token to check for stop requests
     */
    void networkThreadLoop(std::stop_token stopToken);

    /**
     * @brief Handle an incoming packet from the network
     * @param packet Raw packet data
     * 
     * Decodes the packet and creates a NetworkEvent.
     */
    void onInputEvent(const InputEvent &event);
    void processIncomingPacket(const std::vector<uint8_t> &packet);

    EventBus &_eventBus;
    std::atomic<bool> _connected{false};
    std::atomic<bool> _authenticated{false};
    std::atomic<uint32_t> _myPlayerId{0};
    std::string _lastLoginUsername;                       // Track username for AUTH_SUCCESS event
    std::atomic<bool> _autoMatchmakingPreference{false};  // User's auto-matchmaking preference from server
    bool _isSpectator;
    std::string _serverHost;
    uint16_t _serverPort = 0;

    std::atomic<uint32_t> _latency{0};
    uint32_t _packetLoss = 0;

    std::unique_ptr<IHost> _host;
    IPeer *_serverPeer = nullptr;

    // Multi-threading components
    std::jthread _networkThread;                      ///< Dedicated network thread
    ThreadSafeQueue<NetworkEvent> _incomingMessages;  ///< Queue for messages from network thread

    // Smoothed ping calculation (exponential moving average)
    static constexpr float PING_SMOOTHING_FACTOR = 0.3f;  // 30% new, 70% old
    std::atomic<float> _smoothedLatency{0.0f};
};

#endif
