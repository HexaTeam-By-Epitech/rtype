/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Spawner component for declarative entity spawning
*/

#pragma once

#include <string>
#include <vector>
#include "IComponent.hpp"

namespace ecs {
    /**
     * @struct SpawnRequest
     * @brief Declarative request for spawning an entity
     */
    struct SpawnRequest {
        float x;
        float y;
        std::string enemyType;   // "basic", "advanced", etc.
        std::string scriptPath;  // Lua script for behavior
        float health;
        int scoreValue;
    };

    struct WaveConfig {
        std::vector<SpawnRequest> enemies;
        float spawnInterval;  // Time between spawns in this wave
    };

    struct SpawnerConfig {
        std::vector<WaveConfig> waves;
        std::vector<int> wavesIntervals;  // Time between waves
    };

    /**
     * @class Spawner
     * @brief Component that holds spawn requests to be processed by SpawnSystem
     * 
     * Entities with this component act as spawners. They queue up spawn requests
     * that the SpawnSystem will process each frame. This is the clean ECS way to
     * handle dynamic spawning without direct entity creation in Lua.
     */
    class Spawner : public IComponent {
       public:
        Spawner() = default;
        ~Spawner() override = default;

        /**
         * @brief Queue a spawn request
         * @param request The spawn request to queue
         */
        void queueSpawn(const SpawnRequest &request) { _spawnRequests.push_back(request); }

        /**
         * @brief Get all pending spawn requests
         * @return Vector of spawn requests
         */
        const std::vector<SpawnRequest> &getSpawnRequests() const { return _spawnRequests; }

        /**
         * @brief Clear all pending spawn requests (called after processing)
         */
        void clearSpawnRequests() { _spawnRequests.clear(); }

        /**
         * @brief Check if there are pending spawns
         * @return true if spawn requests exist
         */
        bool hasPendingSpawns() const { return !_spawnRequests.empty(); }

        /**
         * @brief Get component type ID
         * @return ComponentType
         */
        ComponentType getType() const override { return getComponentType<Spawner>(); }

        void setConfig(const SpawnerConfig &config) { _config = config; }
        const SpawnerConfig &getConfig() const { return _config; }

        int spawnerTime = 0;
        int spawnerTicks = 0;
        int currentWaveIndex = 0;
        int lastTimeRan = 0;

        bool isActive = true;

       private:
        std::vector<SpawnRequest> _spawnRequests;
        SpawnerConfig _config = {};
    };

}  // namespace ecs
