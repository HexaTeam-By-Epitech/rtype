/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** MatchmakingService.hpp
*/

#pragma once

#include <chrono>
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <vector>
#include "server/Rooms/Matchmaking/IMatchmakingService.hpp"

namespace server {

    /**
     * @struct PlayerQueueInfo
     * @brief Information about a player in matchmaking queue
     */
    struct PlayerQueueInfo {
        uint32_t playerId;
        std::chrono::steady_clock::time_point joinTime;
        int skillRating;  // For future skill-based matchmaking
    };

    /**
     * @class MatchmakingService
     * @brief Automatic matchmaking service
     * 
     * Manages a queue of players waiting for matches and automatically creates games
     * when enough players are available (default: 2-4 players per match).
     * 
     * Features:
     * - Automatic match creation when minimum players reached
     * - Configurable min/max players per match
     * - Wait time tracking
     * - Callback notification when match is created
     */
    class MatchmakingService : public IMatchmakingService {
       public:
        /**
         * @brief Construct matchmaking service
         * @param minPlayers Minimum players to start a match (default: 2)
         * @param maxPlayers Maximum players per match (default: 4)
         */
        explicit MatchmakingService(size_t minPlayers = 2, size_t maxPlayers = 4);
        ~MatchmakingService() override = default;

        void addPlayer(uint32_t playerId) override;
        void removePlayer(uint32_t playerId) override;
        void tick() override;
        size_t getQueueSize() const override;
        void setMatchCreatedCallback(MatchCreatedCallback callback) override;

        /**
         * @brief Get list of waiting players
         * @return Vector of player queue info
         */
        std::vector<PlayerQueueInfo> getWaitingPlayers() const;

        /**
         * @brief Set minimum players required to start a match
         * @param min Minimum player count (must be >= 1 and <= maxPlayers)
         */
        void setMinPlayers(size_t min);

        /**
         * @brief Set maximum players per match
         * @param max Maximum player count (must be >= minPlayers)
         */
        void setMaxPlayers(size_t max);

        /**
         * @brief Get statistics about matchmaking
         * @return String with statistics
         */
        std::string getStatistics() const;

       private:
        /**
         * @brief Try to create a match from waiting players
         * @return true if a match was created
         */
        bool _tryCreateMatch();

        /**
         * @brief Generate unique room ID for a new match
         * @return Room ID string
         */
        std::string _generateRoomId();

        size_t _minPlayers;  ///< Minimum players to start a match
        size_t _maxPlayers;  ///< Maximum players per match

        std::vector<PlayerQueueInfo> _waitingPlayers;  ///< Queue of players
        mutable std::mutex _mutex;                     ///< Thread safety

        MatchCreatedCallback _matchCreatedCallback;  ///< Callback when match created

        // Statistics
        uint32_t _totalMatchesCreated{0};
        uint32_t _totalPlayersMatched{0};
    };

}  // namespace server
