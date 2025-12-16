/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** MatchmakingService.hpp
*/

#pragma once

#include <unordered_map>
#include <vector>
#include "server/Rooms/Matchmaking/IMatchmakingService.hpp"

namespace server {

    /**
     * @class MatchmakingService
     * @brief Concrete implementation of IMatchmakingService
     * 
     * Manages a queue of players waiting for matches and creates games
     * when enough players are available (default: 4 players per match).
     */
    class MatchmakingService : public IMatchmakingService {
       public:
        MatchmakingService() = default;
        ~MatchmakingService() override = default;

        /**
         * @brief Add a player to the matchmaking queue.
         * @param playerId The player ID to add
         */
        void addPlayer(int playerId) override;

        /**
         * @brief Remove a player from the matchmaking queue.
         * @param playerId The player ID to remove
         */
        void removePlayer(int playerId) override;

        /**
         * @brief Start a match with available players (creates match with up to 4 players).
         */
        void startMatch() override;

        /**
         * @brief Get the list of players currently waiting.
         * @return std::vector<int> List of waiting player IDs
         */
        std::vector<int> getWaitingPlayers() const;

        /**
         * @brief Get the number of players in queue.
         * @return size_t Queue size
         */
        size_t getQueueSize() const;

        /**
         * @brief Get all active matches.
         * @return const std::unordered_map<int, std::vector<int>>& Map of match ID to player IDs
         */
        const std::unordered_map<int, std::vector<int>> &getActiveMatches() const;

       private:
        static constexpr size_t PLAYERS_PER_MATCH = 4;  ///< Number of players required for a match

        std::vector<int> _waitingPlayers;                          ///< Queue of players waiting for a match
        std::unordered_map<int, std::vector<int>> _activeMatches;  ///< Active matches (matchId -> playerIds)
        int _nextMatchId = 0;                                      ///< Next available match ID
    };

}  // namespace server
