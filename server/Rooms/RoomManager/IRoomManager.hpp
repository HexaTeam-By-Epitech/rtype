/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** IRoomManager.hpp
*/

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "server/Rooms/Room.hpp"

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
         * @param name Display name (optional)
         * @param maxPlayers Maximum players (default: 4)
         * @param isPrivate Whether room is private (default: false)
         * @return Shared pointer to created room
         */
        virtual std::shared_ptr<Room> createRoom(const std::string &id, const std::string &name = "",
                                                 size_t maxPlayers = 4, bool isPrivate = false) = 0;

        /**
         * @brief Retrieve a room by ID
         * @param id Room identifier
         * @return Shared pointer to the room (nullptr if not found)
         */
        virtual std::shared_ptr<Room> getRoom(const std::string &id) = 0;

        /**
         * @brief Remove a room by ID
         * @param id Room identifier
         * @return true if room was removed
         */
        virtual bool removeRoom(const std::string &id) = 0;

        /**
         * @brief Get all active rooms
         * @return Vector of room pointers
         */
        virtual std::vector<std::shared_ptr<Room>> getAllRooms() = 0;

        /**
         * @brief Get all public rooms (not private)
         * @return Vector of public room pointers
         */
        virtual std::vector<std::shared_ptr<Room>> getPublicRooms() = 0;

        /**
         * @brief Get number of active rooms
         * @return Room count
         */
        virtual size_t getRoomCount() const = 0;

        /**
         * @brief Update all rooms (called by server loop)
         * @param deltaTime Time since last update
         * @return true if room list changed (e.g. room removed)
         */
        virtual bool update(float deltaTime) = 0;
    };

}  // namespace server
