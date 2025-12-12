/*
** EPITECH PROJECT, 2025
** Created by hugo on 12/12/2025
** File description:
** GameStartedEvent.hpp
*/

#pragma once

#include "GameEvent.hpp"

namespace server {

    /**
     * @class GameStartedEvent
     * @brief Event triggered when the game starts
     */
    class GameStartedEvent : public GameEvent {
       public:
        GameStartedEvent() : GameEvent(Type::GAME_STARTED) {}
        ~GameStartedEvent() override = default;
    };

}  // namespace server
