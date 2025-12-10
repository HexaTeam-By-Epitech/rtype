/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** ProjectileSystem
*/

#pragma once

#include "../../Components/IComponent.hpp"
#include "../../Components/Projectile.hpp"
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
        /**
         * @brief Default constructor.
         */
        ProjectileSystem() = default;

        /**
         * @brief Default destructor.
         */
        ~ProjectileSystem() override = default;

        /**
         * @brief Manages projectile lifetime and destruction.
         * 
         * Decrements the lifetime counter for each projectile.
         * Destroys projectiles when their lifetime reaches zero.
         * 
         * @param registry Reference to the ECS registry
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        void update(Registry &registry, float deltaTime) override;

        /**
         * @brief Gets the component mask for this system.
         * 
         * @return ComponentMask requiring Projectile component
         */
        ComponentMask getComponentMask() const override;
    };
}
