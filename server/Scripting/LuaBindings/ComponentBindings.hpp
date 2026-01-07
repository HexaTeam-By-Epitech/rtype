/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** ComponentBindings
*/

#pragma once
#include <sol/state.hpp>

#include "common/ECSWrapper/ECSWorld.hpp"

namespace scripting::bindings {
    /**
     * @brief Bind ECS component types to Lua.
     * 
     * Binds Transform, Velocity, and Health components with their respective
     * properties and methods to the Lua state.
     * 
     * @param lua Reference to the Lua state
     * @param world Pointer to the ECS world (can be nullptr, not used in component bindings)
     * 
     * @note Components bound: Transform, Velocity, Health
     */
    void bindComponents(sol::state &lua, ecs::wrapper::ECSWorld *world);
}  // namespace scripting::bindings
