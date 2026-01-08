/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** Room.hpp
*/

#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "server/Core/EventBus/EventBus.hpp"
#include "server/Core/ServerLoop/ServerLoop.hpp"
#include "server/Game/Logic/IGameLogic.hpp"
#include "server/Rooms/IRoom.hpp"

namespace server {

    /**
     * @class Room
     * @brief Concrete implementation of IRoom with game instance management
     */
    class Room : public IRoom {
       public:
        /**
         * @brief Construct a room
         * @param id Unique room identifier
         * @param name Display name for the room
         * @param maxPlayers Maximum number of players (default: 4)
         * @param isPrivate Whether the room is private (default: false)
         */
        explicit Room(const std::string &id, const std::string &name = "", size_t maxPlayers = 4,
                      bool isPrivate = false);
        ~Room() override = default;

        bool join(uint32_t playerId) override;
        bool leave(uint32_t playerId) override;
        std::string getId() const override { return _id; };
        std::string getName() const override { return _name; };
        RoomState getState() const override { return _state; };
        void setState(RoomState state) override;
        size_t getPlayerCount() const override;
        size_t getMaxPlayers() const override { return _maxPlayers; };
        bool isFull() const override { return _players.size() >= _maxPlayers; };
        std::vector<uint32_t> getPlayers() const override;
        bool hasPlayer(uint32_t playerId) const override;
        RoomInfo getInfo() const override;

        /**
         * @brief Get the game logic instance
         * @return Shared pointer to game logic (may be nullptr)
         */
        std::shared_ptr<IGameLogic> getGameLogic() const { return _gameLogic; };

        /**
         * @brief Get the server loop instance for this room
         * @return Pointer to ServerLoop (may be nullptr)
         */
        ServerLoop *getServerLoop() const { return _gameLoop.get(); };

        /**
         * @brief Start the game for this room
         * @return true if game started successfully
         */
        bool startGame();

        /**
         * @brief Request to start the game (initiates countdown)
         * 
         * This is called when the host requests to start the game.
         * It transitions the room to STARTING state and begins the countdown.
         */
        void requestStartGame();

        /**
         * @brief Update the room state (called by server loop)
         * @param deltaTime Time since last update
         */
        void update(float deltaTime);

        /**
         * @brief Set the host player ID
         * @param playerId Player ID to set as host
         */
        void setHost(uint32_t playerId);

        /**
         * @brief Get the host player ID
         * @return Host player ID (0 if no host)
         */
        uint32_t getHost() const { return _hostPlayerId; };

        /**
         * @brief Check if GameStart has been sent
         * @return true if GameStart has been sent to players
         */
        bool isGameStartSent() const { return _gameStartSent; }

        /**
         * @brief Mark GameStart as sent
         */
        void setGameStartSent(bool sent) { _gameStartSent = sent; }

        /**
         * @brief Atomically check and set GameStart sent flag (thread-safe)
         * @return true if flag was false and is now set to true, false if already sent
         */
        bool tryMarkGameStartSent();

        /**
         * @brief Check if room is private
         * @return true if room is private
         */
        bool isPrivate() const { return _isPrivate; }

       private:
        std::string _id;
        std::string _name;
        RoomState _state;
        size_t _maxPlayers;
        bool _isPrivate;
        uint32_t _hostPlayerId;
        std::vector<uint32_t> _players;
        std::shared_ptr<IGameLogic> _gameLogic;
        std::unique_ptr<ServerLoop> _gameLoop;  // Dedicated game loop for this room
        std::shared_ptr<EventBus> _eventBus;    // Event bus for this room
        mutable std::mutex _mutex;              // Thread safety for player management
        bool _gameStartSent;                    // Whether GameStart has been sent to players
    };

}  // namespace server
