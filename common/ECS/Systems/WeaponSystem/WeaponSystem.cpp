/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** WeaponSystem
*/

#include "WeaponSystem.hpp"
#include <cmath>
#include "common/Animation/AnimationDatabase.hpp"
#include "common/ECS/Components/Animation.hpp"
#include "common/ECS/Components/AnimationSet.hpp"
#include "common/ECS/Components/Collider.hpp"
#include "common/ECS/Components/IComponent.hpp"
#include "common/ECS/Components/Projectile.hpp"
#include "common/ECS/Components/Sprite.hpp"
#include "common/ECS/Prefabs/PrefabFactory.hpp"

namespace ecs {
    /**
     * @brief Updates weapon cooldowns for all entities with weapons.
     */
    void WeaponSystem::update(Registry &registry, float deltaTime) {
        auto entities = registry.getEntitiesWithMask(this->getComponentMask());

        for (auto entityId : entities) {
            auto &weapon = registry.getComponent<Weapon>(entityId);

            // Update cooldown
            weapon.setCooldown(std::max(0.0F, weapon.getCooldown() - deltaTime));

            // Handle charging
            if (weapon.shouldShoot() && weapon.getCooldown() <= 0.0F) {
                // Start or continue charging
                if (!weapon.isCharging()) {
                    weapon.setCharging(true);
                    weapon.setChargeLevel(0.0F);
                }

                // Accumulate charge
                float newCharge = weapon.getChargeLevel() + (weapon.getChargeRate() * deltaTime);
                weapon.setChargeLevel(newCharge);

                // Cap at full charge (1.0)
                if (weapon.getChargeLevel() >= 1.0F) {
                    weapon.setChargeLevel(1.0F);
                }
            } else if (!weapon.shouldShoot() && weapon.isCharging()) {
                // Button released - fire charged shot
                float chargeLevel = weapon.getChargeLevel();

                // Fire the charged shot
                bool isFriendly = true;  // Assume player weapon
                fireChargedShot(registry, entityId, chargeLevel, isFriendly);

                // Reset charge state
                weapon.setCharging(false);
                weapon.setChargeLevel(0.0F);

                // Set cooldown
                float fireRate = weapon.getFireRate();
                if (fireRate > 0.0F) {
                    weapon.setCooldown(1.0F / fireRate);
                } else {
                    weapon.setCooldown(1.0F / 7.0F);  // Fallback: 7 shots/sec
                }
            } else if (!weapon.shouldShoot() && !weapon.isCharging()) {
                // Not shooting and not charging - reset charge
                weapon.setChargeLevel(0.0F);
            }
        }
    }

    std::uint32_t WeaponSystem::fireWeapon(Registry &registry, std::uint32_t ownerId, bool isFriendly) {
        // Get weapon data from owner
        if (!registry.hasComponent<Weapon>(ownerId)) {
            return 0;
        }

        auto &weapon = registry.getComponent<Weapon>(ownerId);
        float damage = weapon.getDamage();
        float baseSpeed = 500.0f;  // Default projectile speed

        // Check for multishot buffs
        int shotCount = 1;

        // Add Collider for collision detection (Layer 4=projectiles, mask=all, solid collision)
        registry.setComponent(projectileId, Collider(10.0f, 10.0f, 0.0f, 0.0f, 4, 0xFFFFFFFF, false));

        // Add projectile animations
        ecs::AnimationSet bulletAnimations = AnimDB::createPlayerBulletAnimations();
        registry.setComponent(projectileId, bulletAnimations);
        registry.setComponent(projectileId, ecs::Animation("projectile_fly", true, true));
        if (registry.hasComponent<Buff>(ownerId)) {
            const Buff &buff = registry.getComponent<Buff>(ownerId);

            // Check all multishot buffs and keep the highest
            if (buff.hasBuff(BuffType::MultiShot)) {
                shotCount = 5;  // MultiShot fires 5 projectiles in a spread
            }
            if (buff.hasBuff(BuffType::TripleShot) && shotCount < 3) {
                shotCount = 3;
            }
            if (buff.hasBuff(BuffType::DoubleShot) && shotCount < 2) {
                shotCount = 2;
            }
        }

        // Fire multiple shots if multishot buff is active
        if (shotCount > 1) {
            fireMultipleShots(registry, ownerId, damage, baseSpeed, isFriendly, false, shotCount);
            return 0;  // Return 0 as we created multiple projectiles
        }

        // Single shot - create projectile
        Transform projectileTransform = calculateProjectileTransform(registry, ownerId);
        Velocity projectileVelocity = calculateProjectileVelocity(baseSpeed);

        return createProjectile(registry, ownerId, projectileTransform, projectileVelocity, damage,
                                isFriendly, false);
    }

