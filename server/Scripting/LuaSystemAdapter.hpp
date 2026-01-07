/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** LuaSystemAdapter - Bridge between ECS LuaSystem and Scripting LuaEngine
*/

#pragma once

#include "LuaEngine.hpp"
#include "common/ECS/Systems/LuaSystem/LuaSystem.hpp"
#include "common/ECSWrapper/ECSWorld.hpp"

namespace scripting {
    /**
     * @class LuaSystemAdapter
     * @brief Adapter that bridges the ECS system and the Lua engine.
     * 
     * This adapter provides a clean integration between:
     * - common/ECS/Systems/LuaSystem (generic ECS system)
     * - server/Scripting/LuaEngine (server-side Lua execution)
     * 
     * It wraps a LuaEngine and exposes it in a way that LuaSystem can use,
     * converting between Registry-based addresses and ECSWorld-based entities.
     */
    class LuaSystemAdapter : public ecs::LuaSystem {
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
         * Overrides the base LuaSystem to properly integrate with LuaEngine.
         * 
         * @param registry Reference to the ECS registry
         * @param deltaTime Time elapsed since last frame
         */
        void update(ecs::Registry &registry, float deltaTime) override;

       private:
        LuaEngine *_luaEngine;
        ecs::wrapper::ECSWorld *_world;
    };

}  // namespace scripting
