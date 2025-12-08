/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** GameEvent.hpp
*/

#pragma once

#include <chrono>
#include <string>
#include "IEvent.hpp"

namespace server {

    /**
     * @class GameEvent
     * @brief Base class for all game-related events
     */
    class GameEvent : public IEvent {
       public:
        enum class Type { UNKNOWN, PLAYER_JOINED, PLAYER_LEFT, SCORE_UPDATED, GAME_STARTED, GAME_ENDED };

        explicit GameEvent(Type type);
        virtual ~GameEvent() override = default;

        /**
         * @brief Get the type of the event
         */
        Type getType() const;

       private:
        Type _type{Type::UNKNOWN};
    };

}  // namespace server
