/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Lobby.cpp - Main lobby implementation
*/

#include "server/Rooms/Lobby/Lobby.hpp"
#include "common/Logger/Logger.hpp"
#include "server/Rooms/RoomManager/RoomManager.hpp"

namespace server {

    // Initialize static member
    std::atomic<uint64_t> Lobby::_nextRoomId{1};

    Lobby::Lobby(std::shared_ptr<RoomManager> roomManager) : _roomManager(roomManager) {
        LOG_INFO("Lobby created");
    }

    bool Lobby::addPlayer(uint32_t playerId, const std::string &playerName) {
        std::lock_guard<std::mutex> lock(_mutex);

        if (_players.find(playerId) != _players.end()) {
            LOG_WARNING("Player ", playerId, " already in lobby");
            return false;
        }

        LobbyPlayer player;
        player.playerId = playerId;
        player.playerName = playerName;
        player.isReady = false;
        player.inMatchmaking = false;

        _players[playerId] = player;
        LOG_INFO("✓ Player '", playerName, "' (", playerId, ") joined lobby (", _players.size(), " players)");
        return true;
    }

    bool Lobby::removePlayer(uint32_t playerId) {
        std::lock_guard<std::mutex> lock(_mutex);

        auto it = _players.find(playerId);
        if (it == _players.end()) {
            return false;
        }

        if (it->second.inMatchmaking && _roomManager) {
            _roomManager->removePlayerFromMatchmaking(playerId);
        }

        _players.erase(it);
        LOG_INFO("✓ Player ", playerId, " left lobby (", _players.size(), " players remaining)");
        return true;
    }

    bool Lobby::updatePlayerName(uint32_t playerId, const std::string &newName) {
        std::lock_guard<std::mutex> lock(_mutex);

        auto it = _players.find(playerId);
        if (it == _players.end()) {
            LOG_WARNING("Cannot update name - player ", playerId, " not in lobby");
            return false;
        }

        std::string oldName = it->second.playerName;
        it->second.playerName = newName;
        LOG_INFO("✓ Player ", playerId, " name updated: '", oldName, "' → '", newName, "'");
        return true;
    }

    const LobbyPlayer *Lobby::getPlayer(uint32_t playerId) const {
        std::lock_guard<std::mutex> lock(_mutex);

        auto it = _players.find(playerId);
        if (it != _players.end()) {
            return &it->second;
        }

        return nullptr;
    }

    std::vector<LobbyPlayer> Lobby::getAllPlayers() const {
        std::lock_guard<std::mutex> lock(_mutex);

        std::vector<LobbyPlayer> players;
        players.reserve(_players.size());

        for (const auto &[id, player] : _players) {
            players.push_back(player);
        }

        return players;
    }

    size_t Lobby::getPlayerCount() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _players.size();
    }

    bool Lobby::startMatchmaking(uint32_t playerId) {
        std::lock_guard<std::mutex> lock(_mutex);

        auto it = _players.find(playerId);
        if (it == _players.end()) {
            LOG_WARNING("Cannot start matchmaking - player ", playerId, " not in lobby");
            return false;
        }

        if (it->second.inMatchmaking) {
            LOG_WARNING("Player ", playerId, " already in matchmaking");
            return false;
        }

        if (!_roomManager) {
            LOG_ERROR("Cannot start matchmaking - room manager not initialized");
            return false;
        }

        it->second.inMatchmaking = true;
        _roomManager->addPlayerToMatchmaking(playerId);

        LOG_INFO("✓ Player ", playerId, " started matchmaking");
        return true;
    }

    bool Lobby::cancelMatchmaking(uint32_t playerId) {
        std::lock_guard<std::mutex> lock(_mutex);

        auto it = _players.find(playerId);
        if (it == _players.end()) {
            return false;
        }

        if (!it->second.inMatchmaking) {
            return false;
        }

        if (_roomManager) {
            _roomManager->removePlayerFromMatchmaking(playerId);
        }

        it->second.inMatchmaking = false;
        LOG_INFO("✓ Player ", playerId, " cancelled matchmaking");
        return true;
    }

    bool Lobby::joinRoom(uint32_t playerId, const std::string &roomId) {
        std::lock_guard<std::mutex> lock(_mutex);

        auto playerIt = _players.find(playerId);
        if (playerIt == _players.end()) {
            LOG_WARNING("Cannot join room - player ", playerId, " not in lobby");
            return false;
        }

        if (!_roomManager) {
            LOG_ERROR("Cannot join room - room manager not initialized");
            return false;
        }

        auto room = _roomManager->getRoom(roomId);
        if (!room) {
            LOG_WARNING("Cannot join room - room '", roomId, "' not found");
            return false;
        }

        if (room->join(playerId)) {
            LOG_INFO("✓ Player ", playerId, " joined room '", roomId, "'");
            return true;
        }

        return false;
    }

    std::string Lobby::createCustomRoom(uint32_t hostPlayerId, const std::string &roomName, size_t maxPlayers,
                                        bool isPrivate) {
        std::lock_guard<std::mutex> lock(_mutex);

        auto playerIt = _players.find(hostPlayerId);
        if (playerIt == _players.end()) {
            LOG_WARNING("Cannot create room - player ", hostPlayerId, " not in lobby");
            return "";
        }

        if (!_roomManager) {
            LOG_ERROR("Cannot create room - room manager not initialized");
            return "";
        }

        // Generate unique room ID using atomic counter to prevent collisions
        uint64_t roomNumber = _nextRoomId.fetch_add(1, std::memory_order_relaxed);
        std::string roomId = "custom_" + std::to_string(hostPlayerId) + "_" + std::to_string(roomNumber);

        auto room = _roomManager->createRoom(roomId, roomName, maxPlayers, isPrivate);
        if (!room) {
            LOG_ERROR("Failed to create custom room");
            return "";
        }

        if (room->join(hostPlayerId)) {
            room->setHost(hostPlayerId);
            LOG_INFO("✓ Player ", hostPlayerId, " created custom room '", roomName, "' (", roomId, ")");
            return roomId;
        }

        _roomManager->removeRoom(roomId);
        return "";
    }

    void Lobby::setPlayerReady(uint32_t playerId, bool ready) {
        std::lock_guard<std::mutex> lock(_mutex);

        auto it = _players.find(playerId);
        if (it != _players.end()) {
            it->second.isReady = ready;
            LOG_INFO("Player ", playerId, " ready status: ", (ready ? "ready" : "not ready"));
        }
    }

}  // namespace server
