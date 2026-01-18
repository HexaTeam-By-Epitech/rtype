/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** CollisionSystem
*/

#include "CollisionSystem.hpp"
#include "../../Components/Enemy.hpp"
#include "../../Components/IComponent.hpp"
#include "../../Components/OrbitalModule.hpp"
#include "../../Components/Projectile.hpp"
#include "../../Components/Wall.hpp"
#include "common/Logger/Logger.hpp"

namespace ecs {
    /**
     * @brief Performs collision detection between all collidable entities.
     */
    void CollisionSystem::update(Registry &registry, [[maybe_unused]] float deltaTime) {
        auto entities = registry.getEntitiesWithMask(this->getComponentMask());
        std::vector<std::uint32_t> entitiesVec(entities.begin(), entities.end());
        std::vector<std::uint32_t> projectilesToDestroy;

        // Collect entities to destroy after collision processing
        std::vector<Address> entitiesToDestroy;

        for (size_t i = 0; i < entitiesVec.size(); ++i) {
            for (size_t j = i + 1; j < entitiesVec.size(); ++j) {
                auto entity1 = entitiesVec[i];
                auto entity2 = entitiesVec[j];

                // Check if entities still exist (might have been marked for destruction)
                if (!registry.hasComponent<Transform>(entity1) ||
                    !registry.hasComponent<Transform>(entity2)) {
                    continue;
                }

                auto &transform1 = registry.getComponent<Transform>(entity1);
                auto &collider1 = registry.getComponent<Collider>(entity1);
                auto &transform2 = registry.getComponent<Transform>(entity2);
                auto &collider2 = registry.getComponent<Collider>(entity2);

                if (!canCollide(collider1.getLayer(), collider1.getMask(), collider2.getLayer(),
                                collider2.getMask())) {
                    continue;
                }

                if (checkAABB(transform1.getPosition(), collider1.getSize(), collider1.getOffset(),
                              transform2.getPosition(), collider2.getSize(), collider2.getOffset())) {
                    // Check for wall collisions
                    bool entity1IsWall = registry.hasComponent<Wall>(entity1);
                    bool entity2IsWall = registry.hasComponent<Wall>(entity2);
                    bool entity1IsPlayer = registry.hasComponent<Player>(entity1);
                    bool entity2IsPlayer = registry.hasComponent<Player>(entity2);
                    bool entity1IsCollectible = registry.hasComponent<Collectible>(entity1);
                    bool entity2IsCollectible = registry.hasComponent<Collectible>(entity2);
                    bool entity1IsOrbitalModule = registry.hasComponent<OrbitalModule>(entity1);
                    bool entity2IsOrbitalModule = registry.hasComponent<OrbitalModule>(entity2);
                    bool entity1IsEnemy = registry.hasComponent<Enemy>(entity1);
                    bool entity2IsEnemy = registry.hasComponent<Enemy>(entity2);
                    bool entity1IsProjectile = registry.hasComponent<Projectile>(entity1);
                    bool entity2IsProjectile = registry.hasComponent<Projectile>(entity2);

                    // Handle Player-Wall collision: block player movement
                    if (entity1IsPlayer && entity2IsWall) {
                        resolveWallCollision(entity1, entity2, transform1, collider1, transform2, collider2,
                                             registry);
                    } else if (entity2IsPlayer && entity1IsWall) {
                        resolveWallCollision(entity2, entity1, transform2, collider2, transform1, collider1,
                                             registry);
                    }

                    // Handle Orbital Module - Enemy collision (module damages enemy)
                    if (entity1IsOrbitalModule && entity2IsEnemy) {
                        handleModuleEnemyCollision(entity1, entity2, registry);
                    } else if (entity2IsOrbitalModule && entity1IsEnemy) {
                        handleModuleEnemyCollision(entity2, entity1, registry);
                    }

                    // Handle Orbital Module - Enemy Projectile collision (module blocks projectile)
                    if (entity1IsOrbitalModule && entity2IsProjectile) {
                        handleModuleProjectileCollision(entity1, entity2, registry, entitiesToDestroy);
                    } else if (entity2IsOrbitalModule && entity1IsProjectile) {
                        handleModuleProjectileCollision(entity2, entity1, registry, entitiesToDestroy);
                    }

                    // Handle player-collectible pickup
                    if ((entity1IsPlayer && entity2IsCollectible) ||
                        (entity2IsPlayer && entity1IsCollectible)) {
                        LOG_DEBUG("[COLLISION] Player-Collectible collision detected: E", entity1, " & E",
                                  entity2);
                    }

                    if (entity1IsPlayer && entity2IsCollectible) {
                        handlePickup(entity1, entity2, registry, entitiesToDestroy);
                    } else if (entity2IsPlayer && entity1IsCollectible) {
                        handlePickup(entity2, entity1, registry, entitiesToDestroy);
                    }
                }
            }
        }

        // Destroy collected entities after all collision processing
        for (Address addr : entitiesToDestroy) {
            if (registry.hasComponent<Transform>(addr)) {  // Check if still exists
                registry.destroyEntity(addr);
            }
        }
    }

