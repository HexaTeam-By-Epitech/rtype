/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** LuaEngine - Core Lua script manager
*/

#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <sol/sol.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include "common/ECSWrapper/ECSWorld.hpp"

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
         * @brief Set the ECS world for entity operations.
         * 
         * This method must be called before executing any Lua scripts that interact
         * with entities or components. It initializes the Lua bindings with the world context.
         * 
         * @param world Pointer to the ECS world (must not be nullptr)
         * @throws std::invalid_argument if world is nullptr
         */
        void setWorld(ecs::wrapper::ECSWorld *world);

        /**
         * @brief Load and cache a Lua script.
         * @param scriptPath Relative path to script file
         * @return true if loaded successfully
         */
        bool loadScript(const std::string &scriptPath);

        /**
         * @brief Execute onUpdate function for an entity's script.
         * @param scriptPath Path to the script
         * @param entity Entity wrapper
         * @param deltaTime Frame delta time
         */
        void executeUpdate(const std::string &scriptPath, ecs::wrapper::Entity entity, float deltaTime);

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

        /**
         * @brief Register a Lua callback to be called when the game starts.
         * @param callback Lua function to call
         */
        void registerGameStartCallback(sol::function callback);

        /**
         * @brief Fire all registered game start callbacks.
         * @param roomId The ID of the room where the game started
         */
        void fireGameStartCallbacks(const std::string &roomId);

        /**
         * @brief Clean up script cache for a destroyed entity.
         * @param entityId The ID of the entity to clean up
         */
        void cleanupEntity(uint32_t entityId);

       private:
        sol::state _lua;
        std::string _scriptPath;
        std::unordered_map<std::string, sol::table> _scriptCache;
        // Per-entity script state (for enemy scripts with local variables)
        std::unordered_map<uint32_t, std::unordered_map<std::string, sol::table>> _entityScriptCache;
        ecs::wrapper::ECSWorld *_world;
        bool _bindingsInitialized;
        mutable std::recursive_mutex _luaMutex;  // Protects _lua and _scriptCache from concurrent access

        void initializeBindings();
        //void bindComponents();

        // Game start callbacks registered via onGameStart()
        std::vector<sol::function> _gameStartCallbacks;
    };

}  // namespace scripting
