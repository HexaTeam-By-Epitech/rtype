/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** PrefabFactory.hpp
*/

#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace ecs::wrapper {
    class ECSWorld;
}

namespace ecs {
    typedef uint32_t Address;
    class Registry;
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
        static ecs::Address createPlayer(ecs::wrapper::ECSWorld &world, uint32_t playerId,
                                         const std::string &playerName);

        /**
         * @brief Create an enemy entity
         * @param world ECS world wrapper
         * @param enemyType Type of enemy (0=basic, 1=heavy, 2=fast, etc.)
         * @param posX Starting X position
         * @param posY Starting Y position
         * @return Entity address or 0 if failed
         */
        static ecs::Address createEnemy(ecs::wrapper::ECSWorld &world, int enemyType, float posX, float posY);

        /**
         * @brief Create an enemy entity with custom parameters (for SpawnSystem)
         * @param world ECS world wrapper
         * @param enemyType Type string ("basic", "advanced", "fast", "boss")
         * @param posX Starting X position
         * @param posY Starting Y position
         * @param health Custom health value
         * @param scoreValue Custom score value
         * @param scriptPath Optional Lua script path for AI behavior
         * @return Entity address or 0 if failed
         */
        static ecs::Address createEnemy(ecs::wrapper::ECSWorld &world, const std::string &enemyType,
                                        float posX, float posY, float health, int scoreValue,
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
         * @param world ECS world wrapper
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
        static ecs::Address createProjectile(ecs::wrapper::ECSWorld &world, uint32_t ownerId, float posX,
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
        static int _enemyTypeFromString(const std::string &enemyType);
    };

}  // namespace server
