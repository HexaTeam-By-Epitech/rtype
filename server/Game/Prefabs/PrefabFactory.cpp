/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** PrefabFactory.cpp
*/

#include "server/Game/Prefabs/PrefabFactory.hpp"
#include <stdexcept>
#include "common/ECS/Components/Collider.hpp"
#include "common/ECS/Components/Enemy.hpp"
#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/Player.hpp"
#include "common/ECS/Components/Projectile.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Components/Velocity.hpp"
#include "common/ECS/Components/Weapon.hpp"
#include "common/ECS/Registry.hpp"
#include "common/Logger/Logger.hpp"

namespace server {

    ecs::Address PrefabFactory::createPlayer(ecs::Registry &registry, uint32_t playerId,
                                             const std::string &playerName) {
        try {
            ecs::Address player = registry.newEntity();

            // Player component
            registry.setComponent(player, ecs::Player(0, 3, playerId));  // score=0, lives=3

            // Transform at spawn position
            registry.setComponent(player, ecs::Transform(50.0f, 300.0f));

            // Velocity for movement
            registry.setComponent(player, ecs::Velocity(0.0f, 0.0f, 200.0f));  // 200 units/sec max speed

            // Health
            registry.setComponent(player, ecs::Health(100, 100));

            // Collision box (50x50)
            registry.setComponent(player, ecs::Collider(50.0f, 50.0f, 0.0f, 0.0f, 1, 0xFFFFFFFF, false));

            // Weapon (fire rate: 10 shots/sec, type 0, 25 damage)
            registry.setComponent(player, ecs::Weapon(10.0f, 0.0f, 0, 25));

            LOG_INFO("✓ Player created: ", playerName, " (ID: ", playerId, ")");
            return player;
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create player: ", e.what());
            return 0;
        }
    }

    ecs::Address PrefabFactory::createEnemy(ecs::Registry &registry, int enemyType, float posX, float posY) {
        try {
            auto spawnData = _getEnemySpawnData(enemyType);

            ecs::Address enemy = registry.newEntity();

            // Enemy component
            registry.setComponent(enemy, ecs::Enemy(enemyType, spawnData.scoreValue, 0));

            // Transform
            registry.setComponent(enemy, ecs::Transform(posX, posY));

            // Velocity (moving left)
            registry.setComponent(enemy, ecs::Velocity(-1.0f, 0.0f, spawnData.speed));

            // Health
            registry.setComponent(enemy, ecs::Health(spawnData.health, spawnData.health));

            // Collider
            registry.setComponent(enemy, ecs::Collider(spawnData.colliderWidth, spawnData.colliderHeight,
                                                       0.0f, 0.0f, 2, 0xFFFFFFFF, false));

            // Weapon (enemies can shoot)
            registry.setComponent(enemy, ecs::Weapon(3.0f, 0.0f, 1, 15));  // 3 shots/sec, less damage

            LOG_INFO("✓ Enemy spawned: Type ", enemyType, " at (", posX, ", ", posY, ")");
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

            // Projectile component
            registry.setComponent(projectile, ecs::Projectile(damage, 10.0f, ownerId, friendly));

            // Transform
            registry.setComponent(projectile, ecs::Transform(posX, posY));

            // Velocity
            registry.setComponent(projectile, ecs::Velocity(dirX, dirY, speed));

            // Small collision box for hit detection
            registry.setComponent(projectile, ecs::Collider(10.0f, 10.0f, 0.0f, 0.0f, 4, 0xFFFFFFFF, true));

            return projectile;
        } catch (const std::exception &e) {
            std::cerr << "[PrefabFactory] Failed to create projectile: " << e.what() << std::endl;
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
