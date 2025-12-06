/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** GameEvent.cpp
*/

#pragma once

#include <string>
#include <chrono>
#include "IEvent.hpp"

namespace server {

/**
 * @class GameEvent
 * @brief Base class for all game-related events
 */
class GameEvent : public IEvent {
public:
    enum class Type {
        UNKNOWN,
        PLAYER_JOINED,
        PLAYER_LEFT,
        SCORE_UPDATED,
        GAME_STARTED,
        GAME_ENDED
    };

    GameEvent(Type type);
    virtual ~GameEvent() override = default;

    /**
     * @brief Get the type of the event
     */
    Type getType() const;

    /**
     * @brief Get the timestamp when the event was created
     */
    std::chrono::system_clock::time_point getTimestamp() const;

private:
    Type _type{Type::UNKNOWN};
    std::chrono::system_clock::time_point _timestamp;
};

} // namespace server

