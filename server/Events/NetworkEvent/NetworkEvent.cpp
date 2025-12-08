/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** NetworkEvent.cpp
*/

#include "NetworkEvent.hpp"

namespace server {

    NetworkEvent::NetworkEvent(Type type, int clientId)
        : _type(type), _clientId(clientId), _timestamp(std::chrono::system_clock::now()) {}

    NetworkEvent::Type NetworkEvent::getType() const {
        return _type;
    }

    int NetworkEvent::getClientId() const {
        return _clientId;
    }

    std::chrono::system_clock::time_point NetworkEvent::getTimestamp() const {
        return _timestamp;
    }

}  // namespace server
