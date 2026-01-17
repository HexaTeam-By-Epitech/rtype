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
#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/Sprite.hpp"
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

        LOG_INFO("Server game bindings initialized");
    }

}  // namespace scripting::bindings
