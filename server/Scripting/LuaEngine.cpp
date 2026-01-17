/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** LuaEngine implementation
*/

#include "LuaEngine.hpp"
#include <filesystem>
#include <unordered_set>

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

        // Utiliser le helper pour g\u00e9n\u00e9rer automatiquement les bindings Entity
        bindings::bindEntity(_lua, _world, helper);

        // Bindings globaux (world, createEntity, etc.)
        bindings::bindWorld(_lua, _world);
        // Bindings spécifiques au serveur (spawn, random, etc.)
        bindings::bindServerGame(_lua, _world, this);
    }

    bool LuaEngine::loadScript(const std::string &scriptPath) {
        std::scoped_lock lock(_luaMutex);

        namespace fs = std::filesystem;

        // scriptPath is typically like: "test_movement.lua"
        // _scriptPath is typically like: "server/Scripting/scripts/"
        const auto base = fs::path(_scriptPath);
        const fs::path candidate = base / scriptPath;

        auto tryLoad = [&](const fs::path &p) -> bool {
            if (!fs::exists(p)) {
                return false;
            }
            try {
                // Load the script file
                const sol::load_result loadResult = _lua.load_file(p.string());
                if (!loadResult.valid()) {
                    const sol::error err = loadResult;
                    LOG_ERROR("Lua error loading " + p.string() + ": " + std::string(err.what()));
                    return false;
                }

                const sol::protected_function scriptFunc = loadResult;

                // Execute in global environment
                sol::protected_function_result result = scriptFunc();
                if (!result.valid()) {
                    const sol::error err = result;
                    LOG_ERROR("Lua error executing " + p.string() + ": " + std::string(err.what()));
                    return false;
                }

                // Create a table that captures the state after script execution
                // Copy only the essential functions (onUpdate, onInit, etc.) not everything
                sol::table scriptTable = _lua.create_table();
                sol::table globals = _lua.globals();

                // Copy only known script functions to avoid deep copy issues
                if (globals["onUpdate"].valid()) {
                    scriptTable["onUpdate"] = globals["onUpdate"];
                }
                if (globals["onInit"].valid()) {
                    scriptTable["onInit"] = globals["onInit"];
                }
                if (globals["onDestroy"].valid()) {
                    scriptTable["onDestroy"] = globals["onDestroy"];
                }

                _scriptCache[scriptPath] = scriptTable;
                LOG_INFO("Loaded Lua script: " + scriptPath + " (" + p.string() + ")");
                return true;
            } catch (const sol::error &e) {
                LOG_ERROR("Lua error loading " + p.string() + ": " + std::string(e.what()));
                return false;
            }
        };

        // 1) Try as-is relative to current working directory
        if (tryLoad(candidate)) {
            return true;
        }

        // 2) Fallback: if CLion runs from a build directory, try going up a few levels
        //    and re-appending server/Scripting/scripts.
        //    This keeps things simple without requiring extra CMake configuration.
        fs::path cwd = fs::current_path();
        for (int up = 0; up < 6; ++up) {
            fs::path probeRoot = cwd;
            for (int i = 0; i < up; ++i) {
                probeRoot = probeRoot.parent_path();
            }
            fs::path probe = probeRoot / "server" / "Scripting" / "scripts" / scriptPath;
            if (tryLoad(probe)) {
                return true;
            }
        }

        // Avoid log spam: only print the "not found" error once per script name.
        static std::unordered_set<std::string> loggedMissing;
        if (!loggedMissing.contains(scriptPath)) {
            loggedMissing.insert(scriptPath);
            LOG_ERROR("Lua script not found: " + candidate.string());
            LOG_ERROR("  - cwd: " + cwd.string());
            LOG_ERROR("  - scriptPath: " + scriptPath);
            LOG_ERROR("  - basePath: " + base.string());
        }
        return false;
    }

    void LuaEngine::executeUpdate(const std::string &scriptPath, ecs::wrapper::Entity entity,
                                  float deltaTime) {
        std::lock_guard<std::recursive_mutex> lock(_luaMutex);

        if (!_world || !_bindingsInitialized) {
            LOG_ERROR("LuaEngine not properly initialized. Call setWorld() first.");
            return;
        }

        // wave_manager uses cached shared state (not per-entity)
        bool isWaveManager = (scriptPath.find("wave_manager") != std::string::npos);

        if (isWaveManager) {
            // Use cached version for wave_manager (shared state, not per-entity)
            if (_scriptCache.find(scriptPath) == _scriptCache.end()) {
                if (!loadScript(scriptPath)) {
                    return;
                }
            }

            try {
                sol::table script = _scriptCache[scriptPath];
                sol::optional<sol::function> onUpdateOpt = script["onUpdate"];

                if (!onUpdateOpt) {
                    LOG_WARNING("Script " + scriptPath + " has no onUpdate function");
                    return;
                }

                sol::protected_function onUpdate = onUpdateOpt.value();
                sol::protected_function_result result = onUpdate(entity, deltaTime);
                if (!result.valid()) {
                    sol::error err = result;
                    LOG_ERROR("Lua runtime error in " + scriptPath + ": " + std::string(err.what()));
                }
            } catch (const sol::error &e) {
                LOG_ERROR("Lua runtime error in " + scriptPath + ": " + std::string(e.what()));
            } catch (const std::exception &e) {
                LOG_ERROR("C++ exception in executeUpdate: " + std::string(e.what()));
            }
            return;  // Exit here for wave_manager
        }

        // For entity scripts (enemy_*): use simple cached script execution
        // No complex per-entity environments - rely on local variables in Lua

        // Use cached version (same as wave_manager)
        if (_scriptCache.find(scriptPath) == _scriptCache.end()) {
            if (!loadScript(scriptPath)) {
                return;
            }
        }

        try {
            sol::table script = _scriptCache[scriptPath];

            // Validate that the table is still valid
            if (!script.valid()) {
                LOG_ERROR("Cached script table is invalid for: " + scriptPath);
                _scriptCache.erase(scriptPath);  // Remove invalid cache
                return;
            }

            sol::optional<sol::function> onUpdateOpt = script["onUpdate"];

            if (!onUpdateOpt) {
                LOG_WARNING("Script " + scriptPath + " has no onUpdate function");
                return;
            }

            sol::protected_function onUpdate = onUpdateOpt.value();

            // Validate function before calling
            if (!onUpdate.valid()) {
                LOG_ERROR("onUpdate function is invalid for: " + scriptPath);
                return;
            }

            sol::protected_function_result result = onUpdate(entity, deltaTime);
            if (!result.valid()) {
                sol::error err = result;
                LOG_ERROR("Lua runtime error in " + scriptPath + ": " + std::string(err.what()));
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
        std::lock_guard<std::recursive_mutex> lock(_luaMutex);

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

    void LuaEngine::registerGameStartCallback(sol::function callback) {
        std::scoped_lock lock(_luaMutex);
        _gameStartCallbacks.push_back(callback);
        LOG_DEBUG("Registered game start callback (total: " + std::to_string(_gameStartCallbacks.size()) +
                  ")");
    }

    void LuaEngine::fireGameStartCallbacks(const std::string &roomId) {
        std::scoped_lock lock(_luaMutex);

        if (_gameStartCallbacks.empty()) {
            LOG_DEBUG("No game start callbacks registered");
            return;
        }

        LOG_INFO("Firing " + std::to_string(_gameStartCallbacks.size()) +
                 " game start callback(s) for room: " + roomId);

        for (auto &callback : _gameStartCallbacks) {
            try {
                sol::protected_function_result result = callback(roomId);
                if (!result.valid()) {
                    sol::error err = result;
                    LOG_ERROR("Lua error in game start callback: " + std::string(err.what()));
                }
            } catch (const sol::error &e) {
                LOG_ERROR("Lua exception in game start callback: " + std::string(e.what()));
            } catch (const std::exception &e) {
                LOG_ERROR("C++ exception in game start callback: " + std::string(e.what()));
            }
        }
    }

    void LuaEngine::cleanupEntity(uint32_t entityId) {
        std::lock_guard<std::recursive_mutex> lock(_luaMutex);

        auto it = _entityScriptCache.find(entityId);
        if (it != _entityScriptCache.end()) {
            LOG_DEBUG("Cleaning up script cache for entity " + std::to_string(entityId));
            _entityScriptCache.erase(it);
        }
    }

}  // namespace scripting
