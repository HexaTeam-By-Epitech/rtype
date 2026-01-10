/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/01/2026
** File description:
** GameRules.hpp - Game configuration and rules
*/

#pragma once

#include <cstdint>

namespace server {

    /**
     * @class GameRules
     * @brief Centralized game rules and configuration
     *
     * Contains gameplay constants used by GameLogic.
     * Modify these values to change game balance without recompiling.
     */
    class GameRules {
       public:
        GameRules() = default;
        ~GameRules() = default;

        // Player Configuration
        constexpr uint32_t getDefaultPlayerHealth() const { return 100; }
        constexpr uint32_t getDefaultPlayerSpeed() const { return 200; }
        constexpr uint32_t getPlayerSpawnX() const { return 50; }
        constexpr uint32_t getPlayerSpawnY() const { return 300; }
        constexpr float getDefaultPlayerFireRate() const { return 10.0f; }
        constexpr uint32_t getDefaultPlayerDamage() const { return 25; }
    };

}  // namespace server
