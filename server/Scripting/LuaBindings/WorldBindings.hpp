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
 * Lua functions added:
 *  createEntity() -> Address
 *  destroyEntity(Entity e) -> void
 *  entityExists(Entity e) -> bool
 *  log(string message) -> void
 */
    void bindWorld(sol::state &lua, ecs::wrapper::ECSWorld *world);
}  // namespace scripting::bindings
