/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** RoomManager.cpp
*/

#include "server/Rooms/RoomManager/RoomManager.hpp"
#include "common/Logger/Logger.hpp"

namespace server {

    void RoomManager::createRoom(const std::string &id) {
        if (_rooms.find(id) != _rooms.end()) {
            LOG_WARNING("Room ", id, " already exists");
            return;
        }

        auto room = std::make_shared<Room>(id);
        _rooms[id] = room;

        LOG_INFO("✓ Room created: ", id);
    }

    std::shared_ptr<Room> RoomManager::getRoom(const std::string &id) {
        auto it = _rooms.find(id);
        if (it != _rooms.end()) {
            return it->second;
        }

        LOG_WARNING("Room ", id, " not found");
        return nullptr;
    }

    void RoomManager::removeRoom(const std::string &id) {
        auto it = _rooms.find(id);
        if (it != _rooms.end()) {
            _rooms.erase(it);
            LOG_INFO("✓ Room removed: ", id);
        }
    }

}  // namespace server
