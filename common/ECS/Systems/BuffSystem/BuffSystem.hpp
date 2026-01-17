/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** BuffSystem - Manages buff timers and effects
*/

#pragma once

#include "common/ECS/Components/Buff.hpp"
#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/Velocity.hpp"
#include "common/ECS/Components/Weapon.hpp"
#include "common/ECS/Registry.hpp"
#include "common/ECS/Systems/ISystem.hpp"

namespace ecs {

    /**
     * @class BuffSystem
     * @brief System managing buff timers and applying buff effects
     * 
     * Updates buff durations, applies stat modifications, and removes expired buffs.
     * Handles both temporary and permanent buffs with different behaviors.
     */
    class BuffSystem : public ISystem {
       public:
        /**
         * @brief Constructs a BuffSystem
         */
        BuffSystem() = default;

        /**
         * @brief Destructor
         */
        ~BuffSystem() override = default;

        /**
         * @brief Update buff timers and apply effects
         * 
         * Decrements buff durations, removes expired buffs, and applies
         * stat modifications to entities based on their active buffs.
         * 
         * @param registry Reference to the ECS registry
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        void update(Registry &registry, float deltaTime) override;

        /**
         * @brief Gets the component mask for this system
         * 
         * Processes entities with Buff component.
         * 
         * @return ComponentMask Bitmask for Buff component
         */
        ComponentMask getComponentMask() const override;

       private:
        /**
         * @brief Update buff timers and remove expired ones
         * @param buff Buff component to update
         * @param deltaTime Time elapsed
         */
        void _updateBuffTimers(Buff &buff, float deltaTime);

        /**
         * @brief Apply buff effects to entity stats
         * @param address Entity address
         * @param registry Registry reference
         * @param buff Buff component with active buffs
         */
        void _applyBuffEffects(Address address, Registry &registry, const Buff &buff);

        /**
         * @brief Apply speed buff multiplier
         * @param velocity Velocity component
         * @param multiplier Speed multiplier
         */
        void _applySpeedBoost(Velocity &velocity, float multiplier);

        /**
         * @brief Apply damage buff multiplier
         * @param weapon Weapon component
         * @param multiplier Damage multiplier
         */
        void _applyDamageBoost(Weapon &weapon, float multiplier);

        /**
         * @brief Apply fire rate buff multiplier
         * @param weapon Weapon component
         * @param multiplier Fire rate multiplier
         */
        void _applyFireRateBoost(Weapon &weapon, float multiplier);

        /**
         * @brief Apply shield (invincibility)
         * @param health Health component
         * @param duration Remaining shield duration
         */
        void _applyShield(Health &health, float duration);

        /**
         * @brief Apply health regeneration
         * @param health Health component
         * @param deltaTime Time elapsed
         * @param regenRate Health points per second
         */
        void _applyHealthRegen(Health &health, float deltaTime, float regenRate);
    };

}  // namespace ecs
