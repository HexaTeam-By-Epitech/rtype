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
    ~GameState() = default;

    // Prototypes
    void enter();
    void exit();
    void update(float dt);
};

} // namespace server

