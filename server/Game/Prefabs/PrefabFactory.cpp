/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** PrefabFactory.cpp
*/

#include "server/Game/Prefabs/PrefabFactory.hpp"

namespace server {

    ecs::Address PrefabFactory::createPlayer(ecs::Registry &registry, uint32_t playerId) {
        try {
            ecs::Address player = registry.newEntity();

            registry.setComponent(player, ecs::Player(0, 3, playerId));
            registry.setComponent(player, ecs::Transform(100.0F, 300.0F));
            registry.setComponent(player, ecs::Velocity(0.0F, 0.0F, 200.0F));
            registry.setComponent(player, ecs::Health(100, 100));
            registry.setComponent(player, ecs::Collider(50.0F, 50.0F, 0.0F, 0.0F, 1, 0xFFFFFFFF, false));
            registry.setComponent(
                player, ecs::Weapon(10.0F, 0.0F, 0, 25));  // fire rate: 10 shots/sec, type 0, 25 damage
            LOG_INFO("✓ Player created with ID: ", playerId);

            return player;
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create player: ", e.what());
            return 0;
        }
    }

    ecs::Address PrefabFactory::createEnemy(ecs::Registry &registry, int enemyType, float posX, float posY) {
        try {
            EnemySpawnData spawnData = _getEnemySpawnData(enemyType);
            ecs::Address enemy = registry.newEntity();
            registry.setComponent(enemy, ecs::Enemy(enemyType, spawnData.scoreValue));
            registry.setComponent(enemy, ecs::Transform(posX, posY));
            registry.setComponent(enemy, ecs::Velocity(-spawnData.speed, 0.0f, 0.0f));
            registry.setComponent(enemy, ecs::Health(spawnData.health, spawnData.health));
            registry.setComponent(enemy, ecs::Collider(spawnData.colliderWidth, spawnData.colliderHeight,
                                                       0.0f, 0.0f, 2, 0xFFFFFFFF, false));
            registry.setComponent(enemy, ecs::Weapon(3.0f, 0.0f, 1, 15));  // 3 shots/sec, less damage
            LOG_INFO("✓ Enemy created of type: ", enemyType);
            return enemy;
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create enemy: ", e.what());
            return 0;
        }
    }

    ecs::Address PrefabFactory::createProjectile(ecs::Registry &registry, uint32_t ownerId, float posX,
                                                 float posY, float dirX, float dirY, float speed, int damage,
                                                 bool friendly) {
        try {
            ecs::Address projectile = registry.newEntity();

            registry.setComponent(projectile, ecs::Projectile(damage, 10.0f, ownerId, friendly));
            registry.setComponent(projectile, ecs::Transform(posX, posY));
            registry.setComponent(projectile, ecs::Velocity(dirX, dirY, speed));
            registry.setComponent(projectile, ecs::Collider(10.0f, 10.0f, 0.0f, 0.0f, 4, 0xFFFFFFFF, true));

            return projectile;
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

}  // namespace server
