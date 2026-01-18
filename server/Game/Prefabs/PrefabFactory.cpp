/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** PrefabFactory.cpp
*/

#include "server/Game/Prefabs/PrefabFactory.hpp"
#include <stdexcept>
#include "common/ECS/Components/Buff.hpp"
#include "common/ECS/Components/Collectible.hpp"
#include "common/ECS/Components/Collider.hpp"
#include "common/ECS/Components/Enemy.hpp"
#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/LuaScript.hpp"
#include "common/ECS/Components/Player.hpp"
#include "common/ECS/Components/Projectile.hpp"
#include "common/ECS/Components/Sprite.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Components/Velocity.hpp"
#include "common/ECS/Components/Wall.hpp"
#include "common/ECS/Components/Weapon.hpp"
#include "common/ECS/Registry.hpp"
#include "common/ECSWrapper/ECSWorld.hpp"
#include "common/Logger/Logger.hpp"

namespace server {

    ecs::Address PrefabFactory::createPlayer(ecs::wrapper::ECSWorld &world, uint32_t playerId,
                                             const std::string &playerName) {
        try {
            ecs::wrapper::Entity player =
                world.createEntity()
                    .with(ecs::Player(0, 3, playerId))  // score=0, lives=3
                    .with(ecs::Transform(50.0f, 300.0f))
                    .with(ecs::Velocity(0.0f, 0.0f, 200.0f))  // 200 units/sec max speed
                    .with(ecs::Health(100, 100))
                    .with(ecs::Collider(50.0f, 50.0f, 0.0f, 0.0f, 1, 0xFFFFFFFF, false))
                    .with(ecs::Weapon(10.0f, 0.0f, 0, 25));  // fire rate: 10 shots/sec, type 0, 25 damage

            LOG_INFO("✓ Player created: ", playerName, " (ID: ", playerId, ")");
            return player.getAddress();
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create player: ", e.what());
            return 0;
        }
    }

    ecs::Address PrefabFactory::createEnemy(ecs::wrapper::ECSWorld &world, int enemyType, float posX,
                                            float posY) {
        try {
            EnemySpawnData spawnData = _getEnemySpawnData(enemyType);

            ecs::wrapper::Entity enemy =
                world.createEntity()
                    .with(ecs::Enemy(enemyType, spawnData.scoreValue, 0))
                    .with(ecs::Transform(posX, posY))
                    .with(ecs::Velocity(-1.0f, 0.0f, spawnData.speed))
                    .with(ecs::Health(spawnData.health, spawnData.health))
                    .with(ecs::Collider(spawnData.colliderWidth, spawnData.colliderHeight, 0.0f, 0.0f, 2,
                                        0xFFFFFFFF, false))
                    .with(ecs::Weapon(3.0f, 0.0f, 1, 15));  // 3 shots/sec, less damage

            LOG_INFO("✓ Enemy spawned: Type ", enemyType, " at (", posX, ", ", posY, ")");
            return enemy.getAddress();
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create enemy: ", e.what());
            return 0;
        }
    }

    ecs::Address PrefabFactory::createProjectile(ecs::wrapper::ECSWorld &world, uint32_t ownerId, float posX,
                                                 float posY, float dirX, float dirY, float speed, int damage,
                                                 bool friendly) {
        try {
            ecs::wrapper::Entity projectile =
                world.createEntity()
                    .with(ecs::Projectile(damage, 10.0f, ownerId, friendly))
                    .with(ecs::Transform(posX, posY))
                    .with(ecs::Velocity(dirX, dirY, speed))
                    .with(ecs::Collider(10.0f, 10.0f, 0.0f, 0.0f, 4, 0xFFFFFFFF, true));

            return projectile.getAddress();
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create projectile: ", e.what());
            return 0;
        }
    }

    PrefabFactory::EnemySpawnData PrefabFactory::_getEnemySpawnData(int enemyType) {
        switch (enemyType) {
            case 0:  // Basic enemy
                return {150.0f, 50, 100, 40.0f, 40.0f};
            case 1:  // Heavy enemy
                return {100.0f, 100, 200, 60.0f, 60.0f};
            case 2:  // Fast enemy
                return {200.0f, 30, 150, 30.0f, 30.0f};
            case 3:  // Boss-like enemy
                return {120.0f, 200, 500, 80.0f, 80.0f};
            default:  // Default to basic
                return {150.0f, 50, 100, 40.0f, 40.0f};
        }
    }

    ecs::Address PrefabFactory::createPowerUp(ecs::wrapper::ECSWorld &world, ecs::BuffType buffType,
                                              float duration, float value, float posX, float posY) {
        try {
            ecs::wrapper::Entity powerUp =
                world.createEntity()
                    .with(ecs::Transform(posX, posY))
                    .with(ecs::Collectible(buffType, duration, value))
                    .with(ecs::Collider(20.0f, 20.0f, 0.0f, 0.0f, 8, 0xFFFFFFFF, false))
                    // TODO: Add appropriate sprite for power-up
                    .with(ecs::Sprite("powerup.png", {0, 0, 20, 20}, 1.0f, 0.0f, false, false, 0));

            LOG_INFO("✓ Power-up spawned at (", posX, ", ", posY, ")");
            return powerUp.getAddress();
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create power-up: ", e.what());
            return 0;
        }
    }

    int PrefabFactory::_enemyTypeFromString(const std::string &enemyType) {
        if (enemyType == "basic") {
            return 0;
        } else if (enemyType == "advanced" || enemyType == "heavy") {
            return 1;
        } else if (enemyType == "fast") {
            return 2;
        } else if (enemyType == "boss") {
            return 3;
        }
        return 0;  // Default to basic
    }

