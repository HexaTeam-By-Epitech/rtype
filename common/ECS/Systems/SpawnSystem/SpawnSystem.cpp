/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** SpawnSystem
*/

#include "SpawnSystem.hpp"
#include "common/ECS/Components/Spawner.hpp"
#include "common/Logger/Logger.hpp"
#include "server/Game/Prefabs/PrefabFactory.hpp"

namespace ecs {
    SpawnSystem::SpawnSystem() {}

    void SpawnSystem::update(Registry &registry, float deltaTime) {
        auto spawners = registry.view<Spawner>();

        // Process wave timing and spawning logic
        for (auto spawner : spawners) {
            Spawner &spawnerComp = registry.getComponent<Spawner>(spawner);

            if (spawnerComp.getConfig().waves.empty() || !spawnerComp.isActive) {
                continue;  // No waves configured or spawner inactive
            }

            SpawnerConfig &config = spawnerComp.getConfigMutable();
            int wavesCount = static_cast<int>(config.waves.size());

            // Check if wavesIntervals is properly configured
            if (config.wavesIntervals.empty() ||
                spawnerComp.currentWaveIndex >= static_cast<int>(config.wavesIntervals.size())) {
                LOG_WARNING("[SpawnSystem] Invalid wave configuration - missing intervals");
                continue;
            }

            if (spawnerComp.currentWaveIndex >= wavesCount) {
                spawnerComp.isActive = false;
                continue;
            }

            // Get current wave
            WaveConfig &currentWave = config.waves[spawnerComp.currentWaveIndex];
            float waveInterval = static_cast<float>(config.wavesIntervals[spawnerComp.currentWaveIndex]);

            // Update wave elapsed time
            spawnerComp.waveElapsedTime += deltaTime;

            // Spawn enemies FIRST based on their individual delay within the wave
            for (auto &enemy : currentWave.enemies) {
                if (!enemy.hasSpawned && spawnerComp.waveElapsedTime >= enemy.spawnDelay) {
                    _spawnEnemy(registry, enemy);
                    enemy.hasSpawned = true;
                }
            }

            // THEN check if it's time to advance to next wave
            if (spawnerComp.waveElapsedTime >= waveInterval) {
                // Check if all enemies in current wave have spawned
                bool allSpawned = true;
                for (const auto &enemy : currentWave.enemies) {
                    if (!enemy.hasSpawned) {
                        allSpawned = false;
                        break;
                    }
                }

                if (allSpawned) {
                    spawnerComp.currentWaveIndex++;
                    spawnerComp.waveElapsedTime = 0.0f;
                    LOG_INFO("[SpawnSystem] Moving to wave ", spawnerComp.currentWaveIndex + 1);

                    if (spawnerComp.currentWaveIndex >= wavesCount) {
                        spawnerComp.isActive = false;
                        LOG_INFO("[SpawnSystem] All waves completed for this spawner.");
                    } else {
                        // Reset hasSpawned flags for the next wave
                        WaveConfig &nextWave = config.waves[spawnerComp.currentWaveIndex];
                        for (auto &enemy : nextWave.enemies) {
                            enemy.hasSpawned = false;
                        }
                        LOG_INFO("[SpawnSystem] Reset spawn flags for wave ",
                                 spawnerComp.currentWaveIndex + 1);
                    }
                }
            }
        }
    }

    void SpawnSystem::_spawnEnemy(Registry &registry, const SpawnRequest &request) {
        ecs::Address enemy = server::PrefabFactory::createEnemyFromRegistry(
            registry, request.enemyType, request.x, request.y, request.health, request.scoreValue,
            request.scriptPath);

        if (enemy == 0) {
            LOG_ERROR("[SpawnSystem] Failed to spawn enemy via PrefabFactory");
        }
    }

    ComponentMask SpawnSystem::getComponentMask() const {
        return 0;
    }
}  // namespace ecs
