/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** MatchmakingService.cpp
*/

#include "server/Rooms/Matchmaking/MatchmakingService.hpp"
#include <algorithm>
#include <sstream>
#include "common/Logger/Logger.hpp"
#include "server/Rooms/Room.hpp"

namespace server {

    MatchmakingService::MatchmakingService(size_t minPlayers, size_t maxPlayers,
                                           std::shared_ptr<EventBus> eventBus)
        : _minPlayers(minPlayers), _maxPlayers(maxPlayers), _eventBus(eventBus) {
        if (_minPlayers < 1) {
            _minPlayers = 1;
        }
        if (_maxPlayers < _minPlayers) {
            _maxPlayers = _minPlayers;
        }
        LOG_INFO("MatchmakingService created (min: ", _minPlayers, ", max: ", _maxPlayers, " players)");
    }

    void MatchmakingService::addPlayer(uint32_t playerId) {
        std::lock_guard<std::mutex> lock(_mutex);

        // Check if player is already in queue
        auto it = std::find_if(_waitingPlayers.begin(), _waitingPlayers.end(),
                               [playerId](const PlayerQueueInfo &info) { return info.playerId == playerId; });

        if (it != _waitingPlayers.end()) {
            LOG_WARNING("Player ", playerId, " is already in matchmaking queue");
            return;
        }

        PlayerQueueInfo info;
        info.playerId = playerId;
        info.joinTime = std::chrono::steady_clock::now();

        _waitingPlayers.push_back(info);
        LOG_INFO("✓ Player ", playerId, " added to matchmaking queue (", _waitingPlayers.size(),
                 " players waiting)");
    }

    void MatchmakingService::removePlayer(uint32_t playerId) {
        std::lock_guard<std::mutex> lock(_mutex);

        auto it = std::find_if(_waitingPlayers.begin(), _waitingPlayers.end(),
                               [playerId](const PlayerQueueInfo &info) { return info.playerId == playerId; });

        if (it != _waitingPlayers.end()) {
            _waitingPlayers.erase(it);
            LOG_INFO("✓ Player ", playerId, " removed from matchmaking queue (", _waitingPlayers.size(),
                     " players remaining)");
        } else {
            LOG_WARNING("Player ", playerId, " not found in matchmaking queue");
        }
    }

    void MatchmakingService::tick() {
        std::lock_guard<std::mutex> lock(_mutex);

        // Try to create matches while we have enough players
        while (_waitingPlayers.size() >= _minPlayers) {
            if (!_tryCreateMatch()) {
                break;  // Could not create match, stop trying
            }
        }
    }

    bool MatchmakingService::_tryCreateMatch() {
        if (_waitingPlayers.size() < _minPlayers) {
            return false;
        }

        // Determine match size (up to maxPlayers)
        size_t matchSize = std::min(_waitingPlayers.size(), _maxPlayers);

        // Extract players for this match
        std::vector<uint32_t> matchedPlayers;
        matchedPlayers.reserve(matchSize);

        for (size_t i = 0; i < matchSize; ++i) {
            matchedPlayers.push_back(_waitingPlayers[i].playerId);
        }

        // Remove matched players from queue
        _waitingPlayers.erase(_waitingPlayers.begin(), _waitingPlayers.begin() + matchSize);

        // Create room for the match
        std::string roomId = _generateRoomId();
        std::shared_ptr<Room> room;

        try {
            room = std::make_shared<Room>(roomId, "Match #" + std::to_string(_totalMatchesCreated + 1),
                                          _maxPlayers, false, 1.0f, _eventBus);
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create match room: ", e.what());
            // Re-add players to waiting queue
            for (uint32_t playerId : matchedPlayers) {
                _waitingPlayers.push_back({playerId, std::chrono::steady_clock::now()});
            }
            return false;
        }

        // Add all matched players to the room
        for (uint32_t playerId : matchedPlayers) {
            room->join(playerId);
        }

        // Update statistics
        _totalMatchesCreated++;
        _totalPlayersMatched += matchedPlayers.size();

        LOG_INFO("✓ Match created: Room '", roomId, "' with ", matchedPlayers.size(), " players");

        // Log player list
        std::ostringstream playerList;
        for (size_t i = 0; i < matchedPlayers.size(); ++i) {
            if (i > 0)
                playerList << ", ";
            playerList << matchedPlayers[i];
        }
        LOG_INFO("  Players: ", playerList.str());

        // Notify via callback if set
        if (_matchCreatedCallback) {
            _matchCreatedCallback(room);
        }

        return true;
    }

