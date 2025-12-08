/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** IRoomManager.hpp
*/

#pragma once

#include <memory>
#include <string>
#include "Room.hpp"

namespace server {

    /**
     * @class IRoomManager
     * @brief Interface for managing game rooms
     */
    class IRoomManager {
       public:
        virtual ~IRoomManager() = default;

        /**
         * @brief Create a new room
         * @param id Room identifier
         */
        virtual void createRoom(const std::string &id) = 0;

        /**
         * @brief Retrieve a room by ID
         * @param id Room identifier
         * @return Shared pointer to the room
         */
        virtual std::shared_ptr<Room> getRoom(const std::string &id) = 0;

        /**
         * @brief Remove a room by ID
         * @param id Room identifier
         */
        virtual void removeRoom(const std::string &id) = 0;
    };

}  // namespace server
