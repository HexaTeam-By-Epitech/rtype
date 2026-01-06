/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** EntityBindings
*/

#pragma once

#include <sol/sol.hpp>
#include "common/ECS/Registry.hpp"
#include "common/ECSWrapper/ECSWorld.tpp"

namespace scripting::bindings {
    /**
     * Lua tables added:
     *  components
     *
     * Lua functions added:
     *  removeComponent(addr, "Type") -> void
     *  table components {
     *
     *  }
     *
     */
    void bindEntity(sol::state &lua, ecs::wrapper::ECSWorld *world);

}  // namespace scripting::bindings
