/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** WeaponSystem
*/

#include "WeaponSystem.hpp"
#include "common/Animation/AnimationDatabase.hpp"
#include "common/ECS/Components/Animation.hpp"
#include "common/ECS/Components/AnimationSet.hpp"
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

        // Calculate projectile properties
        Transform projectileTransform = calculateProjectileTransform(registry, ownerId);
        Velocity projectileVelocity = calculateProjectileVelocity(300.0F);  // Base speed: 300 units/sec

        auto projectileId = registry.newEntity();

        registry.setComponent(projectileId, projectileTransform);
        registry.setComponent(projectileId, projectileVelocity);
        registry.setComponent(projectileId, Projectile(weapon.getDamage(), 10.0f, ownerId, isFriendly));

        // Add projectile animations
        ecs::AnimationSet bulletAnimations = AnimDB::createPlayerBulletAnimations();
        registry.setComponent(projectileId, bulletAnimations);
        registry.setComponent(projectileId, ecs::Animation("projectile_fly", true, true));

        // Create sprite with explicit Rectangle
        ecs::Rectangle projRect = {267, 84, 17, 13};
        ecs::Sprite projSprite("Projectiles", projRect, 2.0f, 0.0f, false, false, 0);
        registry.setComponent(projectileId, projSprite);

        // Debug: verify sprite was created correctly
        LOG_DEBUG("Created projectile ", projectileId, " with Sprite rect(", projRect.x, ",", projRect.y, ",",
                  projRect.width, ",", projRect.height, ")");

        // Reset weapon cooldown
        float fireRate = weapon.getFireRate();
        if (fireRate > 0.0F) {
            weapon.setCooldown(1.0F / fireRate);  // Convert fire rate (shots/sec) to cooldown (sec/shot)
        }

        // Invoke callback if set and projectile was created
        if (_projectileCreatedCallback && projectileId != 0) {
            auto &transform = registry.getComponent<Transform>(projectileId);
            auto &velocity = registry.getComponent<Velocity>(projectileId);
            auto pos = transform.getPosition();
            auto dir = velocity.getDirection();
            _projectileCreatedCallback(projectileId, ownerId, pos.x, pos.y, dir.x, dir.y, 300.0f,
                                       static_cast<int>(weapon.getDamage()), isFriendly);
        }

        return projectileId;
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

        // Calculate projectile properties
        Transform projectileTransform = calculateProjectileTransform(registry, ownerId);
        float chargedSpeed = 300.0F * speedMultiplier;
        Velocity projectileVelocity = calculateProjectileVelocity(chargedSpeed);

        // Create projectile entity
        auto projectileId = registry.newEntity();

        // Set components with charge-boosted damage
        registry.setComponent(projectileId, projectileTransform);
        registry.setComponent(projectileId, projectileVelocity);
        float chargedDamage = weapon.getDamage() * damageMultiplier;
        registry.setComponent(projectileId, Projectile(chargedDamage, 10.0F, ownerId, isFriendly));

        // Add projectile animations with charge-based sprite selection
        ecs::AnimationSet bulletAnimations = AnimDB::createPlayerBulletAnimations();
        registry.setComponent(projectileId, bulletAnimations);

        // Select animation and sprite based on charge level
        std::string animationName = "projectile_fly";
        ecs::Rectangle chargedRect = {267, 84, 17, 13};  // Default normal projectile
        float scale = 2.0f;

        animationName = "charged_projectile_1";

        registry.setComponent(projectileId, ecs::Animation(animationName, true, true));
        ecs::Sprite chargedSprite("Projectiles", chargedRect, scale, 0.0f, false, false, 0);
        registry.setComponent(projectileId, chargedSprite);

        LOG_DEBUG("Created charged projectile ", projectileId, " (charge: ", chargeLevel,
                  ", anim: ", animationName, ") with Sprite rect(", chargedRect.x, ",", chargedRect.y, ",",
                  chargedRect.width, ",", chargedRect.height, ") scale: ", scale);

        // Reset weapon cooldown
        float fireRate = weapon.getFireRate();
        if (fireRate > 0.0F) {
            weapon.setCooldown(1.0F / fireRate);
        }

        // Invoke callback if set and projectile was created
        if (_projectileCreatedCallback && projectileId != 0) {
            auto &transform = registry.getComponent<Transform>(projectileId);
            auto &velocity = registry.getComponent<Velocity>(projectileId);
            auto pos = transform.getPosition();
            auto dir = velocity.getDirection();
            _projectileCreatedCallback(projectileId, ownerId, pos.x, pos.y, dir.x, dir.y, chargedSpeed,
                                       static_cast<int>(chargedDamage), isFriendly);
        }

        return projectileId;
    }

    ComponentMask WeaponSystem::getComponentMask() const {
        return (1ULL << getComponentType<Weapon>()) | (1ULL << getComponentType<Transform>());
    }
}  // namespace ecs
