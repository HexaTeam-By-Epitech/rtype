/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** ServerGameBindings implementation
*/

#include "ServerGameBindings.hpp"
#include <cstdlib>
#include <ctime>
#include "common/ECS/Components/Enemy.hpp"
#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/LuaScript.hpp"
#include "common/ECS/Components/Spawner.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Components/Velocity.hpp"
#include "common/Logger/Logger.hpp"

namespace scripting::bindings {

    void bindServerGame(sol::state &lua, ecs::wrapper::ECSWorld *world) {
        if (!world) {
            LOG_ERROR("Cannot bind server game functions: world is null");
            return;
        }

        // Seed random number generator (do this once)
        static bool seeded = false;
        if (!seeded) {
            std::srand(static_cast<unsigned int>(std::time(nullptr)));
            seeded = true;
        }

        // Spawn a basic enemy at position
        lua.set_function("spawnEnemy",
                         [world](float x, float y, const std::string &enemyType) -> ecs::wrapper::Entity {
                             try {
                                 auto entity = world->createEntity();

                                 entity.with(ecs::Transform(x, y));
                                 entity.with(ecs::Velocity(-1.0f, 0.0f, 100.0f));  // Move left
                                 entity.with(ecs::Health(100, 100));

                                 LOG_INFO("[LUA] Spawned enemy '" + enemyType + "' at (" + std::to_string(x) +
                                          ", " + std::to_string(y) + ")");

                                 return entity;
                             } catch (const std::exception &e) {
                                 LOG_ERROR("[LUA] Failed to spawn enemy: " + std::string(e.what()));
                                 throw;
                             }
                         });

        // Spawn a projectile
        lua.set_function(
            "spawnProjectile",
            [world](float x, float y, float dirX, float dirY, float speed) -> ecs::wrapper::Entity {
                try {
                    auto entity = world->createEntity();

                    entity.with(ecs::Transform(x, y));
                    entity.with(ecs::Velocity(dirX, dirY, speed));

                    LOG_DEBUG("[LUA] Spawned projectile at (" + std::to_string(x) + ", " + std::to_string(y) +
                              ")");

                    return entity;
                } catch (const std::exception &e) {
                    LOG_ERROR("[LUA] Failed to spawn projectile: " + std::string(e.what()));
                    throw;
                }
            });

        // Get current time (useful for timing/animations)
        lua.set_function("getTime", []() -> float {
            return static_cast<float>(std::clock()) / static_cast<float>(CLOCKS_PER_SEC);
        });

        // Math helpers
        lua.set_function("distance", [](float x1, float y1, float x2, float y2) -> float {
            float dx = x2 - x1;
            float dy = y2 - y1;
            return std::sqrt(dx * dx + dy * dy);
        });

        lua.set_function("normalize", [&lua](float x, float y) -> sol::table {
            float length = std::sqrt(x * x + y * y);
            if (length == 0.0f)
                length = 1.0f;

            sol::state_view lua_view(lua.lua_state());
            sol::table result = lua_view.create_table();
            result["x"] = x / length;
            result["y"] = y / length;
            return result;
        });

        // Queue a spawn request through a Spawner entity
        // Usage: queueSpawn(spawnerEntity, x, y, type, scriptPath, health, scoreValue)
        lua.set_function("queueSpawn", [world](ecs::wrapper::Entity spawner, float x, float y,
                                               const std::string &enemyType, const std::string &scriptPath,
                                               float health, int scoreValue) {
            try {
                if (!world) {
                    LOG_ERROR("[LUA] queueSpawn: world is null");
                    return;
                }

                if (!spawner.isValid()) {
                    LOG_WARNING("[LUA] Cannot queue spawn: invalid spawner entity (address: ",
                                spawner.getAddress(), ")");
                    return;
                }

                if (!spawner.has<ecs::Spawner>()) {
                    LOG_WARNING("[LUA] Entity (", spawner.getAddress(), ") does not have Spawner component");
                    return;
                }

                ecs::Spawner &spawnerComp = spawner.get<ecs::Spawner>();
                ecs::SpawnRequest request{x, y, enemyType, scriptPath, health, scoreValue};
                spawnerComp.queueSpawn(request);

                LOG_DEBUG("[LUA] Queued spawn for ", enemyType, " at (", x, ", ", y, ")");
            } catch (const std::exception &e) {
                LOG_ERROR("[LUA] queueSpawn exception: ", e.what());
            }
        });

        LOG_INFO("Server game bindings initialized");
    }

}  // namespace scripting::bindings