    Velocity WeaponSystem::calculateProjectileVelocity(float baseSpeed) {
        return Velocity(1.0F, 0.0F, baseSpeed);
    }

    Transform WeaponSystem::calculateProjectileTransform(Registry &registry, std::uint32_t ownerId) {
        // Get owner's position
        Transform defaultTransform(0.0F, 0.0F);

        if (registry.hasComponent<Transform>(ownerId)) {
            auto &ownerTransform = registry.getComponent<Transform>(ownerId);
            auto ownerPos = ownerTransform.getPosition();

            // Projectile spawns 20 pixels offset from the owner (to the right)
            return Transform(ownerPos.x + 40.0F, ownerPos.y);
        }

        return defaultTransform;
    }

    std::uint32_t WeaponSystem::fireChargedShot(Registry &registry, std::uint32_t ownerId, float chargeLevel,
                                                bool isFriendly) {
        // Check if charge is sufficient (minimum 50% charge)
        const float CHARGE_THRESHOLD = 0.5F;

        if (chargeLevel < CHARGE_THRESHOLD) {
            // Fire normal shot if not charged enough
            return fireWeapon(registry, ownerId, isFriendly);
        }

        // Get weapon data from owner
        if (!registry.hasComponent<Weapon>(ownerId)) {
            return 0;
        }

        auto &weapon = registry.getComponent<Weapon>(ownerId);

        // Calculate charge-based multipliers
        float damageMultiplier = 1.0F + chargeLevel * 1.5F;  // Up to 2.5x at full charge
        float speedMultiplier = 1.0F + chargeLevel * 0.5F;   // Up to 1.5x at full charge

        float chargedDamage = weapon.getDamage() * damageMultiplier;
        float chargedSpeed = 500.0f * speedMultiplier;  // Default projectile speed with charge multiplier

        // Check for multishot buffs
        int shotCount = 1;

        if (registry.hasComponent<Buff>(ownerId)) {
            const Buff &buff = registry.getComponent<Buff>(ownerId);

            // Check all multishot buffs and keep the highest
            if (buff.hasBuff(BuffType::MultiShot)) {
                shotCount = 5;
            }
            if (buff.hasBuff(BuffType::TripleShot) && shotCount < 3) {
                shotCount = 3;
            }
            if (buff.hasBuff(BuffType::DoubleShot) && shotCount < 2) {
                shotCount = 2;
            }
        }

        // Fire multiple shots if multishot buff is active
        if (shotCount > 1) {
            fireMultipleShots(registry, ownerId, chargedDamage, chargedSpeed, isFriendly, true, shotCount);
            return 0;
        }

        // Single charged shot
        Transform projectileTransform = calculateProjectileTransform(registry, ownerId);
        Velocity projectileVelocity = calculateProjectileVelocity(chargedSpeed);

        return createProjectile(registry, ownerId, projectileTransform, projectileVelocity, chargedDamage,
                                isFriendly, true);
    }

    ComponentMask WeaponSystem::getComponentMask() const {
        return (1ULL << getComponentType<Weapon>()) | (1ULL << getComponentType<Transform>());
    }

