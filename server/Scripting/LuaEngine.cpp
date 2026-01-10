/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** LuaEngine implementation
*/

#include "LuaEngine.hpp"
#include <filesystem>

#include "LuaBindings/ComponentBindings.hpp"
#include "LuaBindings/EntityBindings.hpp"
#include "LuaBindings/ServerGameBindings.hpp"
#include "LuaBindings/WorldBindings.hpp"
#include "common/ECS/Components/Health.hpp"
#include "common/ECSWrapper/ECSWorld.hpp"
#include "common/Logger/Logger.hpp"

namespace scripting {
    LuaEngine::LuaEngine(const std::string &scriptPath)
        : _scriptPath(scriptPath), _world(nullptr), _bindingsInitialized(false) {
        _lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table,
                            sol::lib::string);

        LOG_INFO("LuaEngine initialized with script path: " + scriptPath);
        LOG_WARNING("Lua bindings not yet initialized. Call setWorld() before executing scripts.");
    }

    void LuaEngine::setWorld(ecs::wrapper::ECSWorld *world) {
        if (world == nullptr) {
            LOG_ERROR("Attempted to set null world in LuaEngine");
            throw std::invalid_argument("World cannot be nullptr");
        }

        _world = world;

        if (!_bindingsInitialized) {
            initializeBindings();
            _bindingsInitialized = true;
            LOG_INFO("Lua bindings initialized successfully");
        } else {
            LOG_WARNING("World updated in LuaEngine - bindings already initialized");
        }
    }

    void LuaEngine::initializeBindings() {
        // Enregistrer tous les composants et obtenir le helper
        auto &helper = bindings::bindComponents(_lua, _world);

        // Utiliser le helper pour g\u00e9nérer automatiquement les bindings Entity
        bindings::bindEntity(_lua, _world, helper);

        // Bindings globaux (world, createEntity, etc.)
        bindings::bindWorld(_lua, _world);
        // Bindings spécifiques au serveur (spawn, random, etc.)
        bindings::bindServerGame(_lua, _world);
    }

    bool LuaEngine::loadScript(const std::string &scriptPath) {
        std::string fullPath = _scriptPath + scriptPath;

        if (!std::filesystem::exists(fullPath)) {
            LOG_ERROR("Lua script not found: " + fullPath);
            return false;
        }

        try {
            // Execute the script file (loads functions into global environment)
            _lua.script_file(fullPath);

            // Cache the global environment as the script's table
            _scriptCache[scriptPath] = _lua.globals();

            LOG_INFO("Loaded Lua script: " + scriptPath);
            return true;
        } catch (const sol::error &e) {
            LOG_ERROR("Lua error loading " + scriptPath + ": " + std::string(e.what()));
            return false;
        }
    }

    void LuaEngine::executeUpdate(const std::string &scriptPath, ecs::wrapper::Entity entity,
                                  float deltaTime) {
        if (!_world || !_bindingsInitialized) {
            LOG_ERROR("LuaEngine not properly initialized. Call setWorld() first.");
            return;
        }

        if (_scriptCache.find(scriptPath) == _scriptCache.end()) {
            if (!loadScript(scriptPath)) {
                return;
            }
        }

        try {
            sol::optional<sol::function> onUpdate = _lua["onUpdate"];

            if (onUpdate) {
                onUpdate.value()(entity, deltaTime);
            } else {
                LOG_WARNING("Script " + scriptPath + " has no onUpdate function");
            }
        } catch (const sol::error &e) {
            LOG_ERROR("Lua runtime error in " + scriptPath + ": " + std::string(e.what()));
        } catch (const std::exception &e) {
            LOG_ERROR("C++ exception in executeUpdate: " + std::string(e.what()));
        }
    }

    template <typename... Args>
    void LuaEngine::callFunction(const std::string &scriptPath, const std::string &functionName,
                                 Args &&...args) {
        if (!_world || !_bindingsInitialized) {
            LOG_ERROR("LuaEngine not properly initialized. Call setWorld() first.");
            return;
        }

        if (_scriptCache.find(scriptPath) == _scriptCache.end()) {
            if (!loadScript(scriptPath)) {
                return;
            }
        }

        try {
            sol::table script = _scriptCache[scriptPath];
            sol::optional<sol::function> func = script[functionName];

            if (func) {
                func.value()(std::forward<Args>(args)...);
            } else {
                LOG_WARNING("Function " + functionName + " not found in " + scriptPath);
            }
        } catch (const sol::error &e) {
            LOG_ERROR("Lua error calling " + functionName + ": " + std::string(e.what()));
        }
    }
};  // namespace scripting
