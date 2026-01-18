/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Health
*/

#pragma once

#include "IComponent.hpp"

namespace ecs {
    /**
     * @class Health
     * @brief Component representing entity health and invincibility.
     * 
     * Stores current and maximum health values, as well as temporary invincibility state.
     * The invincibility timer is decremented by systems to manage temporary immunity.
     */
    class Health : public IComponent {
       public:
        /**
         * @brief Constructor with maximum health only.
         * Sets current health equal to max health.
         * @param maxHealth Maximum health points
         */
        explicit Health(int maxHealth)
            : _current(maxHealth), _max(maxHealth), _invincible(false), _timer(0.0f) {}

        /**
         * @brief Constructor with current and maximum health.
         * @param currentHealth Starting health points
         * @param maxHealth Maximum health points
         */
        Health(int currentHealth, int maxHealth)
            : _current(currentHealth), _max(maxHealth), _invincible(false), _timer(0.0f) {}

        ~Health() override = default;

        /**
         * @brief Get current health points.
         * @return int Current health value.
         */
        int getCurrentHealth() const { return _current; }

        /**
         * @brief Get maximum health points.
         * @return int Maximum health value.
         */
        int getMaxHealth() const { return _max; }

        /**
         * @brief Check if entity is invincible.
         * @return bool True if invincible, false otherwise.
         */
        bool isInvincible() const { return _invincible; }

        /**
         * @brief Get remaining invincibility time.
         * @return float Timer value in seconds.
         */
        float getInvincibilityTimer() const { return _timer; }

        /**
         * @brief Set maximum health.
         * @param health New maximum health value
         */
        void setMaxHealth(int health) { _max = health; }

        /**
         * @brief Set current health.
         * @param health New current health value
         */
        void setCurrentHealth(int health) { _current = health; }

        /**
         * @brief Set invincibility state.
         * @param invincible True to enable invincibility
         */
        void setInvincible(bool invincible) { _invincible = invincible; }

        /**
         * @brief Set invincibility timer.
         * @param timer Timer value in seconds
         */
        void setInvincibilityTimer(float timer) { _timer = timer; }

        /**
         * @brief Apply damage to the entity.
         * Respects invincibility frames - no damage taken if invincible.
         * @param amount Damage amount (positive value)
         * @return bool True if damage was applied, false if blocked by invincibility
         */
        bool takeDamage(int amount) {
            if (_invincible || amount <= 0) {
                return false;
            }
            _current -= amount;
            if (_current < 0) {
                _current = 0;
            }
            return true;
        }

        /**
         * @brief Restore health points.
         * Cannot exceed maximum health.
         * @param amount Health to restore (positive value)
         */
        void heal(int amount) {
            if (amount <= 0) {
                return;
            }
            _current += amount;
            if (_current > _max) {
                _current = _max;
            }
        }

        /**
         * @brief Check if entity is dead (health <= 0).
         * @return bool True if dead, false if alive.
         */
        bool isDead() const { return _current <= 0; }

        /**
         * @brief Get the component type ID.
         * @return ComponentType Unique ID for Health component.
         */
        ComponentType getType() const override { return getComponentType<Health>(); }

       private:
        int _current;      ///< Current health points
        int _max;          ///< Maximum health points
        bool _invincible;  ///< Invincibility state
        float _timer;      ///< Invincibility duration timer in seconds
    };
}  // namespace ecs
