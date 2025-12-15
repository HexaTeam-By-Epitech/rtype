/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** Session.cpp
*/

#include "server/Sessions/Session/Session.hpp"

namespace server {

    Session::Session(const std::string &id) : _id(id), _playerId(0), _active(true) {}

    std::string Session::getId() const {
        return _id;
    }

    bool Session::isActive() const {
        return _active;
    }

    uint32_t Session::getPlayerId() const {
        return _playerId;
    }

    void Session::setPlayerId(uint32_t playerId) {
        _playerId = playerId;
    }

    void Session::setActive(bool active) {
        _active = active;
    }

}  // namespace server
