/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** NetworkEvent.hpp
*/

#ifndef NETWORKEVENT_HPP
#define NETWORKEVENT_HPP

#include <cstdint>
#include <string>
#include <vector>
#include "Events/IEvent.hpp"

/**
 * @enum NetworkMessageType
 * @brief Types of network messages exchanged between client and server
 * 
 * Defines all possible message types in the R-Type network protocol.
 */
enum class NetworkMessageType {
    CONNECT,         ///< Client connection request
    DISCONNECT,      ///< Client disconnection notification
    PLAYER_INPUT,    ///< Player input data from client
    WORLD_STATE,     ///< Authoritative world state from server
    SPAWN_ENTITY,    ///< Spawn a new entity (enemy, projectile, etc.)
    DESTROY_ENTITY,  ///< Remove an entity from the world
    PING,            ///< Latency measurement request
    PONG             ///< Latency measurement response
};

/**
 * @class NetworkEvent
 * @brief Event representing a network message
 * 
 * NetworkEvent encapsulates network messages received from or sent to the server:
 * - Message type (WORLD_STATE, PLAYER_INPUT, etc.)
 * - Raw binary data payload
 * - Decoded message content (for convenience)
 * - Timestamp for latency measurement
 * 
 * Flow:
 * - Replicator receives UDP packet → Creates NetworkEvent → Publishes on EventBus
 * - Game systems subscribe to NetworkEvent → Process data → Update game state
 * 
 * @note Data is stored as raw bytes; interpretation depends on message type
 * @note Timestamp is set by Replicator for RTT calculation
 */
class NetworkEvent : public IEvent {
   public:
    /**
     * @brief Construct a network event
     * 
     * @param type Type of network message
     * @param data Raw binary data payload
     */
    NetworkEvent(NetworkMessageType type, std::vector<uint8_t> data)
        : _type(type), _data(std::move(data)), _timestamp(0) {}

    /**
     * @brief Get the message type
     * @return Type of network message
     */
    NetworkMessageType getType() const { return _type; }

    /**
     * @brief Get the message data
     * @return Reference to binary data payload
     */
    const std::vector<uint8_t> &getData() const { return _data; }

    /**
     * @brief Get the timestamp
     * @return Timestamp when message was received (milliseconds)
     */
    uint64_t getTimestamp() const { return _timestamp; }

    /**
     * @brief Set the timestamp
     * @param timestamp Time when message was received
     */
    void setTimestamp(uint64_t timestamp) { _timestamp = timestamp; }

    /**
     * @brief Get decoded message content (for convenience)
     * @return Decoded message string
     */
    const std::string &getMessageContent() const { return _messageContent; }

    /**
     * @brief Set decoded message content
     * @param content Decoded message
     */
    void setMessageContent(const std::string &content) { _messageContent = content; }

   private:
    NetworkMessageType _type;     ///< Type of network message
    std::vector<uint8_t> _data;   ///< Raw binary data payload
    uint64_t _timestamp;          ///< Timestamp for latency measurement
    std::string _messageContent;  ///< Decoded message content (optional, for convenience)
};

/**
 * @class ConnectionEvent
 * @brief Event representing a connection state change
 * 
 * ConnectionEvent signals changes in the client-server connection:
 * - Successfully connected to server
 * - Disconnected from server
 * - Connection failed
 * 
 * Published by Replicator when connection state changes.
 * Subscribed by UI systems to show connection status.
 * 
 * @note Contains optional error message for failed connections
 */
class ConnectionEvent : public IEvent {
   public:
    /**
     * @enum Status
     * @brief Connection status states
     */
    enum class Status {
        CONNECTED,     ///< Successfully connected to server
        DISCONNECTED,  ///< Disconnected from server
        FAILED         ///< Connection attempt failed
    };

    /**
     * @brief Construct a connection event
     * 
     * @param status Connection status
     * @param message Optional status message (e.g., error description)
     */
    explicit ConnectionEvent(Status status, const std::string &message = "")
        : _status(status), _message(message) {}

    /**
     * @brief Get the connection status
     * @return Current connection status
     */
    Status getStatus() const { return _status; }

    /**
     * @brief Get the status message
     * @return Optional message (empty if no message)
     */
    const std::string &getMessage() const { return _message; }

   private:
    Status _status;        ///< Connection status
    std::string _message;  ///< Optional status message
};

#endif
