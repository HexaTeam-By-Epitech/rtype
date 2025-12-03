/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** ProjectileSystem
*/

#ifndef PROJECTILESYSTEM_HPP_
#define PROJECTILESYSTEM_HPP_

#include "../ISystem.hpp"

namespace ecs {
    /**
     * @class ProjectileSystem
     * @brief System managing projectile lifetime and destruction.
     * 
     * Decrements projectile lifetime and destroys expired projectiles.
     * Requires Projectile component.
     */
    class ProjectileSystem : public ISystem {
       public:
        ProjectileSystem() = default;
        ~ProjectileSystem() override = default;
    };
}

#endif
