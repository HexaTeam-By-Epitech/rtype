/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** EntityBindings
*/

#pragma once

#include <sol/sol.hpp>
#include "ComponentBindingHelper.hpp"
#include "common/ECS/Registry.hpp"
#include "common/ECSWrapper/ECSWorld.hpp"

namespace scripting::bindings {
    /**
     * @brief Bind Entity wrapper class and operations to Lua.
     * 
     * Uses ComponentBindingHelper to automatically generate
     * get/has methods for all registered components.
     * 
     * @param lua Reference to the Lua state
     * @param world Pointer to the ECS world
     * @param helper The helper with all registered components
     * 
     * Entity methods automatically bound:
     *  - getAddress() -> Address
     *  - isValid() -> bool
     *  - getTransform(), getVelocity(), getHealth() -> auto-generated
     *  - hasTransform(), hasVelocity(), hasHealth() -> auto-generated
     * 
     * Global functions:
     *  - removeComponent(addr, "ComponentName") -> auto-generated
     */
    void bindEntity(sol::state &lua, ecs::wrapper::ECSWorld *world, ComponentBindingHelper &helper);

}  // namespace scripting::bindings
