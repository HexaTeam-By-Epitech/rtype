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
#include "common/Animation/AnimationDatabase.hpp"
#include "common/ECS/Components/Animation.hpp"
#include "common/ECS/Components/AnimationSet.hpp"
#include "common/ECS/Components/Buff.hpp"
#include "common/ECS/Components/Collider.hpp"
#include "common/ECS/Components/Enemy.hpp"
#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/Player.hpp"
#include "common/ECS/Components/Projectile.hpp"
#include "common/ECS/Components/Sprite.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Components/Velocity.hpp"
#include "common/ECS/Components/Weapon.hpp"
#include "common/ECS/Registry.hpp"
#include "common/Logger/Logger.hpp"

// Forward declarations
namespace ecs::wrapper {
    class ECSWorld;
}

namespace ecs {
    using Address = std::uint32_t;

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
         * @brief Create an enemy entity with custom parameters (for SpawnSystem)
         * @param registry ECS registry
         * @param enemyType Type string ("basic", "advanced", "fast", "boss")
         * @param posX Starting X position
         * @param posY Starting Y position
         * @param health Custom health value
         * @param scoreValue Custom score value
         * @param scriptPath Optional Lua script path for AI behavior
         * @return Entity address or 0 if failed
         */
        static ecs::Address createEnemy(ecs::Registry &registry, const std::string &enemyType, float posX,
                                        float posY, float health, int scoreValue,
                                        const std::string &scriptPath = "");

        /**
         * @brief Create an enemy entity directly from Registry (for SpawnSystem)
         * @param registry ECS registry
         * @param enemyType Type string ("basic", "advanced", "fast", "boss")
         * @param posX Starting X position
         * @param posY Starting Y position
         * @param health Custom health value
         * @param scoreValue Custom score value
         * @param scriptPath Optional Lua script path for AI behavior
         * @return Entity address or 0 if failed
         */
        static ecs::Address createEnemyFromRegistry(ecs::Registry &registry, const std::string &enemyType,
                                                    float posX, float posY, float health, int scoreValue,
                                                    const std::string &scriptPath = "");

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

        /**
         * @brief Create a collectible power-up entity
         * @param registry ECS registry
         * @param buffType Type of buff to grant
         * @param duration Duration of buff (0.0f for permanent)
         * @param value Buff value/multiplier
         * @param posX Starting X position
         * @param posY Starting Y position
         * @return Entity address or 0 if failed
         */
        static ecs::Address createPowerUp(ecs::Registry &registry, ecs::BuffType buffType, float duration,
                                          float value, float posX, float posY);

        /**
         * @brief Create a health pack collectible
         * @param registry ECS registry
         * @param healthRestore Amount of health to restore
         * @param posX Starting X position
         * @param posY Starting Y position
         * @return Entity address or 0 if failed
         */
        static ecs::Address createHealthPack(ecs::Registry &registry, int healthRestore, float posX,
                                             float posY);

       private:
        struct EnemySpawnData {
            float speed;
            int health;
            int scoreValue;
            float colliderWidth;
            float colliderHeight;
        };

        static EnemySpawnData _getEnemySpawnData(int enemyType);
        static int _enemyTypeFromString(const std::string &enemyType);
    };

}  // namespace ecs
