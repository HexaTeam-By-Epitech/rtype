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
    class Weapon : public IComponent {
       public:
        Weapon(int fireRate, int cooldown, int projectileType, int damage)
            : _fireRate(fireRate), _cooldown(cooldown), _projectileType(projectileType), _damage(damage) {}
        ~Weapon() override = default;

        // Getters
        int getFireRate() const { return _fireRate; }
        int getCooldown() const { return _cooldown; }
        int getProjectileType() const { return _projectileType; }
        int getDamage() const { return _damage; }

        // Setters
        void setFireRate(int fireRate) { _fireRate = fireRate; }
        void setCooldown(int cooldown) { _cooldown = cooldown; }
        void setProjectileType(int projectileType) { _projectileType = projectileType; }
        void setDamage(int damage) { _damage = damage; }

        // Override getType method
        ComponentType getType() const override {
            return getComponentType<Weapon>();
        }

       private:
       int _fireRate;
       int _cooldown;
       int _projectileType;
       int _damage;
    };
}

#endif
