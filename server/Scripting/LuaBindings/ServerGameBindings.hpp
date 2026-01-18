/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** ServerGameBindings - Server-specific game logic bindings
*/

#pragma once

#include <sol/state.hpp>
#include "common/ECSWrapper/ECSWorld.hpp"

namespace scripting {
    class LuaEngine;
}

namespace scripting::bindings {
    /**
     * @brief Bind server-specific game logic functions to Lua.
     * 
     * Provides high-level game functions for:
     * - Spawning entities (enemies, projectiles, powerups)
     * - Game state queries
     * - Utility functions (math, random, etc.)
     * - Event callbacks (onGameStart)
     * 
     * @param lua Reference to the Lua state
     * @param world Pointer to the ECS world
     * @param engine Pointer to the LuaEngine (for callback registration)
     * 
     * @note Functions exposed:
     * - spawnEnemy(x, y, enemyType) -> Entity
     * - spawnProjectile(x, y, dirX, dirY, speed, damage) -> Entity
     * - random(min, max) -> float
     * - getTime() -> float
     * - onGameStart(callback) -> void
     */
    void bindServerGame(sol::state &lua, ecs::wrapper::ECSWorld *world, LuaEngine *engine);

}  // namespace scripting::bindings
