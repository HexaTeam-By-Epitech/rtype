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
        : _luaEngine(luaEngine), _world(world) {
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

        static int logCounter = 0;
        if (++logCounter % 60 == 0) {
            //LOG_INFO("LuaSystemAdapter: Processing " + std::to_string(entities.size()) +
            //        " entities with LuaScript component");
        }

        for (const auto &entityAddr : entities) {
            try {
                // Check if entity still has LuaScript component
                // (may have been destroyed by previous script execution)
                try {
                    if (!registry.hasComponent<ecs::LuaScript>(entityAddr)) {
                        continue;
                    }
                } catch (...) {
                    // Entity doesn't exist anymore, skip it
                    _luaEngine->cleanupEntity(entityAddr);
                    continue;
                }

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
                    // Clean up script cache for invalid entity
                    _luaEngine->cleanupEntity(entityAddr);
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

    ecs::ComponentMask LuaSystemAdapter::getComponentMask() const {
        return (1ULL << ecs::getComponentType<ecs::LuaScript>());
    }

}  // namespace scripting
