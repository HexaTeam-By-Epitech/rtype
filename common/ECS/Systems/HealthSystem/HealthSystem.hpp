/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** HealthSystem
*/

#pragma once

#include "../../Components/Health.hpp"
#include "../../Components/IComponent.hpp"
#include "../ISystem.hpp"

namespace ecs {
    /**
     * @class HealthSystem
     * @brief System managing entity health, invincibility and death.
     * 
     * Decrements invincibility timers, handles entity destruction when health reaches zero,
     * and manages health-related game logic.
     * Requires Health component.
     */
    class HealthSystem : public ISystem {
       public:
        /**
         * @brief Default constructor.
         */
        HealthSystem() = default;

        /**
         * @brief Default destructor.
         */
        ~HealthSystem() override = default;

        /**
         * @brief Updates health-related logic for all entities.
         * 
         * Performs the following operations:
         * - Decrements invincibility timers
         * - Removes invincibility when timer expires
         * - Destroys entities with zero or negative health
         * 
         * @param registry Reference to the ECS registry
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        void update(Registry &registry, float deltaTime) override;

        /**
         * @brief Gets the component mask for this system.
         * 
         * @return ComponentMask requiring Health component
         */
        ComponentMask getComponentMask() const override;
    };
}
