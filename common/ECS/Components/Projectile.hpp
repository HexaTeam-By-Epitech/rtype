/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Projectile
*/

#ifndef PROJECTILE_HPP_
#define PROJECTILE_HPP_

#include "IComponent.hpp"
#include <cstdint>

namespace ecs {
    class Projectile : public IComponent {
       public:
        Projectile(int damage, int lifetime, std::uint32_t ownerId, bool friendly)
            : _damage(damage), _lifetime(lifetime), _ownerId(ownerId), _friendly(friendly) {}
        ~Projectile() override = default;

        // Getters
        int getDamage() const { return _damage; }
        int getLifetime() const { return _lifetime; }
        std::uint32_t getOwnerId() const { return _ownerId; }
        bool isFriendly() const { return _friendly; }

        // Setters
        void setDamage(int damage) { _damage = damage; }
        void setLifetime(int lifetime) { _lifetime = lifetime; }
        void setOwnerId(std::uint32_t ownerId) { _ownerId = ownerId; }
        void setFriendly(bool friendly) { _friendly = friendly; }

        // Override getType method
        ComponentType getType() const override {
            return getComponentType<Projectile>();
        }

       private:
       int _damage;
       int _lifetime;
       std::uint32_t _ownerId;
       bool _friendly;
    };
}

#endif
