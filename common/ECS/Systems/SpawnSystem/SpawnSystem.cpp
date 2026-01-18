/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** SpawnSystem
*/

#include "SpawnSystem.hpp"
#include "common/ECS/Components/Collider.hpp"
#include "common/ECS/Components/Enemy.hpp"
#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/LuaScript.hpp"
#include "common/ECS/Components/Spawner.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Components/Velocity.hpp"
#include "common/ECS/Components/Weapon.hpp"
#include "common/ECS/Prefabs/PrefabFactory.hpp"
#include "common/Logger/Logger.hpp"

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
        try {
            Address enemy = registry.newEntity();

            // Map enemy type string to numeric type
            int enemyType = 0;  // Default: basic
            float speed = 150.0f;
            float colliderWidth = 40.0f;
            float colliderHeight = 40.0f;

            if (request.enemyType == "basic") {
                enemyType = 0;
                speed = 150.0f;
                colliderWidth = 40.0f;
                colliderHeight = 40.0f;
            } else if (request.enemyType == "advanced" || request.enemyType == "heavy") {
                enemyType = 1;
                speed = 100.0f;
                colliderWidth = 60.0f;
                colliderHeight = 60.0f;
            } else if (request.enemyType == "fast") {
                enemyType = 2;
                speed = 200.0f;
                colliderWidth = 30.0f;
                colliderHeight = 30.0f;
            } else if (request.enemyType == "boss") {
                enemyType = 3;
                speed = 120.0f;
                colliderWidth = 80.0f;
                colliderHeight = 80.0f;
            }

            registry.setComponent<Transform>(enemy, Transform(request.x, request.y));
            registry.setComponent<Velocity>(enemy, Velocity(-1.0f, 0.0f, speed));
            registry.setComponent<Health>(
                enemy, Health(static_cast<int>(request.health), static_cast<int>(request.health)));
            registry.setComponent<Enemy>(enemy, Enemy(enemyType, request.scoreValue));
            registry.setComponent<Collider>(
                enemy, Collider(colliderWidth, colliderHeight, 0.0f, 0.0f, 2, 0xFFFFFFFF, false));
            registry.setComponent<Weapon>(enemy,
                                          Weapon(3.0f, 0.0f, 1, 15));  // 3 shots/sec, type 1, 15 damage

            if (!request.scriptPath.empty()) {
                registry.setComponent<LuaScript>(enemy, LuaScript(request.scriptPath));
            }

            LOG_INFO("[SpawnSystem] Spawned ", request.enemyType, " (type ", enemyType, ") at (", request.x,
                     ", ", request.y, ") with entity ID: ", enemy);
        } catch (const std::exception &e) {
            LOG_ERROR("[SpawnSystem] Failed to spawn enemy: ", e.what());
        }
    }

    ComponentMask SpawnSystem::getComponentMask() const {
        return 0;
    }
}  // namespace ecs