    std::uint32_t WeaponSystem::createProjectile(Registry &registry, std::uint32_t ownerId,
                                                 const Transform &transform, const Velocity &velocity,
                                                 float damage, bool isFriendly, bool isCharged) {
        auto projectileId = registry.newEntity();

        registry.setComponent(projectileId, transform);
        registry.setComponent(projectileId, velocity);
        registry.setComponent(projectileId, Projectile(damage, 10.0f, ownerId, isFriendly));

        // Add projectile animations
        ecs::AnimationSet bulletAnimations = AnimDB::createPlayerBulletAnimations();
        registry.setComponent(projectileId, bulletAnimations);

        // Select animation and sprite based on shot type
        std::string animationName = isCharged ? "charged_projectile_1" : "projectile_fly";
        ecs::Rectangle projRect = {267, 84, 17, 13};
        float scale = isCharged ? 2.5f : 2.0f;

        registry.setComponent(projectileId, ecs::Animation(animationName, true, true));
        ecs::Sprite projSprite("Projectiles", projRect, scale, 0.0f, false, false, 0);
        registry.setComponent(projectileId, projSprite);

        LOG_DEBUG("Created projectile ", projectileId, " (charged:", isCharged, ") with damage:", damage);

        // Reset weapon cooldown
        if (registry.hasComponent<Weapon>(ownerId)) {
            auto &weapon = registry.getComponent<Weapon>(ownerId);
            float fireRate = weapon.getFireRate();
            if (fireRate > 0.0F) {
                weapon.setCooldown(1.0F / fireRate);
            }
        }

        // Invoke callback if set
        if (_projectileCreatedCallback && projectileId != 0) {
            auto pos = transform.getPosition();
            auto dir = velocity.getDirection();
            float speed = velocity.getSpeed();
            _projectileCreatedCallback(projectileId, ownerId, pos.x, pos.y, dir.x, dir.y, speed,
                                       static_cast<int>(damage), isFriendly);
        }

        return projectileId;
    }

    void WeaponSystem::fireMultipleShots(Registry &registry, std::uint32_t ownerId, float damage, float speed,
                                         bool isFriendly, bool isCharged, int shotCount) {
        if (!registry.hasComponent<Transform>(ownerId)) {
            return;
        }

        // Calculate angle spread based on shot count
        float angleSpread = 0.0f;
        float startAngle = 0.0f;

        switch (shotCount) {
            case 2:                   // DoubleShot
                angleSpread = 15.0f;  // 15 degrees apart
                startAngle = -7.5f;   // Center the spread
                break;
            case 3:  // TripleShot
                angleSpread = 15.0f;
                startAngle = -15.0f;
                break;
            case 5:  // MultiShot
                angleSpread = 15.0f;
                startAngle = -30.0f;
                break;
            default:
                angleSpread = 10.0f;
                startAngle = -(angleSpread * (shotCount - 1)) / 2.0f;
                break;
        }

        Transform baseTransform = calculateProjectileTransform(registry, ownerId);

        // Fire each projectile with different angle
        for (int i = 0; i < shotCount; i++) {
            float angle = startAngle + (i * angleSpread);
            float angleRad = angle * 3.14159f / 180.0f;

            // Calculate velocity with angle
            float dirX = std::cos(angleRad);
            float dirY = std::sin(angleRad);
            Velocity projectileVelocity(dirX, dirY, speed);

            // Slightly offset Y position for visual variety
            Transform projectileTransform = baseTransform;
            auto pos = projectileTransform.getPosition();
            projectileTransform.setPosition(pos.x, pos.y + (i - shotCount / 2) * 5.0f);

            createProjectile(registry, ownerId, projectileTransform, projectileVelocity, damage, isFriendly,
                             isCharged);
        }

        LOG_INFO("[WEAPON] Fired ", shotCount, " projectiles with multishot buff");
    }
}  // namespace ecs
