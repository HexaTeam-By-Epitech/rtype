/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** Replicator.hpp
*/

#ifndef REPLICATOR_HPP
#define REPLICATOR_HPP

#include <string>
#include <cstdint>
#include <functional>
#include <vector>
#include "../Core/EventBus/EventBus.hpp"
#include "../Events/NetworkEvent/NetworkEvent.hpp"
#include "../Events/InputEvent/InputEvent.hpp"

/**
 * @class Replicator
 * @brief Client-server network replication manager
 * 
 * The Replicator is responsible for:
 * - UDP/TCP communication with the server
 * - Sending player inputs to the server
 * - Receiving world state from the server
 * - Measuring latency and packet loss
 * - Publishing network events on the EventBus
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
 * - Publishes NetworkEvent when data arrives
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
     * 
     * @note The Replicator automatically subscribes to InputEvent
     */
    explicit Replicator(EventBus& eventBus);
    
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
    bool connect(const std::string& host, uint16_t port);
    
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
     * @brief Update the network system
     * 
     * Processes incoming packets, handles heartbeat, and measures latency.
     * Must be called every frame of the game loop.
     * 
     * Actions performed:
     * - Reading pending UDP packets
     * - Processing and publishing NetworkEvent
     * - Sending heartbeat if necessary
     * - Updating network statistics
     * 
     * @note Non-blocking (asynchronous reading)
     */
    void update();
    
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
    void sendPacket(NetworkMessageType type, const std::vector<uint8_t>& data);
    
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

private:
    void onInputEvent(const InputEvent& event);
    void processIncomingPacket(const std::vector<uint8_t>& packet);

    EventBus& _eventBus;
    bool _connected = false;
    std::string _serverHost;
    uint16_t _serverPort = 0;

    uint32_t _latency = 0;
    uint32_t _packetLoss = 0;
    
    // TODO: Ajouter socket UDP/TCP
};

#endif