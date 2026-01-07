/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** MatchmakingService.cpp
*/

#include "server/Rooms/Matchmaking/MatchmakingService.hpp"
#include <algorithm>
#include "common/Logger/Logger.hpp"

namespace server {

    void MatchmakingService::addPlayer(int playerId) {
        // Check if player is already in queue
        auto it = std::find(_waitingPlayers.begin(), _waitingPlayers.end(), playerId);
        if (it != _waitingPlayers.end()) {
            LOG_WARNING("Player ", playerId, " is already in matchmaking queue");
            return;
        }

        _waitingPlayers.push_back(playerId);
        LOG_INFO("Player ", playerId, " added to matchmaking queue (", _waitingPlayers.size(),
                 " players waiting)");

        // Auto-start match if we have enough players
        if (_waitingPlayers.size() >= PLAYERS_PER_MATCH) {
            startMatch();
        }
    }

    void MatchmakingService::removePlayer(int playerId) {
        auto it = std::find(_waitingPlayers.begin(), _waitingPlayers.end(), playerId);
        if (it != _waitingPlayers.end()) {
            _waitingPlayers.erase(it);
            LOG_INFO("Player ", playerId, " removed from matchmaking queue (", _waitingPlayers.size(),
                     " players remaining)");
        } else {
            LOG_WARNING("Player ", playerId, " not found in matchmaking queue");
        }
    }

    void MatchmakingService::startMatch() {
        if (_waitingPlayers.empty()) {
            LOG_WARNING("Cannot start match: no players in queue");
            return;
        }

        // Take up to PLAYERS_PER_MATCH players for a match
        size_t matchSize = std::min(_waitingPlayers.size(), PLAYERS_PER_MATCH);
        std::vector matchPlayers(_waitingPlayers.begin(), _waitingPlayers.begin() + matchSize);

        // Remove matched players from queue
        _waitingPlayers.erase(_waitingPlayers.begin(), _waitingPlayers.begin() + matchSize);

        // Create and store the match
        int matchId = _nextMatchId++;
        _activeMatches[matchId] = matchPlayers;

        LOG_INFO("Match ", matchId, " created with ", matchPlayers.size(), " players");
        LOG_INFO("  Players: ", [&matchPlayers]() {
            std::string playerList;
            for (size_t i = 0; i < matchPlayers.size(); ++i) {
                if (i > 0)
                    playerList += ", ";
                playerList += std::to_string(matchPlayers[i]);
            }
            return playerList;
        }());

        // In a complete implementation, this would:
        // 1. Create a Room instance
        // 2. Notify players that a match was found
        // 3. Transfer players to the game room
    }

    std::vector<int> MatchmakingService::getWaitingPlayers() const {
        return _waitingPlayers;
    }

    size_t MatchmakingService::getQueueSize() const {
        return _waitingPlayers.size();
    }

    const std::unordered_map<int, std::vector<int>> &MatchmakingService::getActiveMatches() const {
        return _activeMatches;
    }

}  // namespace server
