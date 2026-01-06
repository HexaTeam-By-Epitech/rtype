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
        static constexpr uint32_t getDefaultPlayerHealth() { return 100; }
        static constexpr uint32_t getDefaultPlayerSpeed() { return 200; }
        static constexpr uint32_t getPlayerSpawnX() { return 50; }
        static constexpr uint32_t getPlayerSpawnY() { return 300; }
        static constexpr float getDefaultPlayerFireRate() { return 10.0f; }
        static constexpr uint32_t getDefaultPlayerDamage() { return 25; }
    };

}  // namespace server
