/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** AISystem
*/

#ifndef AISYSTEM_HPP_
#define AISYSTEM_HPP_

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
        AISystem() = default;
        ~AISystem() override = default;
    };
}

#endif
