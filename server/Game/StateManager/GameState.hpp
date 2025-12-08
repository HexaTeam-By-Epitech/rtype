/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** GameState.hpp
*/

#pragma once

namespace server {

    /**
     * @class GameState
     * @brief Represents a single game state
     */
    class GameState {
       public:
        GameState() = default;
        virtual ~GameState() = default;

        // Prototypes
        virtual void enter() = 0;
        virtual void exit() = 0;
        virtual void update(float dt) = 0;
    };

}  // namespace server
