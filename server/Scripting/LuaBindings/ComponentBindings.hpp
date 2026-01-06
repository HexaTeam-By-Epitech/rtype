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
     */
    void bindComponents(sol::state &lua, ecs::wrapper::ECSWorld *world);
}  // namespace scripting::bindings
