/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** IMatchmakingService.hpp
*/

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace server {
    class Room;

    /**
     * @typedef MatchCreatedCallback
     * @brief Callback invoked when a match is created
     * @param room Newly created room with matched players
     */
    using MatchCreatedCallback = std::function<void(std::shared_ptr<Room>)>;

    /**
     * @class IMatchmakingService
     * @brief Interface for a matchmaking system
     */
    class IMatchmakingService {
       public:
        virtual ~IMatchmakingService() = default;

        /**
         * @brief Add a player to the matchmaking queue
         * @param playerId Player ID to add
         */
        virtual void addPlayer(uint32_t playerId) = 0;

        /**
         * @brief Remove a player from the matchmaking queue
         * @param playerId Player ID to remove
         */
        virtual void removePlayer(uint32_t playerId) = 0;

        /**
         * @brief Process matchmaking queue and create matches
         * Called periodically by the server
         */
        virtual void tick() = 0;

        /**
         * @brief Get the number of players waiting in queue
         * @return Queue size
         */
        virtual size_t getQueueSize() const = 0;

        /**
         * @brief Set callback for when a match is created
         * @param callback Function to call when match is ready
         */
        virtual void setMatchCreatedCallback(MatchCreatedCallback callback) = 0;

        /**
         * @brief Find an available room or add player to matchmaking queue
         * 
         * Implements intelligent matchmaking strategy:
         * 1. Try to find a waiting room (instant join)
         * 2. If no waiting room, join as spectator to in-progress game (if allowed)
         * 3. If no matches available, add to queue for future match creation
         * 
         * @param playerId Player ID requesting matchmaking
         * @param availableRooms List of currently available rooms to search
         * @param allowSpectator Whether player can join as spectator if no waiting rooms
         * @return Pair of (room, isSpectator) if immediate match found, (nullptr, false) if added to queue
         */
        virtual std::pair<std::shared_ptr<Room>, bool> findOrCreateMatch(
            uint32_t playerId, const std::vector<std::shared_ptr<Room>> &availableRooms,
            bool allowSpectator = true) = 0;
    };

}  // namespace server
