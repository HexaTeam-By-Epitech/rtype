/*
** EPITECH PROJECT, 2025
** Created by hugo on 12/12/2025
** File description:
** GameStartedEvent.hpp
*/

#pragma once

#include <string>
#include "server/Events/GameEvent/GameEvent.hpp"

namespace server {

    /**
     * @class GameStartedEvent
     * @brief Event triggered when the game starts in a specific room
     */
    class GameStartedEvent : public GameEvent {
       public:
        explicit GameStartedEvent(const std::string &roomId = "")
            : GameEvent(Type::GAME_STARTED), _roomId(roomId) {}
        ~GameStartedEvent() override = default;

        /**
         * @brief Get the room ID where the game started
         * @return Room ID string
         */
        const std::string &getRoomId() const { return _roomId; }

       private:
        std::string _roomId;
    };

}  // namespace server
