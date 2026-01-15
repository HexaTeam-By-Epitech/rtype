/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** WeaponSystem
*/

#include "WeaponSystem.hpp"
#include "common/ECS/Components/IComponent.hpp"
#include "common/ECS/Components/Projectile.hpp"
#include "common/ECS/Prefabs/PrefabFactory.hpp"

namespace ecs {
    /**
     * @brief Updates weapon cooldowns for all entities with weapons.
     */
    void WeaponSystem::update(Registry &registry, float deltaTime) {
        auto entities = registry.getEntitiesWithMask(this->getComponentMask());

        for (auto entityId : entities) {
            auto &weapon = registry.getComponent<Weapon>(entityId);

            weapon.setCooldown(std::max(0.0F, weapon.getCooldown() - deltaTime));
            if (weapon.getCooldown() > 0.0F) {
                continue;
            }
            if (!weapon.shouldShoot()) {
                continue;
            }

            // Use fireRate from weapon configuration instead of hardcoded value
            // fireRate is shots per second (e.g., 10.0f = 10 shots/sec)
            // cooldown is seconds per shot (e.g., 1/10 = 0.1s between shots)
            float fireRate = weapon.getFireRate();
            if (fireRate > 0.0F) {
                weapon.setCooldown(1.0F / fireRate);
            } else {
                weapon.setCooldown(0.1F);  // Fallback: 10 shots/sec
            }

            // Calculate projectile properties using same logic as fireWeapon
            Transform projectileTransform = calculateProjectileTransform(registry, entityId);
            Velocity projectileVelocity = calculateProjectileVelocity(registry, entityId, 300.0F);
            auto pos = projectileTransform.getPosition();
            auto dir = projectileVelocity.getDirection();

            uint32_t projectileId =
                PrefabFactory::createProjectile(registry, entityId, pos.x, pos.y, dir.x, dir.y, 300.0F,
                                                static_cast<int>(weapon.getDamage()), true);

            // Invoke callback if set and projectile was created
            if (_projectileCreatedCallback && projectileId != 0) {
                _projectileCreatedCallback(projectileId, entityId, pos.x, pos.y, dir.x, dir.y, 300.0f,
                                           static_cast<int>(weapon.getDamage()), true);
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
        Velocity projectileVelocity =
            calculateProjectileVelocity(registry, ownerId, 300.0F);  // Base speed: 300 units/sec

        auto projectileId = registry.newEntity();

        registry.setComponent(projectileId, projectileTransform);
        registry.setComponent(projectileId, projectileVelocity);
        registry.setComponent(projectileId, Projectile(weapon.getDamage(), 10.0f, ownerId, isFriendly));

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
            LOG_DEBUG("[WEAPON] Projectile fired: ID=", projectileId, " Pos=(", pos.x, ",", pos.y, ") Dir=(",
                      dir.x, ",", dir.y, ") Speed=300.0");
            _projectileCreatedCallback(projectileId, ownerId, pos.x, pos.y, dir.x, dir.y, 300.0f,
                                       static_cast<int>(weapon.getDamage()), isFriendly);
        }

        return projectileId;
    }

    Velocity WeaponSystem::calculateProjectileVelocity(Registry &registry, std::uint32_t ownerId,
                                                       float baseSpeed) {
        return Velocity(1.0F, 0.0F, baseSpeed);
    }

    Transform WeaponSystem::calculateProjectileTransform(Registry &registry, std::uint32_t ownerId) {
        // Get owner's position
        Transform defaultTransform(0.0F, 0.0F);

        if (registry.hasComponent<Transform>(ownerId)) {
            auto &ownerTransform = registry.getComponent<Transform>(ownerId);
            auto ownerPos = ownerTransform.getPosition();

            // Projectile spawns at the same position as the owner
            return Transform(ownerPos.x, ownerPos.y);
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
        Velocity projectileVelocity = calculateProjectileVelocity(registry, ownerId, chargedSpeed);

        // Create projectile entity
        auto projectileId = registry.newEntity();

        // Set components with charge-boosted damage
        registry.setComponent(projectileId, projectileTransform);
        registry.setComponent(projectileId, projectileVelocity);
        float chargedDamage = weapon.getDamage() * damageMultiplier;
        registry.setComponent(projectileId, Projectile(chargedDamage, 10.0F, ownerId, isFriendly));

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
