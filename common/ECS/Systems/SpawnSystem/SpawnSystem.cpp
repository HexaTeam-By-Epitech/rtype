/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** SpawnSystem
*/

#include "SpawnSystem.hpp"
#include "../../../Logger/Logger.hpp"
#include "../../Components/Collider.hpp"
#include "../../Components/Enemy.hpp"
#include "../../Components/Health.hpp"
#include "../../Components/Transform.hpp"
#include "../../Components/Velocity.hpp"

namespace ecs {
    SpawnSystem::SpawnSystem()
        : _waveNumber(1), _enemiesSpawned(0), _enemiesPerWave(5), _spawnTimer(0.0), _spawnInterval(2.0) {}

    void SpawnSystem::update(Registry &registry, float deltaTime) {
        _spawnTimer += deltaTime;

        if (_spawnTimer >= _spawnInterval && _enemiesSpawned < _enemiesPerWave) {
            try {
                // Create enemy entity
                Address enemy = registry.newEntity();

                // Simple wave-based parameters
                int enemyType = 0;                // 0 = basic
                int scoreValue = 100;             // points
                int pattern = (_waveNumber % 3);  // vary movement pattern
                int maxHealth = 20 + (_waveNumber * 5);
                float speed = static_cast<float>(100.0 + (_waveNumber * 10.0));

                // Random Y spawn within screen bounds
                float spawnX = 1920.0 + 50.0;  // just outside right edge
                float spawnY = static_cast<float>(100.0 + ((_enemiesSpawned % 10) * 80.0));

                // Attach components
                registry.setComponent<Enemy>(enemy, Enemy(enemyType, scoreValue, pattern));
                registry.setComponent<Transform>(enemy, Transform(spawnX, spawnY));
                registry.setComponent<Velocity>(enemy, Velocity(-1.0, 0.0, speed));
                registry.setComponent<Health>(enemy, Health(maxHealth));
                registry.setComponent<Collider>(enemy, Collider(50.0, 50.0, 0.0, 0.0, 2, 0xFFFFFFFF, false));

                // Increment counters and reset timer
                _enemiesSpawned++;
                _spawnTimer = 0.0;
            } catch (const std::exception &e) {
                // If component registration limits or errors occur, skip this spawn
                LOG_WARNING("SpawnSystem: Failed to spawn enemy: ", e.what());
                _spawnTimer = 0.0;
            }
        }

        if (_enemiesSpawned >= _enemiesPerWave) {
            _waveNumber++;
            _enemiesSpawned = 0;
            _enemiesPerWave += 2;
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
        _spawnTimer = 0.0;
    }

    ComponentMask SpawnSystem::getComponentMask() const {
        return 0;
    }
}  // namespace ecs
