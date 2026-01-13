/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** PrefabFactory.hpp
*/

#pragma once

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
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

namespace ecs::wrapper {
    class ECSWorld;
}

namespace ecs {
    typedef uint32_t Address;
}  // namespace ecs

namespace server {

    /**
     * @class PrefabFactory
     * @brief Factory for creating game entity prefabs
     *
     * Used to instantiate entities with predefined components and settings.
     */
    class PrefabFactory {
       public:
        /**
         * @brief Create a player entity
         * @param world ECS world wrapper
         * @param playerId Unique player ID
         * @param playerName Display name
         * @return Entity address or 0 if failed
         */
        static ecs::Address createPlayer(ecs::Registry &registry, uint32_t playerId);

        /**
         * @brief Create an enemy entity
         * @param registry ECS registry
         * @param enemyType Type of enemy (0=basic, 1=heavy, 2=fast, etc.)
         * @param posX Starting X position
         * @param posY Starting Y position
         * @return Entity address or 0 if failed
         */
        static ecs::Address createEnemy(ecs::Registry &registry, int enemyType, float posX, float posY);

        /**
         * @brief Create a projectile entity
         * @param registry ECS registry
         * @param ownerId Owner entity ID (shooter)
         * @param posX Starting X position
         * @param posY Starting Y position
         * @param dirX Direction X component
         * @param dirY Direction Y component
         * @param speed Movement speed
         * @param damage Damage value
         * @param friendly True for player projectile, false for enemy
         * @return Entity address or 0 if failed
         */
        static ecs::Address createProjectile(ecs::Registry &registry, uint32_t ownerId, float posX,
                                             float posY, float dirX, float dirY, float speed, int damage,
                                             bool friendly);

       private:
        struct EnemySpawnData {
            float speed;
            int health;
            int scoreValue;
            float colliderWidth;
            float colliderHeight;
        };

        static EnemySpawnData _getEnemySpawnData(int enemyType);
    };

}  // namespace server
