/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** BuffSystem implementation
*/

#include "BuffSystem.hpp"
#include <algorithm>
#include "common/Logger/Logger.hpp"

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
        auto initialSize = buffs.size();
        buffs.erase(std::remove_if(buffs.begin(), buffs.end(),
                                   [deltaTime](BuffInstance &b) {
                                       if (b.isPermanent) {
                                           return false;  // Never remove permanent buffs
                                       }
                                       b.duration -= deltaTime;
                                       if (b.duration <= 0.0f) {
                                           // Log expired buff
                                           const char *buffName = "Unknown";
                                           switch (b.type) {
                                               case BuffType::SpeedBoost:
                                                   buffName = "SpeedBoost";
                                                   break;
                                               case BuffType::DamageBoost:
                                                   buffName = "DamageBoost";
                                                   break;
                                               case BuffType::FireRateBoost:
                                                   buffName = "FireRateBoost";
                                                   break;
                                               case BuffType::Shield:
                                                   buffName = "Shield";
                                                   break;
                                               case BuffType::HealthRegen:
                                                   buffName = "HealthRegen";
                                                   break;
                                               default:
                                                   break;
                                           }
                                           LOG_INFO("[BUFF] ", buffName, " expired");
                                           return true;
                                       }
                                       return false;
                                   }),
                    buffs.end());

        if (buffs.size() < initialSize) {
            LOG_DEBUG("[BUFF] Removed ", (initialSize - buffs.size()), " expired buff(s)");
        }
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
                    // Already applied when buff was added (permanent increase)
                    // No per-frame processing needed
                    break;
            }
        }
    }

    void BuffSystem::_applySpeedBoost(Velocity &velocity, float multiplier) {
        // Apply speed multiplier to base speed
        float baseSpeed = velocity.getBaseSpeed();
        float newSpeed = baseSpeed * multiplier;
        velocity.setSpeed(newSpeed);
    }

    void BuffSystem::_applyDamageBoost(Weapon &weapon, float multiplier) {
        // Apply damage multiplier to base damage
        int baseDamage = weapon.getBaseDamage();
        int newDamage = static_cast<int>(baseDamage * multiplier);
        weapon.setDamage(newDamage);
    }

    void BuffSystem::_applyFireRateBoost(Weapon &weapon, float multiplier) {
        // Apply fire rate multiplier to base fire rate
        float baseFireRate = weapon.getBaseFireRate();
        float newFireRate = baseFireRate * multiplier;
        weapon.setFireRate(newFireRate);
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
