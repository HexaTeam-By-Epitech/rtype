/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** MovementSystem
*/

#ifndef MOVEMENTSYSTEM_HPP_
#define MOVEMENTSYSTEM_HPP_

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
        MovementSystem() = default;
        ~MovementSystem() override = default;
    };
}

#endif
