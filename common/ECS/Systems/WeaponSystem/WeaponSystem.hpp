/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** WeaponSystem
*/

#ifndef WEAPONSYSTEM_HPP_
#define WEAPONSYSTEM_HPP_

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
        WeaponSystem() = default;
        ~WeaponSystem() override = default;
    };
}

#endif
