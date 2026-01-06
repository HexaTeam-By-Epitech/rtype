/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** LuaEngine - Core Lua script manager
*/

#pragma once

#include <memory>
#include <sol/sol.hpp>
#include <string>
#include <unordered_map>
#include "common/ECS/Registry.hpp"

namespace scripting {
    /**
     * @class LuaEngine
     * @brief Manages Lua state and script execution for the server.
     * 
     * Handles loading, caching, and executing Lua scripts associated with entities.
     * Provides bindings for ECS components and game logic.
     */
    class LuaEngine {
       public:
        /**
         * @brief Constructor with scripts directory path.
         * @param scriptsPath Base path for Lua scripts
         */
        explicit LuaEngine(const std::string &scriptPath = "server/Scripting/scripts/");

        /**
         * @brief Set the ECS registry for entity operations.
         * @param registry Pointer to the game registry
         */
        void setRegistry(ecs::Registry *registry);

        /**
         * @brief Load and cache a Lua script.
         * @param scriptPath Relative path to script file
         * @return true if loaded successfully
         */
        bool loadScript(const std::string &scriptPath);

        /**
         * @brief Execute onUpdate function for an entity's script.
         * @param scriptPath Path to the script
         * @param entityAddress Entity address
         * @param deltaTime Frame delta time
         */
        void executeUpdate(const std::string &scriptPath, ecs::Address entityAddress, float deltaTime);

        /**
         * @brief Call a specific Lua function.
         * @param scriptPath Path to the script
         * @param functionName Name of the function to call
         * @param args Variadic arguments to pass to Lua
         */
        template <typename... Args>
        void callFunction(const std::string &scriptPath, const std::string &functionName, Args &&...args);

        /**
         * @brief Get direct access to Lua state (advanced usage).
         * @return sol::state& Reference to the Lua state
         */
        sol::state &getLuaState() { return _lua; };

       private:
        sol::state _lua;
        std::string _scriptPath;
        std::unordered_map<std::string, sol::table> _scriptCache;
        ecs::Registry *_registry;
    };
}  // namespace scripting
