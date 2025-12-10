/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Projectile
*/

#pragma once

#include <cstdint>
#include "IComponent.hpp"

namespace ecs {
    /**
     * @class Projectile
     * @brief Component for projectile entities (bullets, missiles, etc.).
     * 
     * Stores projectile-specific data including damage, lifetime duration,
     * owner identification, and team affiliation (friendly/enemy).
     */
    class Projectile : public IComponent {
       public:
        /**
         * @brief Constructor with projectile parameters.
         * @param damage Damage dealt on impact
         * @param lifetime Duration before auto-destruction (in seconds)
         * @param ownerId Entity ID of the shooter
         * @param friendly True if friendly projectile, false if enemy
         */
        Projectile(int damage, float lifetime, std::uint32_t ownerId, bool friendly)
            : _damage(damage), _lifetime(lifetime), _ownerId(ownerId), _friendly(friendly) {}
        ~Projectile() override = default;

        /**
         * @brief Get damage value.
         * @return int Damage dealt on impact.
         */
        int getDamage() const { return _damage; }

        /**
         * @brief Get remaining lifetime.
         * @return float Seconds before auto-destruction.
         */
        float getLifetime() const { return _lifetime; }

        /**
         * @brief Get owner entity ID.
         * @return std::uint32_t ID of the shooting entity.
         */
        std::uint32_t getOwnerId() const { return _ownerId; }

        /**
         * @brief Check if projectile is friendly.
         * @return bool True if friendly, false if enemy.
         */
        bool isFriendly() const { return _friendly; }

        /**
         * @brief Set damage value.
         * @param damage New damage value
         */
        void setDamage(int damage) { _damage = damage; }

        /**
         * @brief Set remaining lifetime.
         * @param lifetime New lifetime value in seconds
         */
        void setLifetime(float lifetime) { _lifetime = lifetime; }

        /**
         * @brief Set owner entity ID.
         * @param ownerId New owner ID
         */
        void setOwnerId(std::uint32_t ownerId) { _ownerId = ownerId; }

        /**
         * @brief Set friendly status.
         * @param friendly True for friendly, false for enemy
         */
        void setFriendly(bool friendly) { _friendly = friendly; }

        /**
         * @brief Get the component type ID.
         * @return ComponentType Unique ID for Projectile component.
         */
        ComponentType getType() const override { return getComponentType<Projectile>(); }

       private:
        int _damage;             ///< Damage dealt on impact
        float _lifetime;         ///< Remaining lifetime in seconds
        std::uint32_t _ownerId;  ///< Entity ID of the shooter
        bool _friendly;          ///< Team affiliation (true=friendly, false=enemy)
    };
}  // namespace ecs
