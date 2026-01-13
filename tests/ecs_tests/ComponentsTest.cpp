/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** ComponentsTest
*/

#include <gtest/gtest.h>
#include "../../common/ECS/Components/Collider.hpp"
#include "../../common/ECS/Components/Enemy.hpp"
#include "../../common/ECS/Components/Health.hpp"
#include "../../common/ECS/Components/Player.hpp"
#include "../../common/ECS/Components/Projectile.hpp"
#include "../../common/ECS/Components/Transform.hpp"
#include "../../common/ECS/Components/Velocity.hpp"
#include "../../common/ECS/Components/Weapon.hpp"

// ========================================
// Transform Component Tests
// ========================================
TEST(TransformTest, ConstructorInitializesCorrectly) {
    ecs::Transform transform(100, 200, 45, 2, 2);

    EXPECT_EQ(transform.getPosition().x, 100);
    EXPECT_EQ(transform.getPosition().y, 200);
    EXPECT_EQ(transform.getRotation(), 45);
    EXPECT_EQ(transform.getScale().x, 2);
    EXPECT_EQ(transform.getScale().y, 2);
}

TEST(TransformTest, SettersWorkCorrectly) {
    ecs::Transform transform(0, 0, 0, 1, 1);

    transform.setPosition(50, 75);
    transform.setRotation(90);
    transform.setScale(3, 4);

    EXPECT_EQ(transform.getPosition().x, 50);
    EXPECT_EQ(transform.getPosition().y, 75);
    EXPECT_EQ(transform.getRotation(), 90);
    EXPECT_EQ(transform.getScale().x, 3);
    EXPECT_EQ(transform.getScale().y, 4);
}

TEST(TransformTest, HasUniqueComponentType) {
    ecs::Transform transform(0, 0, 0, 1, 1);
    ecs::ComponentType type = transform.getType();

    EXPECT_EQ(type, ecs::getComponentType<ecs::Transform>());
}

// ========================================
// Velocity Component Tests
// ========================================
TEST(VelocityTest, ConstructorInitializesCorrectly) {
    ecs::Velocity velocity(1, 0, 5);

    EXPECT_EQ(velocity.getDirection().x, 1);
    EXPECT_EQ(velocity.getDirection().y, 0);
    EXPECT_EQ(velocity.getSpeed(), 5);
}

TEST(VelocityTest, SettersWorkCorrectly) {
    ecs::Velocity velocity(0, 0, 0);

    velocity.setDirection(1, 1);
    velocity.setSpeed(10);

    EXPECT_EQ(velocity.getDirection().x, 1);
    EXPECT_EQ(velocity.getDirection().y, 1);
    EXPECT_EQ(velocity.getSpeed(), 10);
}

TEST(VelocityTest, HasUniqueComponentType) {
    ecs::Velocity velocity(0, 0, 0);

    EXPECT_EQ(velocity.getType(), ecs::getComponentType<ecs::Velocity>());
}

// ========================================
// Health Component Tests
// ========================================
TEST(HealthTest, ConstructorWithMaxHealthOnly) {
    ecs::Health health(100);

    EXPECT_EQ(health.getCurrentHealth(), 100);
    EXPECT_EQ(health.getMaxHealth(), 100);
    EXPECT_FALSE(health.isInvincible());
    EXPECT_EQ(health.getInvincibilityTimer(), 0);
}

TEST(HealthTest, ConstructorWithCurrentAndMaxHealth) {
    ecs::Health health(50, 100);

    EXPECT_EQ(health.getCurrentHealth(), 50);
    EXPECT_EQ(health.getMaxHealth(), 100);
}

TEST(HealthTest, SettersWorkCorrectly) {
    ecs::Health health(100);

    health.setCurrentHealth(75);
    health.setMaxHealth(150);
    health.setInvincible(true);
    health.setInvincibilityTimer(60);

    EXPECT_EQ(health.getCurrentHealth(), 75);
    EXPECT_EQ(health.getMaxHealth(), 150);
    EXPECT_TRUE(health.isInvincible());
    EXPECT_EQ(health.getInvincibilityTimer(), 60);
}

