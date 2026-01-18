/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** RoomManager.hpp
*/

#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>
#include "server/Rooms/Matchmaking/MatchmakingService.hpp"
#include "server/Rooms/RoomManager/IRoomManager.hpp"

namespace server {

    // Callback type for when a match is created and players should be notified
    using RoomCreatedCallback = std::function<void(std::shared_ptr<Room>)>;

    /**
     * @class RoomManager
     * @brief Manages all game rooms and matchmaking
     * 
     * Features:
     * - Create/destroy rooms
     * - Automatic matchmaking integration
     * - Room discovery (list public rooms)
     * - Thread-safe operations
     */
    class RoomManager : public IRoomManager {
       public:
        RoomManager();
        explicit RoomManager(std::shared_ptr<MatchmakingService> matchmaking);
        ~RoomManager() override = default;

        std::shared_ptr<Room> createRoom(const std::string &id, const std::string &name = "",
                                         size_t maxPlayers = 4, bool isPrivate = false) override;
        std::shared_ptr<Room> getRoom(const std::string &id) override;
        bool removeRoom(const std::string &id) override;
        std::vector<std::shared_ptr<Room>> getAllRooms() override;
        std::vector<std::shared_ptr<Room>> getPublicRooms() override;
        size_t getRoomCount() const override;
        bool update(float deltaTime) override;

        /**
         * @brief Add player to matchmaking queue
         * @param playerId Player ID to add
         */
        void addPlayerToMatchmaking(uint32_t playerId);

        /**
         * @brief Remove player from matchmaking queue
         * @param playerId Player ID to remove
         */
        void removePlayerFromMatchmaking(uint32_t playerId);

        /**
         * @brief Get the matchmaking service
         * @return Shared pointer to MatchmakingService
         */
        std::shared_ptr<MatchmakingService> getMatchmaking() { return _matchmaking; }

        /**
         * @brief Set callback for when a room is created by matchmaking
         * @param callback Function to call when a room is created with matched players
         */
        void setRoomCreatedCallback(RoomCreatedCallback callback) { _roomCreatedCallback = callback; }

        /**
         * @brief Get room by player ID (find which room a player is in)
         * @param playerId Player ID to search for
         * @return Shared pointer to room (nullptr if not found)
         */
        std::shared_ptr<Room> getRoomByPlayer(uint32_t playerId);

        /**
         * @brief Clean up finished rooms
         * Removes rooms that are in FINISHED state and have no players
         */
        void cleanupFinishedRooms();

       private:
        /**
         * @brief Handle match created by matchmaking service
         * @param room Newly created room with matched players
         */
        void _onMatchCreated(std::shared_ptr<Room> room);

        std::unordered_map<std::string, std::shared_ptr<Room>> _rooms;
        std::shared_ptr<MatchmakingService> _matchmaking;
        RoomCreatedCallback _roomCreatedCallback;
        mutable std::mutex _mutex;
    };

}  // namespace server
