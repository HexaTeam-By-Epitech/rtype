/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** GameEvent.cpp
*/

#include "server/Events/GameEvent/GameEvent.hpp"

namespace server {

    GameEvent::GameEvent(Type type) : _type(type) {}

    GameEvent::Type GameEvent::getType() const {
        return _type;
    }

}  // namespace server
