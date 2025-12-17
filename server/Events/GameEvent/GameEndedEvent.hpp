/*
** EPITECH PROJECT, 2025
** Created by hugo on 12/12/2025
** File description:
** GameEndedEvent.hpp
*/

#pragma once

#include "server/Events/GameEvent/GameEvent.hpp"

namespace server {

    /**
     * @class GameEndedEvent
     * @brief Event triggered when the game ends
     */
    class GameEndedEvent : public GameEvent {
       public:
        /**
         * @brief Construct a GameEndedEvent
         * @param reason Reason for game ending (e.g., "Victory", "Defeat", "Timeout")
         */
        explicit GameEndedEvent(const std::string &reason = "")
            : GameEvent(Type::GAME_ENDED), _reason(reason) {}

        ~GameEndedEvent() override = default;

        /**
         * @brief Get the reason for game ending
         */
        const std::string &getReason() const { return _reason; }

       private:
        std::string _reason;
    };

}  // namespace server
