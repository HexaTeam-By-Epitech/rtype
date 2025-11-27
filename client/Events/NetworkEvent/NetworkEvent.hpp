/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** NetworkEvent.hpp
*/

#ifndef NETWORKEVENT_HPP
#define NETWORKEVENT_HPP

#include "../IEvent.hpp"
#include <vector>
#include <cstdint>

enum class NetworkMessageType {
    CONNECT,
    DISCONNECT,
    PLAYER_INPUT,
    WORLD_STATE,
    SPAWN_ENTITY,
    DESTROY_ENTITY,
    PING,
    PONG
};

class NetworkEvent : public IEvent {
public:
    NetworkEvent(NetworkMessageType type, std::vector<uint8_t> data)
        : _type(type), _data(std::move(data)), _timestamp(0) {}

    NetworkMessageType getType() const { return _type; }
    const std::vector<uint8_t>& getData() const { return _data; }
    uint64_t getTimestamp() const { return _timestamp; }
    
    void setTimestamp(uint64_t timestamp) { _timestamp = timestamp; }

private:
    NetworkMessageType _type;
    std::vector<uint8_t> _data;
    uint64_t _timestamp;
};

class ConnectionEvent : public IEvent {
public:
    enum class Status {
        CONNECTED,
        DISCONNECTED,
        FAILED
    };

    explicit ConnectionEvent(Status status, const std::string& message = "")
        : _status(status), _message(message) {}

    Status getStatus() const { return _status; }
    const std::string& getMessage() const { return _message; }

private:
    Status _status;
    std::string _message;
};

#endif