/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** PrefabFactory.cpp
*/

#include "common/ECS/Prefabs/PrefabFactory.hpp"
#include "common/ECS/Components/Collectible.hpp"
#include "common/ECS/Components/LuaScript.hpp"

namespace ecs {

    Address PrefabFactory::createPlayer(Registry &registry, uint32_t playerId) {
        try {
            Address player = registry.newEntity();

            registry.setComponent(player, Player(0, 3, playerId));
            registry.setComponent(player, Transform(100.0F, 300.0F));
            registry.setComponent(player, Velocity(0.0F, 0.0F, 200.0F));
            registry.setComponent(player, Health(100, 100));
            registry.setComponent(player, Collider(50.0F, 50.0F, 0.0F, 0.0F, 1, 0xFFFFFFFF, false));
            registry.setComponent(player,
                                  Weapon(7.0F, 0.0F, 0, 25));  // fire rate: 7 shots/sec, type 0, 25 damage
            LOG_INFO("✓ Player created with ID: ", playerId);

            return player;
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create player: ", e.what());
            return 0;
        }
    }

    Address PrefabFactory::createEnemy(Registry &registry, int enemyType, float posX, float posY) {
        try {
            EnemySpawnData spawnData = _getEnemySpawnData(enemyType);
            Address enemy = registry.newEntity();
            registry.setComponent(enemy, Enemy(enemyType, spawnData.scoreValue));
            registry.setComponent(enemy, Transform(posX, posY));
            registry.setComponent(enemy, Velocity(-spawnData.speed, 0.0f, 0.0f));
            registry.setComponent(enemy, Health(spawnData.health, spawnData.health));
            registry.setComponent(enemy, Collider(spawnData.colliderWidth, spawnData.colliderHeight, 0.0f,
                                                  0.0f, 2, 0xFFFFFFFF, false));
            registry.setComponent(enemy, Weapon(3.0f, 0.8f, 1, 15));  // 3 shots/sec, less damage
            LOG_INFO("✓ Enemy created of type: ", enemyType);
            return enemy;
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create enemy: ", e.what());
            return 0;
        }
    }

    Address PrefabFactory::createProjectile(Registry &registry, uint32_t ownerId, float posX, float posY,
                                            float dirX, float dirY, float speed, int damage, bool friendly) {
        try {
            Address projectile = registry.newEntity();

            registry.setComponent(projectile, Projectile(damage, 10.0f, ownerId, friendly));
            registry.setComponent(projectile, Transform(posX, posY));
            registry.setComponent(projectile, Velocity(dirX, dirY, speed));
            registry.setComponent(projectile, Collider(10.0f, 10.0f, 0.0f, 0.0f, 4, 0xFFFFFFFF, true));

            // Add animation components for projectile rendering
            registry.setComponent(projectile, AnimDB::createPlayerBulletAnimations());
            registry.setComponent(projectile, Animation("projectile_fly", true, true));
            registry.setComponent(projectile,
                                  Sprite("Projectiles", {267, 84, 17, 13}, 2.0f, 0.0f, false, false, 0));

            return projectile;
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create projectile: ", e.what());
            return 0;
        }
    }

    Address PrefabFactory::createEnemy(Registry &registry, const std::string &enemyType, float posX,
                                       float posY, float health, int scoreValue,
                                       const std::string &scriptPath) {
        try {
            int typeInt = _enemyTypeFromString(enemyType);
            EnemySpawnData spawnData = _getEnemySpawnData(typeInt);

            Address enemy = registry.newEntity();
            registry.setComponent(enemy, Enemy(typeInt, scoreValue));
            registry.setComponent(enemy, Transform(posX, posY));
            registry.setComponent(enemy, Velocity(-1.0f, 0.0f, spawnData.speed));
            registry.setComponent(enemy, Health(static_cast<int>(health), static_cast<int>(health)));
            registry.setComponent(enemy, Collider(spawnData.colliderWidth, spawnData.colliderHeight, 0.0f,
                                                  0.0f, 2, 0xFFFFFFFF, false));
            registry.setComponent(enemy, Weapon(3.0f, 0.0f, 1, 15));

            // Add Lua script if provided
            if (!scriptPath.empty()) {
                registry.setComponent(enemy, LuaScript(scriptPath));
            }

            LOG_INFO("✓ Enemy '", enemyType, "' created at (", posX, ", ", posY, ")");
            return enemy;
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create enemy: ", e.what());
            return 0;
        }
    }

    Address PrefabFactory::createEnemyFromRegistry(Registry &registry, const std::string &enemyType,
                                                   float posX, float posY, float health, int scoreValue,
                                                   const std::string &scriptPath) {
        try {
            int typeInt = _enemyTypeFromString(enemyType);
            EnemySpawnData spawnData = _getEnemySpawnData(typeInt);

            Address enemy = registry.newEntity();
            registry.setComponent(enemy, Enemy(typeInt, scoreValue));
            registry.setComponent(enemy, Transform(posX, posY));
            registry.setComponent(enemy, Velocity(-1.0f, 0.0f, spawnData.speed));
            registry.setComponent(enemy, Health(static_cast<int>(health), static_cast<int>(health)));
            registry.setComponent(enemy, Collider(spawnData.colliderWidth, spawnData.colliderHeight, 0.0f,
                                                  0.0f, 2, 0xFFFFFFFF, false));
            registry.setComponent(enemy, Weapon(3.0f, 0.0f, 1, 15));

            // Add Lua script if provided
            if (!scriptPath.empty()) {
                registry.setComponent(enemy, LuaScript(scriptPath));
            }

            LOG_INFO("✓ Enemy '", enemyType, "' created at (", posX, ", ", posY, ")");
            return enemy;
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create enemy: ", e.what());
            return 0;
        }
    }

    Address PrefabFactory::createPowerUp(Registry &registry, BuffType buffType, float duration, float value,
                                         float posX, float posY) {
        try {
            Address powerUp = registry.newEntity();
            registry.setComponent(powerUp, Collectible(buffType, duration, value));
            registry.setComponent(powerUp, Transform(posX, posY));
            registry.setComponent(powerUp, Velocity(0.0f, 0.0f, 0.0f));
            registry.setComponent(powerUp, Collider(20.0f, 20.0f, 0.0f, 0.0f, 8, 0xFFFFFFFF, false));

            LOG_INFO("✓ Power-up created at (", posX, ", ", posY, ")");
            return powerUp;
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create power-up: ", e.what());
            return 0;
        }
    }

    Address PrefabFactory::createHealthPack(Registry &registry, int healthRestore, float posX, float posY) {
        try {
            Address healthPack = registry.newEntity();
            registry.setComponent(healthPack, Collectible(healthRestore));
            registry.setComponent(healthPack, Transform(posX, posY));
            registry.setComponent(healthPack, Velocity(0.0f, 0.0f, 0.0f));
            registry.setComponent(healthPack, Collider(20.0f, 20.0f, 0.0f, 0.0f, 8, 0xFFFFFFFF, false));

            LOG_INFO("✓ Health pack created at (", posX, ", ", posY, ")");
            return healthPack;
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create health pack: ", e.what());
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

    int PrefabFactory::_enemyTypeFromString(const std::string &enemyType) {
        if (enemyType == "basic")
            return 0;
        if (enemyType == "advanced" || enemyType == "heavy")
            return 1;
        if (enemyType == "fast")
            return 2;
        if (enemyType == "boss")
            return 3;
        LOG_WARNING("Unknown enemy type '", enemyType, "', defaulting to basic");
        return 0;
    }

}  // namespace ecs
