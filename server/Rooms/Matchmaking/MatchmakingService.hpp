/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** MatchmakingService.hpp
*/

#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include "server/Rooms/Matchmaking/IMatchmakingService.hpp"

namespace server {
    class EventBus;  // Forward declaration

    /**
     * @struct PlayerQueueInfo
     * @brief Information about a player in matchmaking queue
     */
    struct PlayerQueueInfo {
        uint32_t playerId;
        std::chrono::steady_clock::time_point joinTime;
    };

    /**
     * @class MatchmakingService
     * @brief Automatic matchmaking service
     * 
     * Manages matchmaking for players in two modes:
     * 
     * 1. **Manual Matchmaking** (queue-based):
     *    - Players are added to a waiting queue via addPlayer()
     *    - tick() is called periodically to create matches when min players reached
     *    - Creates balanced matches (2-4 players)
     * 
     * 2. **Auto-Matchmaking** (instant + queue):
     *    - findOrCreateMatch() tries to find an existing room first (instant join)
     *    - If no room available, automatically adds to queue
     *    - Provides best user experience with minimal wait time
     * 
     * Features:
     * - Automatic match creation when minimum players reached
     * - Configurable min/max players per match
     * - Wait time tracking
     * - Callback notification when match is created
     * - Thread-safe operations
     */
    class MatchmakingService : public IMatchmakingService {
       public:
        /**
         * @brief Construct matchmaking service
         * @param minPlayers Minimum players to start a match (default: 2)
         * @param maxPlayers Maximum players per match (default: 4)
         * @param eventBus Shared event bus for server-wide events
         */
        explicit MatchmakingService(size_t minPlayers = 2, size_t maxPlayers = 4,
                                    std::shared_ptr<server::EventBus> eventBus = nullptr);
        ~MatchmakingService() override = default;

        void addPlayer(uint32_t playerId) override;
        void removePlayer(uint32_t playerId) override;
        void tick() override;
        size_t getQueueSize() const override;
        void setMatchCreatedCallback(MatchCreatedCallback callback) override;
        std::pair<std::shared_ptr<Room>, bool> findOrCreateMatch(
            uint32_t playerId, const std::vector<std::shared_ptr<Room>> &availableRooms,
            bool allowSpectator = true) override;

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

        size_t _minPlayers;
        size_t _maxPlayers;
        std::shared_ptr<server::EventBus> _eventBus;

        std::vector<PlayerQueueInfo> _waitingPlayers;
        mutable std::mutex _mutex;

        MatchCreatedCallback _matchCreatedCallback;

        // Statistics
        uint32_t _totalMatchesCreated{0};
        uint32_t _totalPlayersMatched{0};
    };

}  // namespace server
