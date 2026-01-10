/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** LuaSystemAdapter - Bridge between ECS LuaSystem and Scripting LuaEngine
*/

#pragma once

#include "LuaEngine.hpp"
#include "common/ECS/Components/LuaScript.hpp"
#include "common/ECS/Systems/ISystem.hpp"
#include "common/ECSWrapper/ECSWorld.hpp"

namespace scripting {
    /**
     * @class LuaSystemAdapter
     * @brief ECS system that executes Lua scripts for entities.
     * 
     * Integrates the Lua scripting engine with the ECS system,
     * executing scripts attached to entities via LuaScript components.
     * Requires LuaScript component.
     */
    class LuaSystemAdapter : public ecs::ISystem {
       public:
        /**
         * @brief Constructor with LuaEngine and ECSWorld.
         * 
         * @param luaEngine Pointer to the Lua engine
         * @param world Pointer to the ECS world wrapper
         */
        LuaSystemAdapter(LuaEngine *luaEngine, ecs::wrapper::ECSWorld *world);

        /**
         * @brief Updates all Lua scripts for entities.
         * 
         * Overrides the base ISystem to properly integrate with LuaEngine.
         * 
         * @param registry Reference to the ECS registry
         * @param deltaTime Time elapsed since last frame
         */
        void update(ecs::Registry &registry, float deltaTime) override;

        /**
         * @brief Get component mask for this system.
         * 
         * @return ComponentMask requiring LuaScript component
         */
        ecs::ComponentMask getComponentMask() const override;

       private:
        LuaEngine *_luaEngine;
        ecs::wrapper::ECSWorld *_world;
    };

}  // namespace scripting
