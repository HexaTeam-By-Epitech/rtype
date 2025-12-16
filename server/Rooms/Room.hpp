/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** Room.hpp
*/

#pragma once

#include <vector>
#include "server/Rooms/IRoom.hpp"

namespace server {

    /**
     * @class Room
     * @brief Concrete implementation of IRoom
     */
    class Room : public IRoom {
       public:
        explicit Room(const std::string &id) : _id(id) {}
        ~Room() override = default;

        void join(int playerId) override;
        void leave(int playerId) override;
        std::string getId() const override;

       private:
        std::string _id;
        std::vector<int> _players;
    };

}  // namespace server
