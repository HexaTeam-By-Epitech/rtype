/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** SystemsTest
*/

#include <gtest/gtest.h>

#include "Components/Health.hpp"
#include "Components/Projectile.hpp"
#include "Components/Transform.hpp"
#include "Components/Velocity.hpp"
#include "Components/Weapon.hpp"
#include "Registry.hpp"
#include "Systems/BoundarySystem/BoundarySystem.hpp"
#include "Systems/HealthSystem/HealthSystem.hpp"
#include "Systems/MovementSystem/MovementSystem.hpp"
#include "Systems/WeaponSystem/WeaponSystem.hpp"

// ========== MovementSystem Tests ==========

TEST(MovementSystemTest, EntitiesWithoutVelocityNotAffected) {
    ecs::Registry registry;
    ecs::MovementSystem moveSystem;

    auto entity = registry.newEntity();
    registry.setComponent(entity, ecs::Transform(100.0f, 200.0f));

    moveSystem.update(registry, 0.016f);

    auto &transform = registry.getComponent<ecs::Transform>(entity);
    auto pos = transform.getPosition();
    EXPECT_FLOAT_EQ(pos.x, 100.0f);
    EXPECT_FLOAT_EQ(pos.y, 200.0f);
}

TEST(MovementSystemTest, BasicMovement) {
    ecs::Registry registry;
    ecs::MovementSystem moveSystem;

    auto entity = registry.newEntity();
    registry.setComponent(entity, ecs::Transform(0.0f, 0.0f));
    registry.setComponent(entity, ecs::Velocity(1.0f, 0.0f, 100.0f));  // Moving right at 100 units/s

    moveSystem.update(registry, 0.1f);  // 0.1 second

    auto &transform = registry.getComponent<ecs::Transform>(entity);
    auto pos = transform.getPosition();
    EXPECT_FLOAT_EQ(pos.x, 10.0f);  // 1.0 * 100.0 * 0.1 = 10.0
    EXPECT_FLOAT_EQ(pos.y, 0.0f);
}

TEST(MovementSystemTest, DiagonalMovement) {
    ecs::Registry registry;
    ecs::MovementSystem moveSystem;

    auto entity = registry.newEntity();
    registry.setComponent(entity, ecs::Transform(0.0f, 0.0f));
    // Normalized diagonal direction (0.707, 0.707) at 100 units/s
    registry.setComponent(entity, ecs::Velocity(0.707f, 0.707f, 100.0f));

    moveSystem.update(registry, 1.0f);  // 1 second

    auto &transform = registry.getComponent<ecs::Transform>(entity);
    auto pos = transform.getPosition();
    EXPECT_NEAR(pos.x, 70.7f, 0.1f);  // 0.707 * 100.0 * 1.0
    EXPECT_NEAR(pos.y, 70.7f, 0.1f);
}

TEST(MovementSystemTest, FrameRateIndependent) {
    ecs::Registry registry1, registry2;
    ecs::MovementSystem moveSystem;

    // Entity 1: 60 FPS (many small updates)
    auto entity1 = registry1.newEntity();
    registry1.setComponent(entity1, ecs::Transform(0.0f, 0.0f));
    registry1.setComponent(entity1, ecs::Velocity(1.0f, 0.0f, 100.0f));

    // Entity 2: 30 FPS (fewer larger updates)
    auto entity2 = registry2.newEntity();
    registry2.setComponent(entity2, ecs::Transform(0.0f, 0.0f));
    registry2.setComponent(entity2, ecs::Velocity(1.0f, 0.0f, 100.0f));

    // Simulate 1 second at 60 FPS
    for (int i = 0; i < 60; i++) {
        moveSystem.update(registry1, 1.0f / 60.0f);
    }

    // Simulate 1 second at 30 FPS
    for (int i = 0; i < 30; i++) {
        moveSystem.update(registry2, 1.0f / 30.0f);
    }

    auto &transform1 = registry1.getComponent<ecs::Transform>(entity1);
    auto &transform2 = registry2.getComponent<ecs::Transform>(entity2);

    EXPECT_NEAR(transform1.getPosition().x, transform2.getPosition().x, 0.01f);
}