    std::string MatchmakingService::_generateRoomId() {
        return "match_" + std::to_string(_totalMatchesCreated);
    }

    size_t MatchmakingService::getQueueSize() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _waitingPlayers.size();
    }

    void MatchmakingService::setMatchCreatedCallback(MatchCreatedCallback callback) {
        _matchCreatedCallback = callback;
    }

    std::pair<std::shared_ptr<Room>, bool> MatchmakingService::findOrCreateMatch(
        uint32_t playerId, const std::vector<std::shared_ptr<Room>> &availableRooms, bool allowSpectator) {

        std::lock_guard<std::mutex> lock(_mutex);

        LOG_INFO("[MatchmakingService] Finding match for player ", playerId);

        // STRATEGY 1: Try to find a waiting room (instant join - best UX)
        for (const auto &room : availableRooms) {
            if (room->getState() == RoomState::WAITING && !room->isFull()) {
                LOG_INFO("[MatchmakingService] Found waiting room '", room->getId(), "' for player ",
                         playerId);
                return {room, false};  // Join as player
            }
        }

        // STRATEGY 2: If no waiting room and spectator allowed, try to spectate an in-progress game
        if (allowSpectator) {
            for (const auto &room : availableRooms) {
                if (room->getState() == RoomState::IN_PROGRESS) {
                    LOG_INFO("[MatchmakingService] No waiting rooms, player ", playerId, " will spectate '",
                             room->getId(), "'");
                    return {room, true};  // Join as spectator
                }
            }
        }

        // STRATEGY 3: No immediate match available, add to queue
        LOG_INFO("[MatchmakingService] No immediate match, adding player ", playerId, " to queue");

        // Check if player is already in queue
        auto it = std::find_if(_waitingPlayers.begin(), _waitingPlayers.end(),
                               [playerId](const PlayerQueueInfo &info) { return info.playerId == playerId; });

        if (it == _waitingPlayers.end()) {
            PlayerQueueInfo info;
            info.playerId = playerId;
            info.joinTime = std::chrono::steady_clock::now();
            _waitingPlayers.push_back(info);
            LOG_INFO("✓ Player ", playerId, " added to matchmaking queue (", _waitingPlayers.size(),
                     " players waiting)");
        }

        return {nullptr, false};  // No immediate match, player is in queue
    }

    std::vector<PlayerQueueInfo> MatchmakingService::getWaitingPlayers() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _waitingPlayers;
    }

    void MatchmakingService::setMinPlayers(size_t min) {
        std::lock_guard<std::mutex> lock(_mutex);
        if (min >= 1 && min <= _maxPlayers) {
            _minPlayers = min;
            LOG_INFO("Matchmaking min players set to ", min);
        }
    }

    void MatchmakingService::setMaxPlayers(size_t max) {
        std::lock_guard<std::mutex> lock(_mutex);
        if (max >= _minPlayers) {
            _maxPlayers = max;
            LOG_INFO("Matchmaking max players set to ", max);
        }
    }

    std::string MatchmakingService::getStatistics() const {
        std::lock_guard<std::mutex> lock(_mutex);
        std::ostringstream oss;
        oss << "Matchmaking Statistics:\n";
        oss << "  Players in queue: " << _waitingPlayers.size() << "\n";
        oss << "  Total matches created: " << _totalMatchesCreated << "\n";
        oss << "  Total players matched: " << _totalPlayersMatched << "\n";
        oss << "  Min/Max players per match: " << _minPlayers << "/" << _maxPlayers;
        return oss.str();
    }

}  // namespace server
