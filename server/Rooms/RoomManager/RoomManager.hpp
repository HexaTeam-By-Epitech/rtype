/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** RoomManager.hpp
*/

#pragma once

#include "IRoomManager.hpp"
#include <unordered_map>

namespace server {

/**
 * @class RoomManager
 * @brief Concrete implementation of IRoomManager
 */
class RoomManager : public IRoomManager {
public:
    RoomManager() = default;
    ~RoomManager() override = default;

    void createRoom(const std::string &id) override;
    std::shared_ptr<Room> getRoom(const std::string &id) override;
    void removeRoom(const std::string &id) override;

private:
    std::unordered_map<std::string, std::shared_ptr<Room>> _rooms;
};

} // namespace server