// ========== HealthSystem Tests ==========

TEST(HealthSystemTest, InvincibilityTimerDecreases) {
    ecs::Registry registry;
    ecs::HealthSystem healthSystem;

    auto entity = registry.newEntity();
    ecs::Health health(100);
    health.setInvincible(true);
    health.setInvincibilityTimer(2.0f);  // 2 seconds
    registry.setComponent(entity, health);

    healthSystem.update(registry, 0.5f);

    auto &updatedHealth = registry.getComponent<ecs::Health>(entity);
    EXPECT_FLOAT_EQ(updatedHealth.getInvincibilityTimer(), 1.5f);
    EXPECT_TRUE(updatedHealth.isInvincible());
}

TEST(HealthSystemTest, InvincibilityEndsWhenTimerReachesZero) {
    ecs::Registry registry;
    ecs::HealthSystem healthSystem;

    auto entity = registry.newEntity();
    ecs::Health health(100);
    health.setInvincible(true);
    health.setInvincibilityTimer(0.5f);
    registry.setComponent(entity, health);

    healthSystem.update(registry, 1.0f);  // Timer goes negative

    auto &updatedHealth = registry.getComponent<ecs::Health>(entity);
    EXPECT_FLOAT_EQ(updatedHealth.getInvincibilityTimer(), 0.0f);
    EXPECT_FALSE(updatedHealth.isInvincible());
}

TEST(HealthSystemTest, DeadEntitiesAreDestroyed) {
    ecs::Registry registry;
    ecs::HealthSystem healthSystem;

    auto entity = registry.newEntity();
    ecs::Health health(0);  // Dead entity
    registry.setComponent(entity, health);

    healthSystem.update(registry, 0.016f);

    // Entity should be destroyed
    EXPECT_FALSE(registry.hasComponent<ecs::Health>(entity));
}

TEST(HealthSystemTest, HealthyEntitiesSurvive) {
    ecs::Registry registry;
    ecs::HealthSystem healthSystem;

    auto entity = registry.newEntity();
    registry.setComponent(entity, ecs::Health(100));

    healthSystem.update(registry, 0.016f);

    EXPECT_TRUE(registry.hasComponent<ecs::Health>(entity));
}

// ========== WeaponSystem Tests ==========

TEST(WeaponSystemTest, CooldownDecreases) {
    ecs::Registry registry;
    ecs::WeaponSystem weaponSystem;

    auto entity = registry.newEntity();
    registry.setComponent(entity, ecs::Transform(0.0f, 0.0f));
    registry.setComponent(entity, ecs::Weapon(5.0f, 1.0f, 0, 10));  // 1 second cooldown

    weaponSystem.update(registry, 0.3f);

    auto &weapon = registry.getComponent<ecs::Weapon>(entity);
    EXPECT_FLOAT_EQ(weapon.getCooldown(), 0.7f);
}

TEST(WeaponSystemTest, CooldownDoesNotGoNegative) {
    ecs::Registry registry;
    ecs::WeaponSystem weaponSystem;

    auto entity = registry.newEntity();
    registry.setComponent(entity, ecs::Transform(0.0f, 0.0f));
    registry.setComponent(entity, ecs::Weapon(5.0f, 0.5f, 0, 10));

    weaponSystem.update(registry, 1.0f);

    auto &weapon = registry.getComponent<ecs::Weapon>(entity);
    EXPECT_LE(weapon.getCooldown(), 0.0f);
}

TEST(WeaponSystemTest, NoCooldownWhenZero) {
    ecs::Registry registry;
    ecs::WeaponSystem weaponSystem;

    auto entity = registry.newEntity();
    registry.setComponent(entity, ecs::Transform(0.0f, 0.0f));
    registry.setComponent(entity, ecs::Weapon(5.0f, 0.0f, 0, 10));  // No cooldown

    weaponSystem.update(registry, 0.5f);

    auto &weapon = registry.getComponent<ecs::Weapon>(entity);
    EXPECT_FLOAT_EQ(weapon.getCooldown(), 0.0f);
}

