/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** Room.cpp
*/

#include "Room.hpp"
#include <algorithm>
#include "../../common/Logger/Logger.hpp"

namespace server {

    void Room::join(int playerId) {
        // Check if player already in room
        auto it = std::find(_players.begin(), _players.end(), playerId);
        if (it != _players.end()) {
            LOG_WARNING("Player ", playerId, " already in room ", _id);
            return;
        }

        _players.push_back(playerId);
        LOG_INFO("✓ Player ", playerId, " joined room ", _id, " (", _players.size(), " players)");
    }

    void Room::leave(int playerId) {
        auto it = std::find(_players.begin(), _players.end(), playerId);
        if (it != _players.end()) {
            _players.erase(it);
            LOG_INFO("✓ Player ", playerId, " left room ", _id, " (", _players.size(), " players remaining)");
        }
    }

    std::string Room::getId() const {
        return _id;
    }

}  // namespace server