TEST(HealthTest, HasUniqueComponentType) {
    ecs::Health health(100);

    EXPECT_EQ(health.getType(), ecs::getComponentType<ecs::Health>());
}

// ========================================
// Player Component Tests
// ========================================
TEST(PlayerTest, ConstructorInitializesCorrectly) {
    ecs::Player player(1000, 3, 1);

    EXPECT_EQ(player.getScore(), 1000);
    EXPECT_EQ(player.getLives(), 3);
    EXPECT_EQ(player.getPlayerId(), 1);
}

TEST(PlayerTest, SettersWorkCorrectly) {
    ecs::Player player(0, 0, 0);

    player.setScore(5000);
    player.setLives(5);
    player.setPlayerId(42);

    EXPECT_EQ(player.getScore(), 5000);
    EXPECT_EQ(player.getLives(), 5);
    EXPECT_EQ(player.getPlayerId(), 42);
}

TEST(PlayerTest, HasUniqueComponentType) {
    ecs::Player player(0, 0, 0);

    EXPECT_EQ(player.getType(), ecs::getComponentType<ecs::Player>());
}

// ========================================
// Enemy Component Tests
// ========================================
TEST(EnemyTest, ConstructorInitializesCorrectly) {
    ecs::Enemy enemy(1, 500);

    EXPECT_EQ(enemy.getEnemyType(), 1);
    EXPECT_EQ(enemy.getScoreValue(), 500);
}

TEST(EnemyTest, SettersWorkCorrectly) {
    ecs::Enemy enemy(0, 0);

    enemy.setEnemyType(5);
    enemy.setScoreValue(1000);

    EXPECT_EQ(enemy.getEnemyType(), 5);
    EXPECT_EQ(enemy.getScoreValue(), 1000);
}

TEST(EnemyTest, HasUniqueComponentType) {
    ecs::Enemy enemy(0, 0);

    EXPECT_EQ(enemy.getType(), ecs::getComponentType<ecs::Enemy>());
}

// ========================================
// Projectile Component Tests
// ========================================
TEST(ProjectileTest, ConstructorInitializesCorrectly) {
    ecs::Projectile projectile(10, 120, 42, true);

    EXPECT_EQ(projectile.getDamage(), 10);
    EXPECT_EQ(projectile.getLifetime(), 120);
    EXPECT_EQ(projectile.getOwnerId(), 42);
    EXPECT_TRUE(projectile.isFriendly());
}

TEST(ProjectileTest, SettersWorkCorrectly) {
    ecs::Projectile projectile(0, 0, 0, false);

    projectile.setDamage(25);
    projectile.setLifetime(60);
    projectile.setOwnerId(99);
    projectile.setFriendly(true);

    EXPECT_EQ(projectile.getDamage(), 25);
    EXPECT_EQ(projectile.getLifetime(), 60);
    EXPECT_EQ(projectile.getOwnerId(), 99);
    EXPECT_TRUE(projectile.isFriendly());
}

TEST(ProjectileTest, HasUniqueComponentType) {
    ecs::Projectile projectile(0, 0, 0, false);

    EXPECT_EQ(projectile.getType(), ecs::getComponentType<ecs::Projectile>());
}

// ========================================
// Weapon Component Tests
// ========================================
TEST(WeaponTest, ConstructorInitializesCorrectly) {
    ecs::Weapon weapon(10, 30, 1, 15);

    EXPECT_EQ(weapon.getFireRate(), 10);
    EXPECT_EQ(weapon.getCooldown(), 30);
    EXPECT_EQ(weapon.getProjectileType(), 1);
    EXPECT_EQ(weapon.getDamage(), 15);
}

TEST(WeaponTest, SettersWorkCorrectly) {
    ecs::Weapon weapon(0, 0, 0, 0);

    weapon.setFireRate(20);
    weapon.setCooldown(15);
    weapon.setProjectileType(2);
    weapon.setDamage(50);

    EXPECT_EQ(weapon.getFireRate(), 20);
    EXPECT_EQ(weapon.getCooldown(), 15);
    EXPECT_EQ(weapon.getProjectileType(), 2);
    EXPECT_EQ(weapon.getDamage(), 50);
}

