/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** WorldBindings
*/

#pragma once

#include <sol/state.hpp>
#include "common/ECSWrapper/ECSWorld.hpp"

namespace scripting::bindings {
    /**
     * @brief Bind ECS world operations to Lua.
     * 
     * Provides global Lua functions for entity management and logging.
     * 
     * @param lua Reference to the Lua state
     * @param world Pointer to the ECS world (must not be nullptr)
     * 
     * @warning The world pointer is captured by lambda closures. Ensure the world
     *          lifetime exceeds the Lua state lifetime.
     * 
     * Lua functions added:
     *  - createEntity() -> Entity
     *  - destroyEntity(Entity e) -> void
     *  - entityExists(Address addr) -> bool
     *  - log(string message) -> void
     */
    void bindWorld(sol::state &lua, ecs::wrapper::ECSWorld *world);
}  // namespace scripting::bindings
