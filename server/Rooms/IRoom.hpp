/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** IRoom.hpp
*/

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace server {

    /**
     * @enum RoomState
     * @brief State of a game room
     */
    enum class RoomState {
        WAITING,      // Waiting for players
        STARTING,     // Countdown before game starts
        IN_PROGRESS,  // Game is running
        FINISHED      // Game ended
    };

    /**
     * @struct RoomInfo
     * @brief Information about a room
     */
    struct RoomInfo {
        std::string id;
        std::string name;
        RoomState state;
        size_t currentPlayers;
        size_t maxPlayers;
        bool isPrivate;
        std::string hostPlayerId;
    };

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
         * @return true if player successfully joined, false otherwise
         */
        virtual bool join(uint32_t playerId) = 0;

        /**
         * @brief Remove a player from the room
         * @param playerId Unique identifier of the player
         * @return true if player successfully left, false otherwise
         */
        virtual bool leave(uint32_t playerId) = 0;

        /**
         * @brief Get the room's unique identifier
         * @return Room ID
         */
        virtual std::string getId() const = 0;

        /**
         * @brief Get the room's display name
         * @return Room name
         */
        virtual std::string getName() const = 0;

        /**
         * @brief Get current state of the room
         * @return RoomState
         */
        virtual RoomState getState() const = 0;

        /**
         * @brief Set the state of the room
         * @param state New state
         */
        virtual void setState(RoomState state) = 0;

        /**
         * @brief Get number of players currently in room
         * @return Player count
         */
        virtual size_t getPlayerCount() const = 0;

        /**
         * @brief Get maximum number of players allowed
         * @return Max players
         */
        virtual size_t getMaxPlayers() const = 0;

        /**
         * @brief Check if room is full
         * @return true if full, false otherwise
         */
        virtual bool isFull() const = 0;

        /**
         * @brief Get list of player IDs in this room
         * @return Vector of player IDs
         */
        virtual std::vector<uint32_t> getPlayers() const = 0;

        /**
         * @brief Check if a player is in this room
         * @param playerId Player ID to check
         * @return true if player is in room
         */
        virtual bool hasPlayer(uint32_t playerId) const = 0;

        /**
         * @brief Get room information
         * @return RoomInfo struct
         */
        virtual RoomInfo getInfo() const = 0;
    };

}  // namespace server
