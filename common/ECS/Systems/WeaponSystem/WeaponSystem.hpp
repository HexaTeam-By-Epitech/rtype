/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** WeaponSystem
*/

#ifndef WEAPONSYSTEM_HPP_
#define WEAPONSYSTEM_HPP_

#include "../../Components/Transform.hpp"
#include "../../Components/Weapon.hpp"
#include "../ISystem.hpp"

namespace ecs {
    /**
     * @class WeaponSystem
     * @brief System managing weapon firing and cooldowns.
     * 
     * Handles weapon cooldown timers and projectile spawning.
     * Requires Weapon and Transform components.
     */
    class WeaponSystem : public ISystem {
       public:
        /**
         * @brief Default constructor.
         */
        WeaponSystem() = default;

        /**
         * @brief Default destructor.
         */
        ~WeaponSystem() override = default;

        /**
         * @brief Manages weapon cooldowns for all entities.
         * 
         * Decrements weapon cooldown timers each frame.
         * When cooldown reaches 0, the weapon is ready to fire.
         * Actual projectile creation is handled by input or AI systems.
         * 
         * @param registry Reference to the ECS registry
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        void update(Registry &registry, float deltaTime) override;

        /**
         * @brief Gets the component mask for this system.
         * 
         * @return ComponentMask requiring Weapon and Transform components
         */
        ComponentMask getComponentMask() const override;
    };
}

#endif