// ========== BoundarySystem Tests ==========

TEST(BoundarySystemTest, EntitiesWithinBoundaryNotDestroyed) {
    ecs::Registry registry;
    ecs::BoundarySystem boundarySystem(800, 600);  // 800x600 screen

    auto entity = registry.newEntity();
    registry.setComponent(entity, ecs::Transform(400.0f, 300.0f));  // Center

    boundarySystem.update(registry, 0.016f);

    EXPECT_TRUE(registry.hasComponent<ecs::Transform>(entity));
}

TEST(BoundarySystemTest, EntitiesOutOfBoundsDestroyed) {
    ecs::Registry registry;
    ecs::BoundarySystem boundarySystem(800, 600);

    auto entity = registry.newEntity();
    registry.setComponent(entity, ecs::Transform(-200.0f, 300.0f));  // Far left

    boundarySystem.update(registry, 0.016f);

    EXPECT_FALSE(registry.hasComponent<ecs::Transform>(entity));
}

TEST(BoundarySystemTest, MarginAllowed) {
    ecs::Registry registry;
    ecs::BoundarySystem boundarySystem(800, 600);

    auto entity = registry.newEntity();
    registry.setComponent(entity, ecs::Transform(-50.0f, 300.0f));  // Within -100 margin

    boundarySystem.update(registry, 0.016f);

    EXPECT_TRUE(registry.hasComponent<ecs::Transform>(entity));
}

TEST(BoundarySystemTest, AllDirectionsBoundaries) {
    ecs::Registry registry;
    ecs::BoundarySystem boundarySystem(800, 600);

    auto left = registry.newEntity();
    registry.setComponent(left, ecs::Transform(-150.0f, 300.0f));

    auto right = registry.newEntity();
    registry.setComponent(right, ecs::Transform(950.0f, 300.0f));

    auto top = registry.newEntity();
    registry.setComponent(top, ecs::Transform(400.0f, -150.0f));

    auto bottom = registry.newEntity();
    registry.setComponent(bottom, ecs::Transform(400.0f, 750.0f));

    boundarySystem.update(registry, 0.016f);

    EXPECT_FALSE(registry.hasComponent<ecs::Transform>(left));
    EXPECT_FALSE(registry.hasComponent<ecs::Transform>(right));
    EXPECT_FALSE(registry.hasComponent<ecs::Transform>(top));
    EXPECT_FALSE(registry.hasComponent<ecs::Transform>(bottom));
}

// ========== Integration Tests ==========

TEST(SystemsIntegrationTest, MovementAndBoundary) {
    ecs::Registry registry;
    ecs::MovementSystem moveSystem;
    ecs::BoundarySystem boundarySystem(800, 600);

    auto entity = registry.newEntity();
    registry.setComponent(entity, ecs::Transform(750.0f, 300.0f));
    registry.setComponent(entity, ecs::Velocity(1.0f, 0.0f, 200.0f));  // Moving right fast

    // Move for 1 second - should go out of bounds
    for (int i = 0; i < 60; i++) {
        moveSystem.update(registry, 1.0f / 60.0f);
        boundarySystem.update(registry, 1.0f / 60.0f);
    }

    EXPECT_FALSE(registry.hasComponent<ecs::Transform>(entity));
}

TEST(SystemsIntegrationTest, ProjectileMovementAndLifetime) {
    ecs::Registry registry;
    ecs::MovementSystem moveSystem;

    auto projectile = registry.newEntity();
    registry.setComponent(projectile, ecs::Transform(100.0f, 100.0f));
    registry.setComponent(projectile, ecs::Velocity(1.0f, 0.0f, 500.0f));
    registry.setComponent(projectile, ecs::Projectile(10, 2.0f, 1, true));

    // Update for 3 seconds
    for (int i = 0; i < 180; i++) {
        moveSystem.update(registry, 1.0f / 60.0f);
    }

    // Projectile should be destroyed after lifetime expires
    EXPECT_FALSE(registry.hasComponent<ecs::Projectile>(projectile));
}
