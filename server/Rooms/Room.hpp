/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** Room.hpp
*/

#pragma once

#include "IRoom.hpp"
#include <vector>

namespace server {

/**
 * @class Room
 * @brief Concrete implementation of IRoom
 */
class Room : public IRoom {
public:
    Room(const std::string &id) : _id(id) {}
    ~Room() override = default;

    void join(int playerId) override;
    void leave(int playerId) override;
    std::string getId() const override;

private:
    std::string _id;
    std::vector<int> _players;
};

} // namespace server

