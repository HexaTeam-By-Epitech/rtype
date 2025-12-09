/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Weapon
*/

#ifndef WEAPON_HPP_
#define WEAPON_HPP_

#include "IComponent.hpp"

namespace ecs {
    /**
     * @class Weapon
     * @brief Component for entities capable of shooting projectiles.
     * 
     * Manages weapon characteristics including fire rate, cooldown timer,
     * projectile type spawned, and base damage dealt.
     */
    class Weapon : public IComponent {
       public:
        /**
         * @brief Constructor with weapon parameters.
         * @param fireRate Shots per second (higher = faster)
         * @param cooldown Current cooldown timer (in frames/ticks)
         * @param projectileType Type of projectile spawned
         * @param damage Base damage dealt by this weapon
         */
        Weapon(int fireRate, int cooldown, int projectileType, int damage)
            : _fireRate(fireRate), _cooldown(cooldown), _projectileType(projectileType), _damage(damage) {}
        ~Weapon() override = default;

        /**
         * @brief Get fire rate.
         * @return int Shots per second.
         */
        int getFireRate() const { return _fireRate; }

        /**
         * @brief Get current cooldown.
         * @return int Remaining cooldown in frames/ticks.
         */
        int getCooldown() const { return _cooldown; }

        /**
         * @brief Get projectile type.
         * @return int Type of projectile spawned.
         */
        int getProjectileType() const { return _projectileType; }

        /**
         * @brief Get damage value.
         * @return int Base damage dealt.
         */
        int getDamage() const { return _damage; }

        /**
         * @brief Set fire rate.
         * @param fireRate New shots per second
         */
        void setFireRate(int fireRate) { _fireRate = fireRate; }

        /**
         * @brief Set cooldown timer.
         * @param cooldown New cooldown value
         */
        void setCooldown(int cooldown) { _cooldown = cooldown; }

        /**
         * @brief Set projectile type.
         * @param projectileType New projectile type
         */
        void setProjectileType(int projectileType) { _projectileType = projectileType; }

        /**
         * @brief Set damage value.
         * @param damage New damage value
         */
        void setDamage(int damage) { _damage = damage; }

        /**
         * @brief Get the component type ID.
         * @return ComponentType Unique ID for Weapon component.
         */
        ComponentType getType() const override { return getComponentType<Weapon>(); }

       private:
        int _fireRate;        ///< Shots per second
        int _cooldown;        ///< Current cooldown timer
        int _projectileType;  ///< Type of projectile spawned
        int _damage;          ///< Base damage dealt
    };
}  // namespace ecs

#endif
