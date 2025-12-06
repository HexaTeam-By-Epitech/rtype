/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** GameRules.hpp
*/

#pragma once

namespace server {

/**
 * @class GameRules
 * @brief Contains the rules and constraints of the game
 *
 * Defines constants and validation methods that govern game logic.
 */
class GameRules {
public:
    GameRules() = default;
    ~GameRules() = default;

    // Prototypes
    int getMaxPlayers() const;
    int getMaxScore() const;
    bool isMoveValid(int x, int y) const;
};

} // namespace server

