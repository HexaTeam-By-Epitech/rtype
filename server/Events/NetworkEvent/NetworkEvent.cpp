/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** NetworkEvent.cpp
*/

#include "server/Events/NetworkEvent/NetworkEvent.hpp"

namespace server {

    NetworkEvent::NetworkEvent(Type type, int clientId) : _type(type), _clientId(clientId) {}

    NetworkEvent::Type NetworkEvent::getType() const {
        return _type;
    }

    int NetworkEvent::getClientId() const {
        return _clientId;
    }

}  // namespace server