    void CollisionSystem::handlePickup(Address playerAddr, Address collectibleAddr, Registry &registry,
                                       std::vector<Address> &entitiesToDestroy) {
        // Verify both entities still exist
        if (!registry.hasComponent<Collectible>(collectibleAddr)) {
            return;
        }

        if (!registry.hasComponent<Player>(playerAddr)) {
            return;
        }

        try {
            Collectible &collectible = registry.getComponent<Collectible>(collectibleAddr);

            LOG_INFO("[PICKUP] Player ", playerAddr, " collected item at entity ", collectibleAddr);

            // Apply collectible effects
            if (collectible.grantsBuff()) {
                // Add buff to player
                if (!registry.hasComponent<Buff>(playerAddr)) {
                    registry.setComponent<Buff>(playerAddr, Buff());
                }

                Buff &buff = registry.getComponent<Buff>(playerAddr);
                buff.addBuff(collectible.getBuffType(), collectible.getDuration(), collectible.getValue());

                // Log the buff type
                const char *buffName = "Unknown";
                switch (collectible.getBuffType()) {
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
                    case BuffType::MultiShot:
                        buffName = "MultiShot";
                        break;
                    case BuffType::DoubleShot:
                        buffName = "DoubleShot";
                        break;
                    case BuffType::TripleShot:
                        buffName = "TripleShot";
                        break;
                    case BuffType::PiercingShot:
                        buffName = "PiercingShot";
                        break;
                    case BuffType::HomingShot:
                        buffName = "HomingShot";
                        break;
                    case BuffType::MaxHealthIncrease:
                        buffName = "MaxHealthIncrease";
                        break;
                }
                float duration = collectible.getDuration();
                if (duration > 0.0f) {
                    LOG_INFO("  ✓ Applied buff: ", buffName, " (duration: ", duration,
                             "s, value: ", collectible.getValue(), ")");
                } else {
                    LOG_INFO("  ✓ Applied PERMANENT upgrade: ", buffName, " (value: ", collectible.getValue(),
                             ")");
                }

                // For permanent max health increase, apply immediately
                if (collectible.getBuffType() == BuffType::MaxHealthIncrease) {
                    if (registry.hasComponent<Health>(playerAddr)) {
                        Health &health = registry.getComponent<Health>(playerAddr);
                        int increase = static_cast<int>(collectible.getValue());
                        health.setMaxHealth(health.getMaxHealth() + increase);
                        health.setCurrentHealth(health.getCurrentHealth() + increase);
                    }
                }
            }

            if (collectible.restoresHealth()) {
                if (registry.hasComponent<Health>(playerAddr)) {
                    Health &health = registry.getComponent<Health>(playerAddr);
                    int oldHealth = health.getCurrentHealth();
                    int newHealth = oldHealth + collectible.getHealthRestore();
                    health.setCurrentHealth(std::min(newHealth, health.getMaxHealth()));
                    LOG_INFO("  ✓ Restored health: ", oldHealth, " -> ", health.getCurrentHealth(), " (+",
                             collectible.getHealthRestore(), ")");
                }
            }

            if (collectible.awardsScore()) {
                if (registry.hasComponent<Player>(playerAddr)) {
                    Player &player = registry.getComponent<Player>(playerAddr);
                    int oldScore = player.getScore();
                    player.setScore(oldScore + collectible.getScoreValue());
                    LOG_INFO("  ✓ Awarded score: ", oldScore, " -> ", player.getScore(), " (+",
                             collectible.getScoreValue(), ")");
                }
            }

            // Mark collectible for destruction (will be destroyed after collision loop)
            entitiesToDestroy.push_back(collectibleAddr);
        } catch (const std::exception &e) {
            // Entity might have been destroyed during collision processing
            // This is not critical, just skip this pickup
            return;
        }
    }

