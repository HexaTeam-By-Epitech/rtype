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
#include "LuaBindings/WorldBindings.hpp"
#include "common/ECS/Components/Health.hpp"
#include "common/ECSWrapper/ECSWorld.hpp"
#include "common/Logger/Logger.hpp"

namespace scripting {
    LuaEngine::LuaEngine(const std::string &scriptPath) : _scriptPath(scriptPath), _world(nullptr) {
        _lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table,
                            sol::lib::string);

        initializeBindings();
        LOG_INFO("LuaEngine initialized with script path: " + scriptPath);
    }

    void LuaEngine::setWorld(ecs::wrapper::ECSWorld *world) {
        _world = world;
    }

    void LuaEngine::initializeBindings() {
        bindings::bindComponents(_lua, _world);
        bindings::bindEntity(_lua, _world);
        bindings::bindWorld(_lua, _world);
    }

    bool LuaEngine::loadScript(const std::string &scriptPath) {
        std::string fullPath = _scriptPath + scriptPath;

        if (!std::filesystem::exists(fullPath)) {
            LOG_ERROR("Lua script not found: " + fullPath);
            return false;
        }

        try {
            sol::table script = _lua.script_file(fullPath);
            _scriptCache[scriptPath] = script;
            LOG_INFO("Loaded Lua script: " + scriptPath);
            return true;
        } catch (const sol::error &e) {
            LOG_ERROR("Lua error loading " + scriptPath + ": " + std::string(e.what()));
            return false;
        }
    }

    void LuaEngine::executeUpdate(const std::string &scriptPath, ecs::wrapper::Entity entity,
                                  float deltaTime) {
        if (!_world) {
            LOG_ERROR("World not set in LuaEngine");
            return;
        }

        if (_scriptCache.find(scriptPath) == _scriptCache.end()) {
            if (!loadScript(scriptPath)) {
                return;
            }
        }

        try {
            sol::table script = _scriptCache[scriptPath];
            sol::optional<sol::function> onUpdate = script["onUpdate"];

            if (onUpdate) {
                onUpdate.value()(entity, deltaTime);
            } else {
                LOG_WARNING("Script " + scriptPath + " has no onUpdate function");
            }
        } catch (const sol::error &e) {
            LOG_ERROR("Lua runtime error in " + scriptPath + ": " + std::string(e.what()));
        }
    }

    template <typename... Args>
    void LuaEngine::callFunction(const std::string &scriptPath, const std::string &functionName,
                                 Args &&...args) {
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
