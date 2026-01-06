/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** LuaSystem
*/

#pragma once

#include "common/ECS/Components/LuaScript.hpp"
#include "common/ECS/Registry.hpp"
#include "common/ECS/Systems/ISystem.hpp"

namespace ecs {
    /**
     * @class LuaSystem
     * @brief System managing Lua script execution for entities.
     * 
     * Executes Lua scripts associated with entities via the LuaScript component.
     * Requires LuaScript component.
     * 
     * @note The actual Lua execution is implemented in the server-side LuaEngine.
     * This system provides the integration point between ECS and Lua.
     */
    class LuaSystem : public ISystem {
       public:
        /**
         * @brief Default constructor.
         */
        LuaSystem() = default;

        /**
         * @brief Default destructor.
         */
        ~LuaSystem() override = default;

        /**
         * @brief Updates all Lua scripts attached to entities.
         * 
         * Iterates through all entities with LuaScript components
         * and executes their associated scripts via the Lua engine.
         * 
         * @param registry Reference to the ECS registry
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        void update(Registry &registry, float deltaTime) override;

        /**
         * @brief Gets the component mask for this system.
         * 
         * @return ComponentMask requiring LuaScript component
         */
        ComponentMask getComponentMask() const override;
    };
}  // namespace ecs
