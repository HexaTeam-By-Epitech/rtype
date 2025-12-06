/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** IRoom.hpp
*/

#pragma once

#include <string>

namespace server {

/**
 * @class IRoom
 * @brief Interface for a game room
 *
 * Defines basic operations for a game room like joining, leaving, and retrieving information.
 */
class IRoom {
public:
    virtual ~IRoom() = default;

    /**
     * @brief Join a player to the room
     * @param playerId Unique identifier of the player
     */
    virtual void join(int playerId) = 0;

    /**
     * @brief Remove a player from the room
     * @param playerId Unique identifier of the player
     */
    virtual void leave(int playerId) = 0;

    /**
     * @brief Get the room's unique identifier
     * @return Room ID
     */
    virtual std::string getId() const = 0;
};

} // namespace server

