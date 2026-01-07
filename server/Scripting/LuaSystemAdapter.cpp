/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** LuaSystemAdapter implementation
*/

#include "LuaSystemAdapter.hpp"
#include "common/ECS/Components/LuaScript.hpp"
#include "common/Logger/Logger.hpp"

namespace scripting {

    LuaSystemAdapter::LuaSystemAdapter(LuaEngine *luaEngine, ecs::wrapper::ECSWorld *world)
        : ecs::LuaSystem(luaEngine), _luaEngine(luaEngine), _world(world) {
        if (!luaEngine) {
            LOG_ERROR("LuaSystemAdapter: LuaEngine is null");
        }
        if (!world) {
            LOG_ERROR("LuaSystemAdapter: ECSWorld is null");
        }
    }

    void LuaSystemAdapter::update(ecs::Registry &registry, float deltaTime) {
        if (!_luaEngine || !_world) {
            LOG_ERROR("LuaSystemAdapter not properly initialized");
            return;
        }

        auto entities = registry.getEntitiesWithMask(this->getComponentMask());

        for (const auto &entityAddr : entities) {
            try {
                auto &luaScript = registry.getComponent<ecs::LuaScript>(entityAddr);
                const std::string &scriptPath = luaScript.getScriptPath();

                if (scriptPath.empty()) {
                    continue;
                }

                // Convert Registry address to ECSWorld Entity wrapper
                ecs::wrapper::Entity entity = _world->getEntity(entityAddr);

                if (!entity.isValid()) {
                    LOG_WARNING("Invalid entity " + std::to_string(entityAddr) +
                                " for script: " + scriptPath);
                    continue;
                }

                // Execute the script via LuaEngine
                _luaEngine->executeUpdate(scriptPath, entity, deltaTime);

            } catch (const std::exception &e) {
                LOG_ERROR("Error executing Lua script for entity " + std::to_string(entityAddr) + ": " +
                          std::string(e.what()));
            }
        }
    }

}  // namespace scripting
