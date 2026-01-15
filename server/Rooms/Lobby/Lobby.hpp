/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Lobby.hpp - Main lobby for player connections
*/

#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace server {

    class RoomManager;

    /**
     * @struct LobbyPlayer
     * @brief Information about a player in the lobby
     */
    struct LobbyPlayer {
        uint32_t playerId;
        std::string playerName;
        bool isReady;
        bool inMatchmaking;
    };

    /**
     * @class Lobby
     * @brief Main lobby where players connect before joining rooms
     * 
     * Features:
     * - Player registration/deregistration
     * - Room discovery (list available rooms)
     * - Matchmaking integration
     * - Ready system for manual rooms
     */
    class Lobby {
       public:
        /**
         * @brief Construct a lobby
         * @param roomManager Shared pointer to room manager
         */
        explicit Lobby(std::shared_ptr<RoomManager> roomManager);
        ~Lobby() = default;

        /**
         * @brief Add a player to the lobby
         * @param playerId Player ID
         * @param playerName Player display name
         * @return true if player was added
         */
        bool addPlayer(uint32_t playerId, const std::string &playerName);

        /**
         * @brief Remove a player from the lobby
         * @param playerId Player ID
         * @return true if player was removed
         */
        bool removePlayer(uint32_t playerId);

        /**
         * @brief Update a player's display name
         * @param playerId Player ID
         * @param newName New display name
         * @return true if player name was updated
         */
        bool updatePlayerName(uint32_t playerId, const std::string &newName);

        /**
         * @brief Get player info
         * @param playerId Player ID
         * @return Pointer to LobbyPlayer (nullptr if not found)
         */
        const LobbyPlayer *getPlayer(uint32_t playerId) const;

        /**
         * @brief Get all players in lobby
         * @return Vector of lobby players
         */
        std::vector<LobbyPlayer> getAllPlayers() const;

        /**
         * @brief Get number of players in lobby
         * @return Player count
         */
        size_t getPlayerCount() const;

        /**
         * @brief Start matchmaking for a player
         * @param playerId Player ID
         * @return true if matchmaking started
         */
        bool startMatchmaking(uint32_t playerId);

        /**
         * @brief Cancel matchmaking for a player
         * @param playerId Player ID
         * @return true if matchmaking cancelled
         */
        bool cancelMatchmaking(uint32_t playerId);

        /**
         * @brief Join a specific room by ID
         * @param playerId Player ID
         * @param roomId Room identifier
         * @return true if player joined room
         */
        bool joinRoom(uint32_t playerId, const std::string &roomId);

        /**
         * @brief Create a custom room
         * @param hostPlayerId Host player ID
         * @param roomName Room display name
         * @param maxPlayers Maximum players
         * @param isPrivate Whether room is private
         * @return Room ID if created, empty string otherwise
         */
        std::string createCustomRoom(uint32_t hostPlayerId, const std::string &roomName, size_t maxPlayers,
                                     bool isPrivate);

        /**
         * @brief Set player ready status (for custom rooms)
         * @param playerId Player ID
         * @param ready Ready status
         */
        void setPlayerReady(uint32_t playerId, bool ready);

        /**
         * @brief Get room manager
         * @return Shared pointer to room manager
         */
        std::shared_ptr<RoomManager> getRoomManager() const { return _roomManager; }

       private:
        std::unordered_map<uint32_t, LobbyPlayer> _players;
        std::shared_ptr<RoomManager> _roomManager;
        mutable std::mutex _mutex;

        // Static counter for unique room ID generation
        static std::atomic<uint64_t> _nextRoomId;
    };

}  // namespace server
