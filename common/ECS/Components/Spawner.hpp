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

       private:
        std::vector<SpawnRequest> _spawnRequests;
    };

}  // namespace ecs