TEST(WeaponTest, HasUniqueComponentType) {
    ecs::Weapon weapon(0, 0, 0, 0);

    EXPECT_EQ(weapon.getType(), ecs::getComponentType<ecs::Weapon>());
}

// ========================================
// Collider Component Tests
// ========================================
TEST(ColliderTest, ConstructorInitializesCorrectly) {
    ecs::Collider collider(32, 48, 0, 0, 1, 0xFF, false);

    EXPECT_EQ(collider.getSize().x, 32);
    EXPECT_EQ(collider.getSize().y, 48);
    EXPECT_EQ(collider.getOffset().x, 0);
    EXPECT_EQ(collider.getOffset().y, 0);
    EXPECT_EQ(collider.getLayer(), 1);
    EXPECT_EQ(collider.getMask(), 0xFF);
    EXPECT_FALSE(collider.isTrigger());
}

TEST(ColliderTest, SettersWorkCorrectly) {
    ecs::Collider collider(0, 0, 0, 0, 0, 0, false);

    collider.setSize(64, 64);
    collider.setOffset(8, 8);
    collider.setLayer(2);
    collider.setMask(0xF0);
    collider.setTrigger(true);

    EXPECT_EQ(collider.getSize().x, 64);
    EXPECT_EQ(collider.getSize().y, 64);
    EXPECT_EQ(collider.getOffset().x, 8);
    EXPECT_EQ(collider.getOffset().y, 8);
    EXPECT_EQ(collider.getLayer(), 2);
    EXPECT_EQ(collider.getMask(), 0xF0);
    EXPECT_TRUE(collider.isTrigger());
}

TEST(ColliderTest, HasUniqueComponentType) {
    ecs::Collider collider(0, 0, 0, 0, 0, 0, false);

    EXPECT_EQ(collider.getType(), ecs::getComponentType<ecs::Collider>());
}

// ========================================
// Component Type Uniqueness Test
// ========================================
TEST(AllComponentsTest, AllComponentTypesAreUnique) {
    ecs::ComponentType transformType = ecs::getComponentType<ecs::Transform>();
    ecs::ComponentType velocityType = ecs::getComponentType<ecs::Velocity>();
    ecs::ComponentType healthType = ecs::getComponentType<ecs::Health>();
    ecs::ComponentType playerType = ecs::getComponentType<ecs::Player>();
    ecs::ComponentType enemyType = ecs::getComponentType<ecs::Enemy>();
    ecs::ComponentType projectileType = ecs::getComponentType<ecs::Projectile>();
    ecs::ComponentType weaponType = ecs::getComponentType<ecs::Weapon>();
    ecs::ComponentType colliderType = ecs::getComponentType<ecs::Collider>();

    // Verify all component types are unique
    EXPECT_NE(transformType, velocityType);
    EXPECT_NE(transformType, healthType);
    EXPECT_NE(transformType, playerType);
    EXPECT_NE(transformType, enemyType);
    EXPECT_NE(transformType, projectileType);
    EXPECT_NE(transformType, weaponType);
    EXPECT_NE(transformType, colliderType);

    EXPECT_NE(velocityType, healthType);
    EXPECT_NE(velocityType, playerType);
    EXPECT_NE(velocityType, enemyType);
    EXPECT_NE(velocityType, projectileType);
    EXPECT_NE(velocityType, weaponType);
    EXPECT_NE(velocityType, colliderType);

    EXPECT_NE(healthType, playerType);
    EXPECT_NE(healthType, enemyType);
    EXPECT_NE(healthType, projectileType);
    EXPECT_NE(healthType, weaponType);
    EXPECT_NE(healthType, colliderType);

    EXPECT_NE(playerType, enemyType);
    EXPECT_NE(playerType, projectileType);
    EXPECT_NE(playerType, weaponType);
    EXPECT_NE(playerType, colliderType);

    EXPECT_NE(enemyType, projectileType);
    EXPECT_NE(enemyType, weaponType);
    EXPECT_NE(enemyType, colliderType);

    EXPECT_NE(projectileType, weaponType);
    EXPECT_NE(projectileType, colliderType);

    EXPECT_NE(weaponType, colliderType);
}
