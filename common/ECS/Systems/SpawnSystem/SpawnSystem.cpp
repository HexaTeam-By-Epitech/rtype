/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** SpawnSystem
*/

#include "SpawnSystem.hpp"

namespace ecs {
    SpawnSystem::SpawnSystem()
        : _waveNumber(1), _enemiesSpawned(0), _enemiesPerWave(5), _spawnTimer(0.0f), _spawnInterval(2.0f) {}

    void SpawnSystem::update([[maybe_unused]] Registry &registry, float deltaTime) {
        _spawnTimer += deltaTime;

        if (_spawnTimer >= _spawnInterval && _enemiesSpawned < _enemiesPerWave) {
            // TODO: Implement actual enemy entity creation with components

            _enemiesSpawned++;
            _spawnTimer = 0.0f;
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
        _spawnTimer = 0.0f;
    }

    ComponentMask SpawnSystem::getComponentMask() const {
        return 0;
    }
}  // namespace ecs
