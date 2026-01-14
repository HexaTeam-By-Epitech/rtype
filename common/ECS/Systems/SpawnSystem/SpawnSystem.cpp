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
#include "common/ECS/Prefabs/PrefabFactory.hpp"
#include "common/Logger/Logger.hpp"

namespace ecs {
    SpawnSystem::SpawnSystem()
        : _waveNumber(1), _enemiesSpawned(0), _enemiesPerWave(5), _spawnTimer(0.0f), _spawnInterval(2.0f) {}

    void SpawnSystem::update(Registry &registry, float deltaTime) {
        // Process spawn requests from Spawner components
        auto spawners = registry.view<Spawner>();

        for (auto entity : spawners) {
            Spawner &spawner = registry.getComponent<Spawner>(entity);

            if (!spawner.hasPendingSpawns()) {
                continue;
            }

            const auto &requests = spawner.getSpawnRequests();
            for (const auto &request : requests) {
                _spawnEnemy(registry, request);
            }

            spawner.clearSpawnRequests();
        }

        // Legacy wave-based spawning (optional)
        _spawnTimer += deltaTime;

        if (_spawnTimer >= _spawnInterval && _enemiesSpawned < _enemiesPerWave) {
            // Spawn using legacy system if needed
            _spawnTimer = 0.0f;
            _enemiesSpawned++;
        }

        if (_enemiesSpawned >= _enemiesPerWave) {
            _waveNumber++;
            _enemiesSpawned = 0;
            _enemiesPerWave += 2;
        }
    }

    void SpawnSystem::_spawnEnemy(Registry &registry, const SpawnRequest &request) {
        try {
            Address enemy = registry.newEntity();

            registry.setComponent<Transform>(enemy, Transform(request.x, request.y));
            registry.setComponent<Velocity>(enemy, Velocity(-1.0f, 0.0f, 200.0f));
            registry.setComponent<Health>(
                enemy, Health(static_cast<int>(request.health), static_cast<int>(request.health)));
            registry.setComponent<Enemy>(enemy, Enemy(0, request.scoreValue));

            if (!request.scriptPath.empty()) {
                registry.setComponent<LuaScript>(enemy, LuaScript(request.scriptPath));
            }

            LOG_INFO("[SpawnSystem] Spawned ", request.enemyType, " at (", request.x, ", ", request.y, ")");
        } catch (const std::exception &e) {
            LOG_ERROR("[SpawnSystem] Failed to spawn enemy: ", e.what());
        }
    }

    /**
     * @brief Configures the spawn system for a specific wave.
     */
    void SpawnSystem::setWaveConfig(int waveNumber, int enemiesPerWave, float spawnInterval) {
        _waveNumber = waveNumber;
        _enemiesPerWave = enemiesPerWave;
        _spawnInterval = spawnInterval;
        _enemiesSpawned = 0;
        _spawnTimer = 0.0f;
    }

    ComponentMask SpawnSystem::getComponentMask() const {
        return 0;
    }
}  // namespace ecs
