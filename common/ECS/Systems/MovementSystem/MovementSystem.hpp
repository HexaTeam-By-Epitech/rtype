/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** MovementSystem
*/

#pragma once

#include "../../Components/IComponent.hpp"
#include "../../Components/Transform.hpp"
#include "../../Components/Velocity.hpp"
#include "../ISystem.hpp"

namespace ecs {
    /**
     * @class MovementSystem
     * @brief System handling entity movement based on velocity.
     * 
     * Updates entity positions by applying their velocity and speed.
     * Requires Transform and Velocity components.
     */
    class MovementSystem : public ISystem {
       public:
        /**
         * @brief Default constructor.
         */
        MovementSystem() = default;

        /**
         * @brief Default destructor.
         */
        ~MovementSystem() override = default;

        /**
         * @brief Updates entity positions based on their velocities.
         * 
         * Applies velocity vectors and speed to transform positions.
         * Movement is frame-rate independent using deltaTime.
         * 
         * @param registry Reference to the ECS registry
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        void update(Registry &registry, float deltaTime) override;

        /**
         * @brief Gets the component mask for this system.
         * 
         * @return ComponentMask requiring Transform and Velocity components
         */
        ComponentMask getComponentMask() const override;
    };
}
