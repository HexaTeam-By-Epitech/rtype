/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** BuffSystem implementation
*/

#include "BuffSystem.hpp"
#include <algorithm>

namespace ecs {

    void BuffSystem::update(Registry &registry, float deltaTime) {
        // Get all entities with Buff component
        auto entities = registry.view<Buff>();

        for (auto entity : entities) {
            Buff &buff = registry.getComponent<Buff>(entity);

            // Update buff timers and remove expired ones
            _updateBuffTimers(buff, deltaTime);

            // Apply buff effects to entity stats
            if (buff.hasAnyBuffs()) {
                _applyBuffEffects(entity, registry, buff);
            }

            // Remove Buff component if no buffs remain
            if (!buff.hasAnyBuffs()) {
                registry.removeComponent<Buff>(entity);
            }
        }
    }

    ComponentMask BuffSystem::getComponentMask() const {
        ComponentMask mask = 0;
        mask |= (1ULL << getComponentType<Buff>());
        return mask;
    }

    void BuffSystem::_updateBuffTimers(Buff &buff, float deltaTime) {
        auto &buffs = buff.getBuffsMutable();

        // Remove expired buffs
        buffs.erase(std::remove_if(buffs.begin(), buffs.end(),
                                   [deltaTime](BuffInstance &b) {
                                       if (b.isPermanent) {
                                           return false;  // Never remove permanent buffs
                                       }
                                       b.duration -= deltaTime;
                                       return b.duration <= 0.0f;  // Remove if expired
                                   }),
                    buffs.end());
    }

    void BuffSystem::_applyBuffEffects(Address address, Registry &registry, const Buff &buff) {
        const auto &buffs = buff.getBuffs();

        for (const auto &b : buffs) {
            switch (b.type) {
                case BuffType::SpeedBoost:
                    if (registry.hasComponent<Velocity>(address)) {
                        Velocity &vel = registry.getComponent<Velocity>(address);
                        _applySpeedBoost(vel, b.value);
                    }
                    break;

                case BuffType::DamageBoost:
                    if (registry.hasComponent<Weapon>(address)) {
                        Weapon &weapon = registry.getComponent<Weapon>(address);
                        _applyDamageBoost(weapon, b.value);
                    }
                    break;

                case BuffType::FireRateBoost:
                    if (registry.hasComponent<Weapon>(address)) {
                        Weapon &weapon = registry.getComponent<Weapon>(address);
                        _applyFireRateBoost(weapon, b.value);
                    }
                    break;

                case BuffType::Shield:
                    if (registry.hasComponent<Health>(address)) {
                        Health &health = registry.getComponent<Health>(address);
                        _applyShield(health, b.duration);
                    }
                    break;

                case BuffType::HealthRegen:
                    if (registry.hasComponent<Health>(address)) {
                        Health &health = registry.getComponent<Health>(address);
                        // b.value = regen rate (HP per second)
                        _applyHealthRegen(health, 0.016f, b.value);  // Assume ~60 FPS
                    }
                    break;

                // Permanent buffs (modify behavior, handled by weapon system)
                case BuffType::MultiShot:
                case BuffType::DoubleShot:
                case BuffType::TripleShot:
                case BuffType::PiercingShot:
                case BuffType::HomingShot:
                    // These are checked by WeaponSystem when firing
                    break;

                case BuffType::MaxHealthIncrease:
                    if (registry.hasComponent<Health>(address)) {
                        Health &health = registry.getComponent<Health>(address);
                        // Already applied when buff was added
                        // This is a permanent increase
                    }
                    break;
            }
        }
    }

    void BuffSystem::_applySpeedBoost(Velocity &velocity, float multiplier) {
        // Speed boost is multiplicative
        // Note: This modifies speed every frame, so base speed should be stored separately
        // For simplicity, we apply it directly (assumes system order handles this)
        float currentSpeed = velocity.getSpeed();
        // Speed already includes buffs from previous frame
        // To properly handle this, we'd need to store base speed separately
        // For now, just ensure speed is reasonable
        (void)multiplier;  // Placeholder - needs proper implementation
    }

    void BuffSystem::_applyDamageBoost(Weapon &weapon, float multiplier) {
        // Similar issue - needs base damage storage
        (void)weapon;
        (void)multiplier;
    }

    void BuffSystem::_applyFireRateBoost(Weapon &weapon, float multiplier) {
        // Similar issue - needs base fire rate storage
        (void)weapon;
        (void)multiplier;
    }

    void BuffSystem::_applyShield(Health &health, float duration) {
        if (duration > 0.0f) {
            health.setInvincible(true);
            health.setInvincibilityTimer(duration);
        }
    }

    void BuffSystem::_applyHealthRegen(Health &health, float deltaTime, float regenRate) {
        int currentHealth = health.getCurrentHealth();
        int maxHealth = health.getMaxHealth();

        if (currentHealth < maxHealth) {
            float regenAmount = regenRate * deltaTime;
            int newHealth = std::min(currentHealth + static_cast<int>(regenAmount), maxHealth);
            health.setCurrentHealth(newHealth);
        }
    }

}  // namespace ecs
