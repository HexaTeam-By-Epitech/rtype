/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** ComponentBindings
*/

#pragma once
#include <sol/state.hpp>

#include "ComponentBindingHelper.hpp"
#include "common/ECSWrapper/ECSWorld.hpp"

namespace scripting::bindings {
    /**
     * @brief Bind ECS component types to Lua.
     * 
     * Binds each component to Lua and registers it with the helper.
     * To add a new component:
     * 1. Bind it to Lua with lua.new_usertype<T>()
     * 2. Register it with helper.registerComponent<T>("Name")
     * 
     * @param lua Reference to the Lua state
     * @param world Pointer to the ECS world
     * @return ComponentBindingHelper& The configured helper
     * 
     * @note Currently bound components: Transform, Velocity, Health
     */
    ComponentBindingHelper &bindComponents(sol::state &lua, ecs::wrapper::ECSWorld *world);
}  // namespace scripting::bindings
