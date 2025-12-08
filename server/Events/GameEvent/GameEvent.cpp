/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** GameEvent.cpp
*/

#include "GameEvent.hpp"

namespace server {

    GameEvent::GameEvent(Type type) : _type(type), _timestamp(std::chrono::system_clock::now()) {}

    GameEvent::Type GameEvent::getType() const {
        return _type;
    }

    std::chrono::system_clock::time_point GameEvent::getTimestamp() const {
        return _timestamp;
    }

}  // namespace server
