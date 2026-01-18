/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** RoomManager.cpp
*/

#include "server/Rooms/RoomManager/RoomManager.hpp"
#include "common/Logger/Logger.hpp"

namespace server {

    RoomManager::RoomManager()
        : _matchmaking(std::make_shared<MatchmakingService>(2, 4, nullptr)), _eventBus(nullptr) {
        _matchmaking->setMatchCreatedCallback([this](std::shared_ptr<Room> room) { _onMatchCreated(room); });
        LOG_INFO("RoomManager created with matchmaking service");
    }

    RoomManager::RoomManager(std::shared_ptr<MatchmakingService> matchmaking,
                             std::shared_ptr<EventBus> eventBus)
        : _matchmaking(matchmaking), _eventBus(eventBus) {
        if (_matchmaking) {
            _matchmaking->setMatchCreatedCallback(
                [this](std::shared_ptr<Room> room) { _onMatchCreated(room); });
        }
        LOG_INFO("RoomManager created with provided matchmaking service");
    }

    void RoomManager::addPlayerToMatchmaking(uint32_t playerId) {
        if (!_matchmaking) {
            LOG_ERROR("Cannot add player to matchmaking - service not initialized");
            return;
        }

        _matchmaking->addPlayer(playerId);
        LOG_INFO("Player ", playerId, " added to matchmaking (queue size: ", _matchmaking->getQueueSize(),
                 ")");
    }

    void RoomManager::removePlayerFromMatchmaking(uint32_t playerId) {
        if (!_matchmaking) {
            return;
        }

        _matchmaking->removePlayer(playerId);
        LOG_INFO("Player ", playerId, " removed from matchmaking");
    }

    std::shared_ptr<Room> RoomManager::createRoom(const std::string &id, const std::string &name,
                                                  size_t maxPlayers, bool isPrivate,
                                                  float gameSpeedMultiplier) {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        if (_rooms.find(id) != _rooms.end()) {
            LOG_WARNING("Room ", id, " already exists");
            return _rooms[id];
        }

        try {
            auto room =
                std::make_shared<Room>(id, name, maxPlayers, isPrivate, gameSpeedMultiplier, _eventBus);
            _rooms[id] = room;

            LOG_INFO("✓ Room created: '", room->getName(), "' (", id, ")");
            return room;
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create room '", name, "' (", id, "): ", e.what());
            return nullptr;
        }
    }

    std::shared_ptr<Room> RoomManager::getRoom(const std::string &id) {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        auto it = _rooms.find(id);
        if (it != _rooms.end()) {
            return it->second;
        }

        return nullptr;
    }

    bool RoomManager::removeRoom(const std::string &id) {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        auto it = _rooms.find(id);
        if (it != _rooms.end()) {
            LOG_INFO("✓ Room removed: ", id);
            _rooms.erase(it);
            return true;
        }

        return false;
    }

    std::vector<std::shared_ptr<Room>> RoomManager::getAllRooms() {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        std::vector<std::shared_ptr<Room>> rooms;
        rooms.reserve(_rooms.size());

        for (const auto &[id, room] : _rooms) {
            rooms.push_back(room);
        }

        return rooms;
    }

    std::vector<std::shared_ptr<Room>> RoomManager::getPublicRooms() {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        std::vector<std::shared_ptr<Room>> publicRooms;

        for (const auto &[id, room] : _rooms) {
            auto info = room->getInfo();
            if (!info.isPrivate && info.state != RoomState::FINISHED) {
                publicRooms.push_back(room);
            }
        }

        return publicRooms;
    }

    size_t RoomManager::getRoomCount() const {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        return _rooms.size();
    }

    bool RoomManager::update(float deltaTime) {
        if (_matchmaking) {
            _matchmaking->tick();
        }

        std::lock_guard<std::recursive_mutex> lock(_mutex);
        for (auto &[id, room] : _rooms) {
            room->update(deltaTime);
        }

        std::vector<std::string> roomsToRemove;
        for (const auto &[id, room] : _rooms) {
            if (room->getState() == RoomState::FINISHED && room->getPlayerCount() == 0) {
                roomsToRemove.push_back(id);
            }
        }

        for (const auto &id : roomsToRemove) {
            _rooms.erase(id);
            LOG_INFO("Cleaned up finished room: ", id);
        }

        if (!roomsToRemove.empty()) {
            LOG_INFO("Cleaned up ", roomsToRemove.size(), " finished room(s)");
            return true;
        }

        return false;
    }

    std::shared_ptr<Room> RoomManager::getRoomByPlayer(uint32_t playerId) {
        std::lock_guard lock(_mutex);

        for (const auto &[id, room] : _rooms) {
            if (room->hasPlayer(playerId) || room->hasSpectator(playerId)) {
                return room;
            }
        }

        return nullptr;
    }

    void RoomManager::cleanupFinishedRooms() {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        std::vector<std::string> roomsToRemove;

        for (const auto &[id, room] : _rooms) {
            if (room->getState() == RoomState::FINISHED && room->getPlayerCount() == 0) {
                roomsToRemove.push_back(id);
            }
        }

        for (const auto &id : roomsToRemove) {
            _rooms.erase(id);
            LOG_INFO("Cleaned up finished room: ", id);
        }

        if (!roomsToRemove.empty()) {
            LOG_INFO("Cleaned up ", roomsToRemove.size(), " finished room(s)");
        }
    }

    void RoomManager::_onMatchCreated(std::shared_ptr<Room> room) {
        // Store callback and room outside the lock to avoid deadlock
        // (callback may call getPublicRooms() which acquires the same mutex)
        std::function<void(std::shared_ptr<Room>)> callback;

        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);

            std::string roomId = room->getId();

            if (_rooms.find(roomId) != _rooms.end()) {
                LOG_WARNING("Match room ", roomId, " already exists");
                return;
            }

            _rooms[roomId] = room;
            LOG_INFO("✓ Match room registered: ", roomId, " (", room->getPlayerCount(), " players)");
            room->setState(RoomState::STARTING);

            callback = _roomCreatedCallback;
        }

        // Invoke callback outside the lock to prevent deadlock
        if (callback) {
            callback(room);
        }
    }

}  // namespace server
