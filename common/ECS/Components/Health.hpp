/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Health
*/

#ifndef HEALTH_HPP_
#define HEALTH_HPP_

#include "IComponent.hpp"

namespace ecs {
    class Health : public IComponent {
       public:
        // Constructors
        Health(int maxHealth)
            : _current(maxHealth), _max(maxHealth), _invincible(false), _timer(0) {}
        Health(int currentHealth, int maxHealth)
            : _current(currentHealth), _max(maxHealth), _invincible(false), _timer(0) {}
        ~Health() override = default;

        // Getters
        int getCurrentHealth() const { return _current; }
        int getMaxHealth() const { return _max; }
        bool isInvincible() const { return _invincible; }
        int getInvincibilityTimer() const { return _timer; }

        // Setters
        void setMaxHealth(int health) { _max = health; }
        void setCurrentHealth(int health) { _current = health; }
        void setInvincible(bool invincible) { _invincible = invincible; }
        void setInvincibilityTimer(int timer) { _timer = timer; }

        // Override getType method
        ComponentType getType() const override {
            return getComponentType<Health>();
        }
        private:
        int _current;
        int _max;
        bool _invincible;
        int _timer;
    };
}

#endif
