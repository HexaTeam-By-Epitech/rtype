/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** WeaponSystem
*/

#include "WeaponSystem.hpp"
#include "../../Components/IComponent.hpp"

namespace ecs {
    /**
     * @brief Updates weapon cooldowns for all entities with weapons.
     */
    void WeaponSystem::update(Registry &registry, float deltaTime) {
        auto entities = registry.getEntitiesWithMask(this->getComponentMask());

        for (auto entityId : entities) {
            auto &weapon = registry.getComponent<Weapon>(entityId);

            float cooldown = weapon.getCooldown();
            if (cooldown > 0.0F) {
                weapon.setCooldown(cooldown - deltaTime);
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

        return projectileId;
    }

    Velocity WeaponSystem::calculateProjectileVelocity(Registry &registry, std::uint32_t ownerId,
                                                       float baseSpeed) {
        // Default: projectile moves right (standard R-Type behavior)
        Velocity defaultVelocity(1.0F, 0.0F, baseSpeed);

        // If owner has velocity, inherit its direction
        if (registry.hasComponent<Velocity>(ownerId)) {
            auto &ownerVelocity = registry.getComponent<Velocity>(ownerId);
            auto ownerDir = ownerVelocity.getDirection();
            return Velocity(ownerDir.x, ownerDir.y, baseSpeed);
        }

        return defaultVelocity;
    }

    Transform WeaponSystem::calculateProjectileTransform(Registry &registry, std::uint32_t ownerId) {
        // Get owner's position
        Transform defaultTransform(0.0F, 0.0F);

        if (registry.hasComponent<Transform>(ownerId)) {
            auto &ownerTransform = registry.getComponent<Transform>(ownerId);
            auto ownerPos = ownerTransform.getPosition();

            // Offset projectile slightly from owner to avoid immediate collision
            float offsetX = 10.0F;  // Small offset in front of the shooter
            return Transform(ownerPos.x + offsetX, ownerPos.y);
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
        int chargedDamage = static_cast<int>(weapon.getDamage() * damageMultiplier);
        registry.setComponent(projectileId, Projectile(chargedDamage, 10.0F, ownerId, isFriendly));

        // Reset weapon cooldown
        float fireRate = weapon.getFireRate();
        if (fireRate > 0.0F) {
            weapon.setCooldown(1.0F / fireRate);
        }

        return projectileId;
    }

    ComponentMask WeaponSystem::getComponentMask() const {
        return (1ULL << getComponentType<Weapon>()) | (1ULL << getComponentType<Transform>());
    }
}  // namespace ecs
