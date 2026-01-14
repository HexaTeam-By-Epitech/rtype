/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** SpawnSystem
*/

#pragma once

#include "../ISystem.hpp"

namespace ecs {
    struct SpawnRequest;

    /**
     * @class SpawnSystem
     * @brief System managing entity spawning and wave generation.
     * 
     * Handles enemy waves, power-ups spawning, and entity creation based on
     * game progression and patterns.
     * 
     * Processes SpawnRequest queues from Spawner components—the clean ECS way
     * to handle dynamic spawning without tight coupling.
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
         * Processes spawn requests from Spawner components.
         * Handles the actual entity creation with all components.
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

       private:
        /**
         * @brief Internal method to spawn an enemy from a request
         * @param registry Reference to the ECS registry
         * @param request The spawn request with all parameters
         */
        void _spawnEnemy(Registry &registry, const SpawnRequest &request);
    };
}  // namespace ecs