    void CollisionSystem::handleModuleEnemyCollision(Address moduleAddr, Address enemyAddr,
                                                     Registry &registry) {
        // Get module damage value
        if (!registry.hasComponent<OrbitalModule>(moduleAddr)) {
            return;
        }

        const OrbitalModule &module = registry.getComponent<OrbitalModule>(moduleAddr);
        int damage = module.getDamage();

        // Apply damage to enemy
        if (registry.hasComponent<Health>(enemyAddr)) {
            Health &enemyHealth = registry.getComponent<Health>(enemyAddr);
            int oldHealth = enemyHealth.getCurrentHealth();
            enemyHealth.setCurrentHealth(oldHealth - damage);

            LOG_DEBUG("[COLLISION] Orbital module E", moduleAddr, " hit enemy E", enemyAddr, " for ", damage,
                      " damage (", oldHealth, " -> ", enemyHealth.getCurrentHealth(), ")");

            // If enemy is dead, it will be handled by HealthSystem
        }
    }

    void CollisionSystem::handleModuleProjectileCollision(Address moduleAddr, Address projectileAddr,
                                                          Registry &registry,
                                                          std::vector<Address> &entitiesToDestroy) {
        // Check if projectile is from enemy
        if (!registry.hasComponent<Projectile>(projectileAddr)) {
            return;
        }

        const Projectile &projectile = registry.getComponent<Projectile>(projectileAddr);

        // Only block enemy projectiles
        if (!projectile.isFriendly()) {
            LOG_DEBUG("[COLLISION] Orbital module E", moduleAddr, " blocked enemy projectile E",
                      projectileAddr);

            // Destroy the projectile
            entitiesToDestroy.push_back(projectileAddr);
        }
    }

    /**
     * @brief Performs AABB (Axis-Aligned Bounding Box) collision test.
     */
    bool CollisionSystem::checkAABB(const Transform::Vector2 &pos1, const Collider::Vector2 &size1,
                                    const Collider::Vector2 &offset1, const Transform::Vector2 &pos2,
                                    const Collider::Vector2 &size2, const Collider::Vector2 &offset2) const {

        // Position is the CENTER of the entity, so we need to offset by half the size
        float left1 = pos1.x + offset1.x - (size1.x / 2.0f);
        float right1 = left1 + size1.x;
        float top1 = pos1.y + offset1.y - (size1.y / 2.0f);
        float bottom1 = top1 + size1.y;

        float left2 = pos2.x + offset2.x - (size2.x / 2.0f);
        float right2 = left2 + size2.x;
        float top2 = pos2.y + offset2.y - (size2.y / 2.0f);
        float bottom2 = top2 + size2.y;

        return !(right1 < left2 || left1 > right2 || bottom1 < top2 || top1 > bottom2);
    }

    /**
     * @brief Resolves player-wall collision by instantly killing the player.
     */
    void CollisionSystem::resolveWallCollision(Address playerAddr, Address wallAddr,
                                               Transform &playerTransform, Collider &playerCollider,
                                               Transform &wallTransform, Collider &wallCollider,
                                               Registry &registry) {
        (void)wallAddr;
        (void)playerTransform;
        (void)playerCollider;
        (void)wallTransform;
        (void)wallCollider;

        // Walls are instant death - deal massive damage to kill player
        LOG_INFO("[COLLISION] Player touched wall - instant death!");

        // Deal 9999 damage to ensure instant death
        if (registry.hasComponent<Health>(playerAddr)) {
            Health &health = registry.getComponent<Health>(playerAddr);
            health.setCurrentHealth(0);
        }
    }

    /**
     * @brief Checks if two entities can collide based on layer masks.
     */
    bool CollisionSystem::canCollide(std::uint32_t layer1, std::uint32_t mask1, std::uint32_t layer2,
                                     std::uint32_t mask2) const {
        return (mask1 & layer2) && (mask2 & layer1);
    }

    ComponentMask CollisionSystem::getComponentMask() const {
        return (1ULL << getComponentType<Transform>()) | (1ULL << getComponentType<Collider>());
    }
}  // namespace ecs
