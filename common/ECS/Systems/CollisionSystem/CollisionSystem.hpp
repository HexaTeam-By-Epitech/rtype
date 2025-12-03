/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** CollisionSystem
*/

#ifndef COLLISIONSYSTEM_HPP_
#define COLLISIONSYSTEM_HPP_

#include "../ISystem.hpp"

namespace ecs {
    /**
     * @class CollisionSystem
     * @brief System handling collision detection between entities.
     * 
     * Detects AABB collisions and manages layer-based filtering.
     * Requires Transform and Collider components.
     */
    class CollisionSystem : public ISystem {
       public:
        CollisionSystem() = default;
        ~CollisionSystem() override = default;
    };
}

#endif
