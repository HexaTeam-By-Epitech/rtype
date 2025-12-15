/*
** EPITECH PROJECT, 2025
** Created by hugo on 12/12/2025
** File description:
** PlayerLeftEvent.hpp
*/

#pragma once

#include "server/Events/GameEvent/GameEvent.hpp"

namespace server {

    /**
     * @class PlayerLeftEvent
     * @brief Event triggered when a player leaves the game
     */
    class PlayerLeftEvent : public GameEvent {
       public:
        /**
         * @brief Construct a PlayerLeftEvent
         * @param playerId The ID of the player who left
         */
        explicit PlayerLeftEvent(uint32_t playerId) : GameEvent(Type::PLAYER_LEFT), _playerId(playerId) {}

        ~PlayerLeftEvent() override = default;

        /**
         * @brief Get the player ID
         */
        uint32_t getPlayerId() const { return _playerId; }

       private:
        uint32_t _playerId;
    };

}  // namespace server
