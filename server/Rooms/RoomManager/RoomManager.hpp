/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** RoomManager.hpp
*/

#pragma once

#include <memory>
#include <unordered_map>
#include "server/Rooms/Matchmaking/MatchmakingService.hpp"
#include "server/Rooms/RoomManager/IRoomManager.hpp"

namespace server {

    /**
     * @class RoomManager
     * @brief Concrete implementation of IRoomManager with matchmaking support
     */
    class RoomManager : public IRoomManager {
       public:
        RoomManager();
        explicit RoomManager(std::shared_ptr<MatchmakingService> matchmaking);
        ~RoomManager() override = default;

        void createRoom(const std::string &id) override;
        std::shared_ptr<Room> getRoom(const std::string &id) override;
        void removeRoom(const std::string &id) override;

        /**
         * @brief Add player to matchmaking queue
         * @param playerId Player ID to add
         */
        void addPlayerToMatchmaking(int playerId);

        /**
         * @brief Remove player from matchmaking queue
         * @param playerId Player ID to remove
         */
        void removePlayerFromMatchmaking(int playerId);

        /**
         * @brief Get the matchmaking service
         * @return Shared pointer to MatchmakingService
         */
        std::shared_ptr<MatchmakingService> getMatchmaking() { return _matchmaking; }

       private:
        std::unordered_map<std::string, std::shared_ptr<Room>> _rooms;
        std::shared_ptr<MatchmakingService> _matchmaking;
    };

}  // namespace server
