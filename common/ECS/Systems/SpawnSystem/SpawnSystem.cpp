/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** SpawnSystem
*/

#include "SpawnSystem.hpp"
#include "common/ECS/Components/Enemy.hpp"
#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/LuaScript.hpp"
#include "common/ECS/Components/Spawner.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Components/Velocity.hpp"
#include "common/Logger/Logger.hpp"
#include "server/Game/Prefabs/PrefabFactory.hpp"

namespace ecs {
    SpawnSystem::SpawnSystem() {}

    void SpawnSystem::update(Registry &registry, float deltaTime) {
        LOG_DEBUG("[SpawnSystem] Update function called with deltaTime: ", deltaTime);
        auto spawners = registry.view<Spawner>();

        // Process wave timing and spawning logic
        for (auto spawner : spawners) {
            Spawner &spawnerComp = registry.getComponent<Spawner>(spawner);
            // handle the case there is not config set -- ""SpawnerConfig _config = {};"" in the relevant component
            if (spawnerComp.getConfig().waves.empty()) {
                continue;
            }

            SpawnerConfig const &config = spawnerComp.getConfig();
            int wavesCount = static_cast<int>(config.waves.size());

            if (config.waves.empty() || !spawnerComp.isActive) {
                continue;  // No waves configured or spawner inactive
                // auto-inactivate when a config is finished
            }

            // Check if wavesIntervals is properly configured
            if (config.wavesIntervals.empty() ||
                spawnerComp.currentWaveIndex >= static_cast<int>(config.wavesIntervals.size())) {
                LOG_WARNING("[SpawnSystem] Invalid wave configuration - missing intervals");
                continue;
            }

            // check if we need to move to the next wave, using wave intervals
            if (spawnerComp.currentWaveIndex < wavesCount) {
                int waveInterval = config.wavesIntervals[spawnerComp.currentWaveIndex];

                if (spawnerComp.spawnerTime >= waveInterval) {
                    spawnerComp.spawnerTime = 0;
                    spawnerComp.currentWaveIndex++;
                    LOG_INFO("[SpawnSystem] Moving to wave ", spawnerComp.currentWaveIndex);

                    if (spawnerComp.currentWaveIndex >= wavesCount) {
                        spawnerComp.isActive = false;  // No more waves
                        LOG_INFO("[SpawnSystem] All waves completed for this spawner.");
                    }
                }
            }

            // Spawn enemies for the current wave (only once when wave starts)
            if (spawnerComp.spawnerTime == 0 && spawnerComp.spawnerTicks == 0 &&
                spawnerComp.currentWaveIndex < wavesCount) {
                const WaveConfig &currentWave = config.waves[spawnerComp.currentWaveIndex];
                for (const auto &request : currentWave.enemies) {
                    spawnerComp.queueSpawn(request);
                }
                LOG_INFO("[SpawnSystem] Queued ", currentWave.enemies.size(), " enemies for wave ",
                         spawnerComp.currentWaveIndex + 1);
            }

            // Update spawner time
            spawnerComp.spawnerTicks += 1;
            if (spawnerComp.spawnerTicks >= static_cast<int>(1.0f / deltaTime)) {
                spawnerComp.spawnerTime += 1;
                spawnerComp.spawnerTicks = 0;
            }
        }

        // Process spawn requests from Spawner components
        for (auto entity : spawners) {
            Spawner &spawner = registry.getComponent<Spawner>(entity);

            if (spawner.lastTimeRan == spawner.spawnerTime) {
                continue;  // Already processed this time tick
            }
            spawner.lastTimeRan = spawner.spawnerTime;

            if (!spawner.hasPendingSpawns()) {
                continue;
            }

            const auto &requests = spawner.getSpawnRequests();
            for (const auto &request : requests) {
                _spawnEnemy(registry, request);
            }

            spawner.clearSpawnRequests();
        }
    }

    void SpawnSystem::_spawnEnemy(Registry &registry, const SpawnRequest &request) {
        try {
            Address enemy = registry.newEntity();

            registry.setComponent<Transform>(enemy, Transform(request.x, request.y));
            registry.setComponent<Velocity>(enemy, Velocity(-1.0f, 0.0f, 200.0f));
            registry.setComponent<Health>(
                enemy, Health(static_cast<int>(request.health), static_cast<int>(request.health)));
            registry.setComponent<Enemy>(enemy, Enemy(0, request.scoreValue, 0));

            if (!request.scriptPath.empty()) {
                registry.setComponent<LuaScript>(enemy, LuaScript(request.scriptPath));
            }

            LOG_INFO("[SpawnSystem] Spawned ", request.enemyType, " at (", request.x, ", ", request.y, ")");
        } catch (const std::exception &e) {
            LOG_ERROR("[SpawnSystem] Failed to spawn enemy: ", e.what());
        }
    }

    ComponentMask SpawnSystem::getComponentMask() const {
        return 0;
    }
}  // namespace ecs
