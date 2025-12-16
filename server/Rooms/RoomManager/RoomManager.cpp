/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** RoomManager.cpp
*/

#include "server/Rooms/RoomManager/RoomManager.hpp"
#include "common/Logger/Logger.hpp"

namespace server {

    RoomManager::RoomManager() : _matchmaking(std::make_shared<MatchmakingService>()) {}

    RoomManager::RoomManager(std::shared_ptr<MatchmakingService> matchmaking) : _matchmaking(matchmaking) {}

    void RoomManager::addPlayerToMatchmaking(int playerId) {
        _matchmaking->addPlayer(playerId);

        // Check if a match was created (4 players queued)
        const auto &matches = _matchmaking->getActiveMatches();
        for (const auto &[matchId, players] : matches) {
            std::string roomId = "match_" + std::to_string(matchId);
            if (_rooms.find(roomId) == _rooms.end()) {
                createRoom(roomId);
                LOG_INFO("✓ Match room created: ", roomId, " with ", players.size(), " players");
            }
        }
    }

    void RoomManager::removePlayerFromMatchmaking(int playerId) {
        _matchmaking->removePlayer(playerId);
    }

    void RoomManager::createRoom(const std::string &id) {
        if (_rooms.find(id) != _rooms.end()) {
            LOG_WARNING("Room ", id, " already exists");
            return;
        }

        std::shared_ptr<Room> room = std::make_shared<Room>(id);
        _rooms[id] = room;

        LOG_INFO("✓ Room created: ", id);
    }

    std::shared_ptr<Room> RoomManager::getRoom(const std::string &id) {
        auto it = _rooms.find(id);
        if (it != _rooms.end()) {
            return it->second;
        }

        LOG_WARNING("Room ", id, " not found");
        return nullptr;
    }

    void RoomManager::removeRoom(const std::string &id) {
        auto it = _rooms.find(id);
        if (it != _rooms.end()) {
            _rooms.erase(it);
            LOG_INFO("✓ Room removed: ", id);
        }
    }

}  // namespace server
