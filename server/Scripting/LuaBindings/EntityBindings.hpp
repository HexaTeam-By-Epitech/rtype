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
     * @brief Bind Entity wrapper class and operations to Lua.
     * 
     * Provides the Entity usertype with component accessors (get, has).
     * 
     * @param lua Reference to the Lua state
     * @param world Pointer to the ECS world (can be nullptr, not directly used)
     * 
     * Entity methods bound:
     *  - getAddress() -> Address
     *  - isValid() -> bool
     *  - getTransform() -> Transform&
     *  - getVelocity() -> Velocity&
     *  - getHealth() -> Health&
     *  - hasTransform() -> bool
     *  - hasVelocity() -> bool
     *  - hasHealth() -> bool
     */
    void bindEntity(sol::state &lua, ecs::wrapper::ECSWorld *world);

}  // namespace scripting::bindings