    ecs::Address PrefabFactory::createEnemy(ecs::wrapper::ECSWorld &world, const std::string &enemyType,
                                            float posX, float posY, float health, int scoreValue,
                                            const std::string &scriptPath) {
        try {
            int typeId = _enemyTypeFromString(enemyType);
            EnemySpawnData spawnData = _getEnemySpawnData(typeId);

            // Use custom health/score if provided, otherwise use defaults
            int finalHealth = (health > 0) ? static_cast<int>(health) : spawnData.health;
            int finalScore = (scoreValue > 0) ? scoreValue : spawnData.scoreValue;

            ecs::wrapper::Entity enemy =
                world.createEntity()
                    .with(ecs::Enemy(typeId, finalScore, 0))
                    .with(ecs::Transform(posX, posY))
                    .with(ecs::Velocity(-1.0f, 0.0f, spawnData.speed))
                    .with(ecs::Health(finalHealth, finalHealth))
                    .with(ecs::Collider(spawnData.colliderWidth, spawnData.colliderHeight, 0.0f, 0.0f, 2,
                                        0xFFFFFFFF, false))
                    .with(ecs::Weapon(3.0f, 0.0f, 1, 15));  // 3 shots/sec, type 1, 15 damage

            // Add Lua script if provided
            if (!scriptPath.empty()) {
                enemy.with(ecs::LuaScript(scriptPath));
            }

            LOG_INFO("✓ Enemy spawned: ", enemyType, " (type ", typeId, ") at (", posX, ", ", posY, ")");
            return enemy.getAddress();
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create enemy: ", e.what());
            return 0;
        }
    }

    ecs::Address PrefabFactory::createHealthPack(ecs::wrapper::ECSWorld &world, int healthRestore, float posX,
                                                 float posY) {
        try {
            ecs::wrapper::Entity healthPack =
                world.createEntity()
                    .with(ecs::Transform(posX, posY))
                    .with(ecs::Collectible(healthRestore))
                    .with(ecs::Collider(20.0f, 20.0f, 0.0f, 0.0f, 8, 0xFFFFFFFF, false))
                    // TODO: Add appropriate sprite for health pack
                    .with(ecs::Sprite("health.png", {0, 0, 20, 20}, 1.0f, 0.0f, false, false, 0));

            LOG_INFO("✓ Health pack spawned at (", posX, ", ", posY, ")");
            return healthPack.getAddress();
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create health pack: ", e.what());
            return 0;
        }
    }

    ecs::Address PrefabFactory::createEnemyFromRegistry(ecs::Registry &registry, const std::string &enemyType,
                                                        float posX, float posY, float health, int scoreValue,
                                                        const std::string &scriptPath) {
        try {
            int typeId = _enemyTypeFromString(enemyType);
            EnemySpawnData spawnData = _getEnemySpawnData(typeId);

            // Use custom health/score if provided, otherwise use defaults
            int finalHealth = (health > 0) ? static_cast<int>(health) : spawnData.health;
            int finalScore = (scoreValue > 0) ? scoreValue : spawnData.scoreValue;

            ecs::Address enemy = registry.newEntity();

            registry.setComponent<ecs::Transform>(enemy, ecs::Transform(posX, posY));
            registry.setComponent<ecs::Velocity>(enemy, ecs::Velocity(-1.0f, 0.0f, spawnData.speed));
            registry.setComponent<ecs::Health>(enemy, ecs::Health(finalHealth, finalHealth));
            registry.setComponent<ecs::Enemy>(enemy, ecs::Enemy(typeId, finalScore, 0));
            registry.setComponent<ecs::Collider>(
                enemy, ecs::Collider(spawnData.colliderWidth, spawnData.colliderHeight, 0.0f, 0.0f, 2,
                                     0xFFFFFFFF, false));
            registry.setComponent<ecs::Weapon>(enemy, ecs::Weapon(3.0f, 0.0f, 1, 15));

            if (!scriptPath.empty()) {
                registry.setComponent<ecs::LuaScript>(enemy, ecs::LuaScript(scriptPath));
            }

            LOG_INFO("✓ Enemy spawned: ", enemyType, " (type ", typeId, ") at (", posX, ", ", posY, ")");
            return enemy;
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create enemy from registry: ", e.what());
            return 0;
        }
    }

    ecs::Address PrefabFactory::createWall(ecs::wrapper::ECSWorld &world, float posX, float posY, float width,
                                           float height, bool destructible, int health) {
        try {
            ecs::wrapper::Entity wall =
                world.createEntity()
                    .with(ecs::Transform(posX, posY))
                    .with(ecs::Wall(destructible))
                    .with(ecs::Collider(width, height, 0.0f, 0.0f, 16, 0xFFFFFFFF,
                                        false))  // Layer 16 for walls
                    .with(ecs::Sprite("wall.png", {0, 0, static_cast<int>(width), static_cast<int>(height)},
                                      1.0f, 0.0f, false, false, 0));

            // Add health if destructible
            if (destructible && health > 0) {
                wall.with(ecs::Health(health, health));
            }

            LOG_INFO("✓ Wall spawned at (", posX, ", ", posY, ") - Size: ", width, "x", height,
                     destructible ? " [Destructible]" : " [Solid]");
            return wall.getAddress();
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create wall: ", e.what());
            return 0;
        }
    }

}  // namespace server
