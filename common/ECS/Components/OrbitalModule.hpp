/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** OrbitalModule - Component for orbital companion modules
*/

#pragma once

#include <cstdint>
#include "IComponent.hpp"

namespace ecs {
    /**
     * @class OrbitalModule
     * @brief Component for entities that orbit around a parent entity (like drones in Isaac).
     * 
     * Defines orbital movement around a parent entity (typically a player).
     * The module rotates at a fixed radius and speed, blocking projectiles
     * and damaging enemies on contact.
     */
    class OrbitalModule : public IComponent {
       public:
        /**
         * @brief Constructor with all orbital parameters.
         * @param parentEntityId ID of the entity to orbit around
         * @param orbitRadius Distance from parent entity center
         * @param orbitSpeed Angular velocity in radians per second
         * @param startAngle Initial angle in radians (default: 0)
         * @param damage Damage dealt on contact with enemies
         */
        OrbitalModule(uint32_t parentEntityId, float orbitRadius, float orbitSpeed, float startAngle = 0.0f,
                      int damage = 10)
            : _parentEntityId(parentEntityId),
              _orbitRadius(orbitRadius),
              _orbitSpeed(orbitSpeed),
              _currentAngle(startAngle),
              _damage(damage),
              _blocksProjectiles(true) {}

        ~OrbitalModule() override = default;

        /**
         * @brief Get parent entity ID.
         * @return uint32_t Entity ID to orbit around.
         */
        uint32_t getParentEntityId() const { return _parentEntityId; }

        /**
         * @brief Get orbit radius.
         * @return float Distance from parent center.
         */
        float getOrbitRadius() const { return _orbitRadius; }

        /**
         * @brief Get orbit speed.
         * @return float Angular velocity in rad/s.
         */
        float getOrbitSpeed() const { return _orbitSpeed; }

        /**
         * @brief Get current angle.
         * @return float Current angle in radians.
         */
        float getCurrentAngle() const { return _currentAngle; }

        /**
         * @brief Get contact damage.
         * @return int Damage dealt to enemies.
         */
        int getDamage() const { return _damage; }

        /**
         * @brief Check if blocks projectiles.
         * @return bool True if blocks enemy projectiles.
         */
        bool blocksProjectiles() const { return _blocksProjectiles; }

        /**
         * @brief Set parent entity ID.
         * @param parentEntityId New parent entity ID
         */
        void setParentEntityId(uint32_t parentEntityId) { _parentEntityId = parentEntityId; }

        /**
         * @brief Set orbit radius.
         * @param orbitRadius New orbit radius
         */
        void setOrbitRadius(float orbitRadius) { _orbitRadius = orbitRadius; }

        /**
         * @brief Set orbit speed.
         * @param orbitSpeed New angular velocity
         */
        void setOrbitSpeed(float orbitSpeed) { _orbitSpeed = orbitSpeed; }

        /**
         * @brief Set current angle.
         * @param angle New angle in radians
         */
        void setCurrentAngle(float angle) { _currentAngle = angle; }

        /**
         * @brief Set contact damage.
         * @param damage New damage value
         */
        void setDamage(int damage) { _damage = damage; }

        /**
         * @brief Set projectile blocking status.
         * @param blocks True to block projectiles
         */
        void setBlocksProjectiles(bool blocks) { _blocksProjectiles = blocks; }

        /**
         * @brief Get the component type ID.
         * @return ComponentType Unique ID for OrbitalModule component.
         */
        ComponentType getType() const override { return getComponentType<OrbitalModule>(); }

       private:
        uint32_t _parentEntityId;  ///< Entity to orbit around
        float _orbitRadius;        ///< Distance from parent (in pixels)
        float _orbitSpeed;         ///< Angular velocity (rad/s)
        float _currentAngle;       ///< Current orbital angle (radians)
        int _damage;               ///< Damage dealt on contact
        bool _blocksProjectiles;   ///< Whether it blocks enemy projectiles
    };
}  // namespace ecs
