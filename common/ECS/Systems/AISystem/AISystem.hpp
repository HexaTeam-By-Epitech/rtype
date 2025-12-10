/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** AISystem
*/

#pragma once

#include "../../Components/Enemy.hpp"
#include "../../Components/Transform.hpp"
#include "../../Components/Velocity.hpp"
#include "../ISystem.hpp"

namespace ecs {
    /**
     * @class AISystem
     * @brief System managing enemy AI behavior and attack patterns.
     * 
     * Controls enemy movement patterns and attack behaviors.
     * Requires Enemy, Transform and Velocity components.
     */
    class AISystem : public ISystem {
       public:
        /**
         * @brief Default constructor.
         */
        AISystem() = default;

        /**
         * @brief Default destructor.
         */
        ~AISystem() override = default;

        /**
         * @brief Updates all enemy AI behaviors.
         * 
         * Processes each enemy entity and applies movement patterns
         * based on their attack pattern type.
         * 
         * @param registry Reference to the ECS registry
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        void update(Registry &registry, float deltaTime) override;

        /**
         * @brief Gets the component mask for this system.
         * 
         * @return ComponentMask requiring Enemy, Transform and Velocity components
         */
        ComponentMask getComponentMask() const override;

       private:
        /**
         * @brief Applies movement pattern to an enemy entity.
         * 
         * @param enemy Enemy component containing pattern information
         * @param transform Transform component for position data
         * @param velocity Velocity component to modify
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        void applyMovementPattern(const Enemy &enemy, Transform &transform, Velocity &velocity,
                                  float deltaTime);
    };
}
