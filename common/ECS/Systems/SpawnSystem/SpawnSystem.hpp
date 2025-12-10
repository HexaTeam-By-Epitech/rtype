/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** SpawnSystem
*/

#pragma once

#include "../ISystem.hpp"

namespace ecs {
    /**
     * @class SpawnSystem
     * @brief System managing entity spawning and wave generation.
     * 
     * Handles enemy waves, power-ups spawning, and entity creation based on
     * game progression and patterns.
     */
    class SpawnSystem : public ISystem {
       public:
        /**
         * @brief Constructs a SpawnSystem with default wave configuration.
         * 
         * Initializes with wave 1, 5 enemies per wave, and 2-second spawn interval.
         */
        SpawnSystem();

        /**
         * @brief Default destructor.
         */
        ~SpawnSystem() override = default;

        /**
         * @brief Manages entity spawning based on wave progression.
         * 
         * Controls the spawning of enemies at timed intervals.
         * Handles wave progression when all enemies are spawned.
         * 
         * @param registry Reference to the ECS registry
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        void update(Registry &registry, float deltaTime) override;

        /**
         * @brief Gets the component mask for this system.
         * 
         * @return ComponentMask of 0 (this system doesn't filter entities, it creates them)
         */
        ComponentMask getComponentMask() const override;

        /**
         * @brief Configures wave parameters.
         * 
         * Allows customization of spawn behavior for different wave configurations.
         * Resets spawn timer and enemy count.
         * 
         * @param waveNumber Current wave number
         * @param enemiesPerWave Number of enemies to spawn in this wave
         * @param spawnInterval Time in seconds between each enemy spawn
         */
        void setWaveConfig(int waveNumber, int enemiesPerWave, float spawnInterval);

       private:
        int _waveNumber;
        int _enemiesSpawned;
        int _enemiesPerWave;
        float _spawnTimer;
        float _spawnInterval;
    };
}
