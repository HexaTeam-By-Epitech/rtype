/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** ServerGameBindings implementation
*/

#include "ServerGameBindings.hpp"
#include <cstdlib>
#include <ctime>
#include "common/ECS/Components/Buff.hpp"
#include "common/ECS/Components/Collectible.hpp"
#include "common/ECS/Components/Collider.hpp"
#include "common/ECS/Components/Enemy.hpp"
#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/LuaScript.hpp"
#include "common/ECS/Components/Spawner.hpp"
#include "common/ECS/Components/Sprite.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Components/Velocity.hpp"
#include "common/ECS/Components/Wall.hpp"
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

        // Spawn power-up (temporary buff)
        lua.set_function(
            "spawnPowerUp",
            [world](const std::string &buffType, float duration, float value, float x,
                    float y) -> ecs::wrapper::Entity {
                try {
                    ecs::BuffType type = ecs::BuffType::SpeedBoost;  // Default

                    // Convert string to BuffType
                    if (buffType == "speed")
                        type = ecs::BuffType::SpeedBoost;
                    else if (buffType == "damage")
                        type = ecs::BuffType::DamageBoost;
                    else if (buffType == "firerate")
                        type = ecs::BuffType::FireRateBoost;
                    else if (buffType == "shield")
                        type = ecs::BuffType::Shield;
                    else if (buffType == "regen")
                        type = ecs::BuffType::HealthRegen;

                    auto entity = world->createEntity();
                    entity.with(ecs::Transform(x, y));
                    entity.with(ecs::Collectible(type, duration, value));
                    entity.with(ecs::Collider(20.0f, 20.0f, 0.0f, 0.0f, 8, 0xFFFFFFFF, false));
                    entity.with(ecs::Sprite("powerup.png", {0, 0, 20, 20}, 1.0f, 0.0f, false, false, 0));

                    LOG_INFO("[LUA] Spawned power-up '" + buffType + "' at (" + std::to_string(x) + ", " +
                             std::to_string(y) + ")");

                    return entity;
                } catch (const std::exception &e) {
                    LOG_ERROR("[LUA] Failed to spawn power-up: " + std::string(e.what()));
                    throw;
                }
            });

        // Spawn permanent upgrade
        lua.set_function(
            "spawnUpgrade",
            [world](const std::string &buffType, float value, float x, float y) -> ecs::wrapper::Entity {
                try {
                    ecs::BuffType type = ecs::BuffType::MultiShot;  // Default

                    // Convert string to BuffType
                    if (buffType == "multishot")
                        type = ecs::BuffType::MultiShot;
                    else if (buffType == "doubleshot")
                        type = ecs::BuffType::DoubleShot;
                    else if (buffType == "tripleshot")
                        type = ecs::BuffType::TripleShot;
                    else if (buffType == "piercing")
                        type = ecs::BuffType::PiercingShot;
                    else if (buffType == "homing")
                        type = ecs::BuffType::HomingShot;
                    else if (buffType == "maxhealth")
                        type = ecs::BuffType::MaxHealthIncrease;

                    auto entity = world->createEntity();
                    entity.with(ecs::Transform(x, y));
                    entity.with(ecs::Collectible(type, 0.0f, value));  // 0.0f = permanent
                    entity.with(ecs::Collider(20.0f, 20.0f, 0.0f, 0.0f, 8, 0xFFFFFFFF, false));
                    entity.with(ecs::Sprite("upgrade.png", {0, 0, 20, 20}, 1.0f, 0.0f, false, false, 0));

                    LOG_INFO("[LUA] Spawned upgrade '" + buffType + "' at (" + std::to_string(x) + ", " +
                             std::to_string(y) + ")");

                    return entity;
                } catch (const std::exception &e) {
                    LOG_ERROR("[LUA] Failed to spawn upgrade: " + std::string(e.what()));
                    throw;
                }
            });

        // Spawn health pack
        lua.set_function(
            "spawnHealthPack", [world](int healthRestore, float x, float y) -> ecs::wrapper::Entity {
                try {
                    auto entity = world->createEntity();
                    entity.with(ecs::Transform(x, y));
                    entity.with(ecs::Collectible(healthRestore));
                    entity.with(ecs::Collider(20.0f, 20.0f, 0.0f, 0.0f, 8, 0xFFFFFFFF, false));
                    entity.with(ecs::Sprite("health.png", {0, 0, 20, 20}, 1.0f, 0.0f, false, false, 0));

                    LOG_INFO("[LUA] Spawned health pack at (" + std::to_string(x) + ", " + std::to_string(y) +
                             ")");

                    return entity;
                } catch (const std::exception &e) {
                    LOG_ERROR("[LUA] Failed to spawn health pack: " + std::string(e.what()));
                    throw;
                }
            });

        // Spawn wall/obstacle
        lua.set_function(
            "spawnWall",
            [world](float x, float y, float width, float height, sol::optional<bool> destructible,
                    sol::optional<int> health) -> ecs::wrapper::Entity {
                try {
                    bool isDestructible = destructible.value_or(false);
                    int wallHealth = health.value_or(0);

                    auto entity = world->createEntity();
                    entity.with(ecs::Transform(x, y));
                    entity.with(ecs::Wall(isDestructible));
                    entity.with(ecs::Collider(width, height, 0.0f, 0.0f, 16, 0xFFFFFFFF, false));
                    entity.with(ecs::Sprite("wall.png",
                                            {0, 0, static_cast<int>(width), static_cast<int>(height)}, 1.0f,
                                            0.0f, false, false, 0));

                    if (isDestructible && wallHealth > 0) {
                        entity.with(ecs::Health(wallHealth, wallHealth));
                    }

                    LOG_INFO("[LUA] Spawned wall at (", x, ", ", y, ") - Size: ", width, "x", height,
                             isDestructible ? " [Destructible]" : " [Solid]");

                    return entity;
                } catch (const std::exception &e) {
                    LOG_ERROR("[LUA] Failed to spawn wall: ", e.what());
                    throw;
                }
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
                ecs::SpawnRequest request{x, y, enemyType, scriptPath, health, scoreValue, 0.0f};
                spawnerComp.queueSpawn(request);

                LOG_DEBUG("[LUA] Queued spawn for ", enemyType, " at (", x, ", ", y, ")");
            } catch (const std::exception &e) {
                LOG_ERROR("[LUA] queueSpawn exception: ", e.what());
            }
        });

        lua.set_function("setSpawnerConfig", [world](ecs::wrapper::Entity spawner, sol::table configTable) {
            try {
                if (!world) {
                    LOG_ERROR("[LUA] setSpawnerConfig: world is null");
                    return;
                }

                if (!spawner.isValid()) {
                    LOG_WARNING("[LUA] Cannot set spawner config: invalid spawner entity (address: ",
                                spawner.getAddress(), ")");
                    return;
                }

                if (!spawner.has<ecs::Spawner>()) {
                    LOG_WARNING("[LUA] Entity (", spawner.getAddress(), ") does not have Spawner component");
                    return;
                }

                ecs::Spawner &spawnerComp = spawner.get<ecs::Spawner>();
                ecs::SpawnerConfig config;

                // Parse waves
                sol::optional<sol::table> wavesTableOpt = configTable["waves"];
                if (wavesTableOpt) {
                    sol::table wavesTable = wavesTableOpt.value();
                    for (auto &wavePair : wavesTable) {
                        sol::table waveTable = wavePair.second;
                        ecs::WaveConfig waveConfig;

                        // Parse enemies in wave (using enemyConfigs key from Lua)
                        sol::optional<sol::table> enemiesTableOpt = waveTable["enemyConfigs"];
                        if (!enemiesTableOpt) {
                            enemiesTableOpt = waveTable["enemies"];
                        }
                        if (enemiesTableOpt) {
                            sol::table enemiesTable = enemiesTableOpt.value();
                            for (auto &enemyPair : enemiesTable) {
                                sol::table enemyTable = enemyPair.second;
                                ecs::SpawnRequest request;
                                request.x = enemyTable.get_or("x", 0.0f);
                                request.y = enemyTable.get_or("y", 0.0f);
                                request.enemyType = enemyTable.get_or<std::string>("type", "basic");
                                request.scriptPath = enemyTable.get_or<std::string>("script", "");
                                request.health = enemyTable.get_or("health", 100.0f);
                                request.scoreValue = enemyTable.get_or("scoreValue", 100);
                                request.spawnDelay = enemyTable.get_or("delay", 0.0f);
                                request.hasSpawned = false;
                                waveConfig.enemies.push_back(request);
                            }
                        }

                        waveConfig.spawnInterval = waveTable.get_or("spawnInterval", 1.0f);
                        config.waves.push_back(waveConfig);
                    }
                }

                // Parse wave intervals
                sol::optional<sol::table> intervalsTableOpt = configTable["wavesIntervals"];
                if (intervalsTableOpt) {
                    sol::table intervalsTable = intervalsTableOpt.value();
                    for (auto &intervalPair : intervalsTable) {
                        int interval = intervalPair.second.as<int>();
                        config.wavesIntervals.push_back(interval);
                    }
                }

                spawnerComp.setConfig(config);

                LOG_INFO("[LUA] Set spawner config for entity ", spawner.getAddress(), " with ",
                         config.waves.size(), " waves");
            } catch (const std::exception &e) {
                LOG_ERROR("[LUA] setSpawnerConfig exception: ", e.what());
            }
        });

        LOG_INFO("Server game bindings initialized");
    }

}  // namespace scripting::bindings
