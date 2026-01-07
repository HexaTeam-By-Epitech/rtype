/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** LuaSystem
*/

#include "LuaSystem.hpp"
#include "common/ECS/Registry.hpp"
#include "common/Logger/Logger.hpp"

// Forward declaration to avoid circular dependency
namespace scripting {
    class LuaEngine;
}

// We'll use a forward-declared interface here
// The actual LuaEngine will be provided by the server
namespace ecs {
    LuaSystem::LuaSystem(scripting::LuaEngine *luaEngine) : _luaEngine(luaEngine) {
        if (!luaEngine) {
            LOG_ERROR("LuaSystem initialized with null LuaEngine");
        }
    }

    void LuaSystem::update(Registry &registry, float deltaTime) {
        if (!_luaEngine) {
            LOG_ERROR("LuaSystem has no LuaEngine set");
            return;
        }

        auto entities = registry.getEntitiesWithMask(this->getComponentMask());

        for (const auto &entityAddr : entities) {
            try {
                auto &luaScript = registry.getComponent<LuaScript>(entityAddr);
                const std::string &scriptPath = luaScript.getScriptPath();

                if (scriptPath.empty()) {
                    continue;
                }

                // The LuaEngine will handle the actual script execution
                // We need to pass the entity address wrapped in Entity class
                // This will be done via a public interface method
                // For now, we'll log that we're executing
                // The actual implementation depends on LuaEngine's interface

                LOG_DEBUG("Executing Lua script: " + scriptPath + " for entity " +
                          std::to_string(entityAddr));

                // Note: The executeUpdate call needs to be added here
                // but requires including the full LuaEngine header
                // This will be done in the server-side integration

            } catch (const std::exception &e) {
                LOG_ERROR("Error in LuaSystem for entity " + std::to_string(entityAddr) + ": " +
                          std::string(e.what()));
            }
        }
    }

    ComponentMask LuaSystem::getComponentMask() const {
        return (1ULL << getComponentType<LuaScript>());
    }
}  // namespace ecs
