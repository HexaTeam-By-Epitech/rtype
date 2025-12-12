/*
** EPITECH PROJECT, 2025
** Created by hugo on 12/12/2025
** File description:
** PlayerJoinedEvent.hpp
*/

#pragma once

#include <string>
#include "GameEvent.hpp"

namespace server {

    /**
     * @class PlayerJoinedEvent
     * @brief Event triggered when a player joins the game
     */
    class PlayerJoinedEvent : public GameEvent {
       public:
        /**
         * @brief Construct a PlayerJoinedEvent
         * @param playerId The ID of the player who joined
         * @param playerName The name of the player
         */
        PlayerJoinedEvent(uint32_t playerId, const std::string &playerName)
            : GameEvent(Type::PLAYER_JOINED), _playerId(playerId), _playerName(playerName) {}

        ~PlayerJoinedEvent() override = default;

        /**
         * @brief Get the player ID
         */
        uint32_t getPlayerId() const { return _playerId; }

        /**
         * @brief Get the player name
         */
        const std::string &getPlayerName() const { return _playerName; }

       private:
        uint32_t _playerId;
        std::string _playerName;
    };

}  // namespace server
