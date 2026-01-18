/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Weapon
*/

#pragma once

#include "IComponent.hpp"

namespace ecs {
    /**
     * @class Weapon
     * @brief Component for entities capable of shooting projectiles.
     * 
     * Manages weapon characteristics including fire rate, cooldown timer,
     * projectile type spawned, base damage dealt, and charged shot mechanics.
     * 
     * ## Charged Shots
     * Weapons support a charging mechanic where holding the fire button accumulates
     * charge over time. When released:
     * - If charge >= 50%: Fires a charged shot with increased damage (up to 2.5x) and speed (up to 1.5x)
     * - If charge < 50%: Fires a normal shot
     * 
     * Charge accumulation is controlled by the `chargeRate` field (default: 1.0 = full charge in 1 second).
     */
    class Weapon : public IComponent {
       public:
        /**
         * @brief Constructor with weapon parameters.
         * @param fireRate Shots per second (higher = faster)
         * @param cooldown Current cooldown timer (in seconds)
         * @param projectileType Type of projectile spawned
         * @param damage Base damage dealt by this weapon
         */
        Weapon(float fireRate, float cooldown, int projectileType, int damage)
            : _fireRate(fireRate),
              _cooldown(cooldown),
              _projectileType(projectileType),
              _damage(damage),
              _baseFireRate(fireRate),
              _baseDamage(damage) {}
        ~Weapon() override = default;

        /**
         * @brief Get fire rate.
         * @return float Shots per second.
         */
        [[nodiscard]] float getFireRate() const { return _fireRate; }

        /**
         * @brief Get current cooldown.
         * @return float Remaining cooldown in seconds.
         */
        [[nodiscard]] float getCooldown() const { return _cooldown; }

        /**
         * @brief Get projectile type.
         * @return int Type of projectile spawned.
         */
        [[nodiscard]] int getProjectileType() const { return _projectileType; }

        /**
         * @brief Get damage value.
         * @return float Base damage dealt.
         */
        [[nodiscard]] float getDamage() const { return _damage; }

        /**
         * @brief Check if the weapon should shoot.
         * @return bool True if the weapon is set to shoot, false otherwise.
         */
        [[nodiscard]] bool shouldShoot() const { return _shouldShoot; }

        /**
         * @brief Get base fire rate (before buffs).
         * @return float Base shots per second
         */
        float getBaseFireRate() const { return _baseFireRate; }

        /**
         * @brief Get base damage (before buffs).
         * @return int Base damage value
         */
        int getBaseDamage() const { return _baseDamage; }

        /**
         * @brief Set fire rate.
         * @param fireRate New shots per second
         */
        void setFireRate(float fireRate) { _fireRate = fireRate; }

        /**
         * @brief Set cooldown timer.
         * @param cooldown New cooldown value in seconds
         */
        void setCooldown(float cooldown) { _cooldown = cooldown; }

        /**
         * @brief Set projectile type.
         * @param projectileType New projectile type
         */
        void setProjectileType(int projectileType) { _projectileType = projectileType; }

        /**
         * @brief Set damage value.
         * @param damage New damage value
         */
        void setDamage(float damage) { _damage = damage; }

        /**
         * @brief Set whether the weapon should shoot.
         * @param shouldShoot True to enable shooting, false to disable
         */
        void setShouldShoot(bool shouldShoot) { _shouldShoot = shouldShoot; }

        /**
         * @brief Check if weapon is currently charging.
         * @return bool True if charging, false otherwise
         */
        [[nodiscard]] bool isCharging() const { return _isCharging; }

        /**
         * @brief Set charging state.
         * @param charging True to start charging, false to stop
         */
        void setCharging(bool charging) { _isCharging = charging; }

        /**
         * @brief Get current charge level.
         * @return float Charge level from 0.0 (empty) to 1.0 (full)
         */
        [[nodiscard]] float getChargeLevel() const { return _chargeLevel; }

        /**
         * @brief Set charge level.
         * @param level Charge level (clamped to 0.0-1.0)
         */
        void setChargeLevel(float level) {
            _chargeLevel = (level < 0.0F) ? 0.0F : (level > 1.0F) ? 1.0F : level;
        }

        /**
         * @brief Get charge rate (how fast the weapon charges).
         * @return float Charge rate (0.0 to 1.0 per second)
         */
        [[nodiscard]] float getChargeRate() const { return _chargeRate; }

        /**
         * @brief Set charge rate.
         * @param rate Charge rate per second
         */
        void setChargeRate(float rate) { _chargeRate = rate; }

        /**
         * @brief Get the component type ID.
         * @return ComponentType Unique ID for Weapon component.
         */
        ComponentType getType() const override { return getComponentType<Weapon>(); }

       private:
        float _fireRate;            ///< Shots per second
        float _cooldown;            ///< Current cooldown timer in seconds
        int _projectileType;        ///< Type of projectile spawned
        float _damage;              ///< Base damage dealt
        bool _shouldShoot = false;  ///< Whether the weapon is currently set to shoot
        float _baseFireRate;        ///< Base fire rate (before buffs)
        int _baseDamage;            ///< Base damage (before buffs)

        // Charge shot fields
        bool _isCharging = false;   ///< Whether the weapon is currently charging
        float _chargeLevel = 0.0F;  ///< Current charge level (0.0 to 1.0)
        float _chargeRate = 1.0F;   ///< How fast the weapon charges per second
    };
}  // namespace ecs
