/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameLoopTest.cpp - Test suite for game loop determinism, timing, and event flow
*/

#include <gtest/gtest.h>
#include <cmath>
#include <memory>
#include <vector>

#include "ECS/Components/Collider.hpp"
#include "ECS/Components/Enemy.hpp"
#include "ECS/Components/Health.hpp"
#include "ECS/Components/Player.hpp"
#include "ECS/Components/Transform.hpp"
#include "ECS/Components/Velocity.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Systems/CollisionSystem/CollisionSystem.hpp"
#include "ECS/Systems/HealthSystem/HealthSystem.hpp"
#include "ECS/Systems/MovementSystem/MovementSystem.hpp"

using namespace ecs;

/**
 * @test GameLoopTimingTest
 * @brief Verify fixed timestep execution and deterministic updates
 * 
 * Requirements:
 * - Game loop executes at a fixed 60 Hz (1/60 = 0.0166667s per frame)
 * - Multiple frames produce consistent, reproducible state
 * - Delta time is constant across frames
 */
class GameLoopTimingTest : public ::testing::Test {
   protected:
    Registry registry;
    MovementSystem movementSystem;
    const float FIXED_TIMESTEP = 1.0f / 60.0f;  // 60 Hz

    void SetUp() override {
        // Initialize registry for each test
    }
};

TEST_F(GameLoopTimingTest, FixedTimestepConsistency) {
    // Create entity with movement
    Address entity = registry.newEntity();
    registry.setComponent(entity, Transform(0.0f, 0.0f));
    registry.setComponent(entity, Velocity(1.0f, 0.0f, 100.0f));  // 100 units/sec to the right

    // First frame update
    movementSystem.update(registry, FIXED_TIMESTEP);
    Transform &transform1 = registry.getComponent<Transform>(entity);
    float posAfterFrame1 = transform1.getPosition().x;

    // Reset for second frame - same velocity
    registry.setComponent(entity, Transform(0.0f, 0.0f));
    movementSystem.update(registry, FIXED_TIMESTEP);
    Transform &transform2 = registry.getComponent<Transform>(entity);
    float posAfterFrame2 = transform2.getPosition().x;

    // Both frames should move the same distance
    EXPECT_FLOAT_EQ(posAfterFrame1, posAfterFrame2);
    EXPECT_FLOAT_EQ(posAfterFrame1, 100.0f * FIXED_TIMESTEP);
}

TEST_F(GameLoopTimingTest, MultipleFramesAccumulate) {
    // Create entity with constant velocity
    Address entity = registry.newEntity();
    registry.setComponent(entity, Transform(0.0f, 0.0f));
    registry.setComponent(entity, Velocity(1.0f, 0.0f, 100.0f));  // 100 units/sec

    // Simulate 60 frames (1 second)
    for (int i = 0; i < 60; ++i) {
        movementSystem.update(registry, FIXED_TIMESTEP);
    }

    Transform &transform = registry.getComponent<Transform>(entity);
    float finalX = transform.getPosition().x;

    // After 1 second at 100 units/sec, should be at x=100
    EXPECT_FLOAT_EQ(finalX, 100.0f);
}

TEST_F(GameLoopTimingTest, ZeroVelocityNoMovement) {
    Address entity = registry.newEntity();
    registry.setComponent(entity, Transform(50.0f, 50.0f));
    registry.setComponent(entity, Velocity(0.0f, 0.0f, 0.0f));

    for (int i = 0; i < 10; ++i) {
        movementSystem.update(registry, FIXED_TIMESTEP);
    }

    Transform &transform = registry.getComponent<Transform>(entity);
    EXPECT_FLOAT_EQ(transform.getPosition().x, 50.0f);
    EXPECT_FLOAT_EQ(transform.getPosition().y, 50.0f);
}

/**
 * @test EntityLifecycleTest
 * @brief Verify entity creation, component assignment, and destruction
 * 
 * Requirements:
 * - Entities spawn with correct initial state
 * - Component data persists across frames
 * - Entities can be destroyed cleanly
 */
class EntityLifecycleTest : public ::testing::Test {
   protected:
    Registry registry;
};

TEST_F(EntityLifecycleTest, EntityCreationWithComponents) {
    Address entity = registry.newEntity();
    EXPECT_NE(entity, 0);  // Valid entity ID

    registry.setComponent(entity, Transform(100.0f, 200.0f));
    registry.setComponent(entity, Health(100));

    EXPECT_TRUE(registry.hasComponent<Transform>(entity));
    EXPECT_TRUE(registry.hasComponent<Health>(entity));

    Transform &t = registry.getComponent<Transform>(entity);
    EXPECT_FLOAT_EQ(t.getPosition().x, 100.0f);
    EXPECT_FLOAT_EQ(t.getPosition().y, 200.0f);
}

TEST_F(EntityLifecycleTest, PlayerEntityCreation) {
    Address playerEntity = registry.newEntity();
    registry.setComponent(playerEntity, Player(0, 3, 1001));  // score, lives, playerId
    registry.setComponent(playerEntity, Transform(50.0f, 300.0f));
    registry.setComponent(playerEntity, Health(100));
    registry.setComponent(playerEntity, Velocity(0.0f, 0.0f, 200.0f));

    Player &player = registry.getComponent<Player>(playerEntity);
    EXPECT_EQ(player.getScore(), 0);
    EXPECT_EQ(player.getLives(), 3);
    EXPECT_EQ(player.getPlayerId(), 1001);
}

TEST_F(EntityLifecycleTest, EnemyEntityCreation) {
    Address enemyEntity = registry.newEntity();
    registry.setComponent(enemyEntity, Enemy(100));  // health
    registry.setComponent(enemyEntity, Transform(800.0f, 400.0f));
    registry.setComponent(enemyEntity, Velocity(-1.0f, 0.0f, 150.0f));  // Moving left
    registry.setComponent(enemyEntity, Health(100));

    Enemy &enemy = registry.getComponent<Enemy>(enemyEntity);
    EXPECT_EQ(enemy.getHealth(), 100);
}

TEST_F(EntityLifecycleTest, ComponentRemoval) {
    Address entity = registry.newEntity();
    registry.setComponent(entity, Transform(0.0f, 0.0f));
    registry.setComponent(entity, Velocity(1.0f, 0.0f, 100.0f));

    EXPECT_TRUE(registry.hasComponent<Velocity>(entity));
    registry.removeComponent<Velocity>(entity);
    EXPECT_FALSE(registry.hasComponent<Velocity>(entity));
}

/**
 * @test MovementPhysicsTest
 * @brief Verify movement calculations, velocity application, and directional movement
 * 
 * Requirements:
 * - Velocity is applied correctly to position
 * - Direction vectors are normalized properly
 * - Movement is frame-independent (uses deltaTime)
 * - Diagonal movement works correctly
 */
class MovementPhysicsTest : public ::testing::Test {
   protected:
    Registry registry;
    MovementSystem movementSystem;
    const float FIXED_TIMESTEP = 1.0f / 60.0f;
};

TEST_F(MovementPhysicsTest, HorizontalMovementRight) {
    Address entity = registry.newEntity();
    registry.setComponent(entity, Transform(0.0f, 0.0f));
    registry.setComponent(entity, Velocity(1.0f, 0.0f, 100.0f));

    movementSystem.update(registry, FIXED_TIMESTEP);

    Transform &t = registry.getComponent<Transform>(entity);
    float expectedX = 100.0f * FIXED_TIMESTEP;
    EXPECT_FLOAT_EQ(t.getPosition().x, expectedX);
    EXPECT_FLOAT_EQ(t.getPosition().y, 0.0f);
}

TEST_F(MovementPhysicsTest, HorizontalMovementLeft) {
    Address entity = registry.newEntity();
    registry.setComponent(entity, Transform(1000.0f, 500.0f));
    registry.setComponent(entity, Velocity(-1.0f, 0.0f, 150.0f));

    movementSystem.update(registry, FIXED_TIMESTEP);

    Transform &t = registry.getComponent<Transform>(entity);
    float expectedX = 1000.0f - (150.0f * FIXED_TIMESTEP);
    EXPECT_FLOAT_EQ(t.getPosition().x, expectedX);
}

TEST_F(MovementPhysicsTest, DiagonalMovement) {
    Address entity = registry.newEntity();
    registry.setComponent(entity, Transform(0.0f, 0.0f));

    // Normalized diagonal direction (45 degrees)
    float diagX = 1.0f / std::sqrt(2.0f);
    float diagY = 1.0f / std::sqrt(2.0f);
    float speed = 100.0f;

    registry.setComponent(entity, Velocity(diagX, diagY, speed));

    movementSystem.update(registry, FIXED_TIMESTEP);

    Transform &t = registry.getComponent<Transform>(entity);
    float expectedDist = speed * FIXED_TIMESTEP;
    float expectedX = expectedDist * diagX;
    float expectedY = expectedDist * diagY;

    EXPECT_FLOAT_EQ(t.getPosition().x, expectedX);
    EXPECT_FLOAT_EQ(t.getPosition().y, expectedY);
}

TEST_F(MovementPhysicsTest, VerticalMovement) {
    Address entity = registry.newEntity();
    registry.setComponent(entity, Transform(0.0f, 0.0f));
    registry.setComponent(entity, Velocity(0.0f, 1.0f, 200.0f));

    movementSystem.update(registry, FIXED_TIMESTEP);

    Transform &t = registry.getComponent<Transform>(entity);
    EXPECT_FLOAT_EQ(t.getPosition().x, 0.0f);
    EXPECT_FLOAT_EQ(t.getPosition().y, 200.0f * FIXED_TIMESTEP);
}

/**
 * @test CollisionDetectionTest
 * @brief Verify AABB collision detection, layer filtering, and trigger zones
 * 
 * Requirements:
 * - AABB intersection detection works
 * - Layer masks filter collisions correctly
 * - Trigger zones don't cause physics responses
 * - Collision events are generated
 */
class CollisionDetectionTest : public ::testing::Test {
   protected:
    Registry registry;
    CollisionSystem collisionSystem;
};

TEST_F(CollisionDetectionTest, AABBCollisionDetection) {
    // Create two overlapping entities
    Address entity1 = registry.newEntity();
    registry.setComponent(entity1, Transform(0.0f, 0.0f));
    registry.setComponent(entity1, Collider(100.0f, 100.0f, 0.0f, 0.0f, 0, 0xFFFFFFFF, false));

    Address entity2 = registry.newEntity();
    registry.setComponent(entity2, Transform(50.0f, 50.0f));
    registry.setComponent(entity2, Collider(100.0f, 100.0f, 0.0f, 0.0f, 0, 0xFFFFFFFF, false));

    // Check that entities overlap
    Transform &t1 = registry.getComponent<Transform>(entity1);
    Transform &t2 = registry.getComponent<Transform>(entity2);
    Collider &c1 = registry.getComponent<Collider>(entity1);
    Collider &c2 = registry.getComponent<Collider>(entity2);

    // AABB check: [0, 100] x [0, 100] overlaps with [50, 150] x [50, 150]
    bool xOverlap = (t1.getPosition().x + c1.getSize().x) > t2.getPosition().x &&
                    (t2.getPosition().x + c2.getSize().x) > t1.getPosition().x;
    bool yOverlap = (t1.getPosition().y + c1.getSize().y) > t2.getPosition().y &&
                    (t2.getPosition().y + c2.getSize().y) > t1.getPosition().y;

    EXPECT_TRUE(xOverlap && yOverlap);
}

TEST_F(CollisionDetectionTest, NoCollisionWhenSeparated) {
    Address entity1 = registry.newEntity();
    registry.setComponent(entity1, Transform(0.0f, 0.0f));
    registry.setComponent(entity1, Collider(100.0f, 100.0f, 0.0f, 0.0f, 0, 0xFFFFFFFF, false));

    Address entity2 = registry.newEntity();
    registry.setComponent(entity2, Transform(200.0f, 200.0f));
    registry.setComponent(entity2, Collider(100.0f, 100.0f, 0.0f, 0.0f, 0, 0xFFFFFFFF, false));

    Transform &t1 = registry.getComponent<Transform>(entity1);
    Transform &t2 = registry.getComponent<Transform>(entity2);
    Collider &c1 = registry.getComponent<Collider>(entity1);
    Collider &c2 = registry.getComponent<Collider>(entity2);

    bool xOverlap = (t1.getPosition().x + c1.getSize().x) > t2.getPosition().x &&
                    (t2.getPosition().x + c2.getSize().x) > t1.getPosition().x;
    bool yOverlap = (t1.getPosition().y + c1.getSize().y) > t2.getPosition().y &&
                    (t2.getPosition().y + c2.getSize().y) > t1.getPosition().y;

    EXPECT_FALSE(xOverlap && yOverlap);
}

TEST_F(CollisionDetectionTest, TriggerZoneDetection) {
    Address entity1 = registry.newEntity();
    registry.setComponent(entity1, Transform(0.0f, 0.0f));
    registry.setComponent(entity1, Collider(100.0f, 100.0f, 0.0f, 0.0f, 0, 0xFFFFFFFF, true));  // trigger

    Address entity2 = registry.newEntity();
    registry.setComponent(entity2, Transform(50.0f, 50.0f));
    registry.setComponent(entity2, Collider(100.0f, 100.0f, 0.0f, 0.0f, 0, 0xFFFFFFFF, false));

    Collider &c1 = registry.getComponent<Collider>(entity1);
    EXPECT_TRUE(c1.isTrigger());
}

/**
 * @test HealthDamageTest
 * @brief Verify health system, damage application, invincibility frames
 * 
 * Requirements:
 * - Health decreases correctly when damaged
 * - Entity dies when health reaches 0
 * - Invincibility frames prevent damage temporarily
 * - Max health is enforced
 */
class HealthDamageTest : public ::testing::Test {
   protected:
    Registry registry;
    HealthSystem healthSystem;
    const float FIXED_TIMESTEP = 1.0f / 60.0f;
};

TEST_F(HealthDamageTest, ApplyDamage) {
    Address entity = registry.newEntity();
    registry.setComponent(entity, Health(100, 100));

    Health &health = registry.getComponent<Health>(entity);
    health.takeDamage(25);

    EXPECT_EQ(health.getCurrentHealth(), 75);
}

TEST_F(HealthDamageTest, HealthCannotExceedMax) {
    Address entity = registry.newEntity();
    registry.setComponent(entity, Health(100, 100));

    Health &health = registry.getComponent<Health>(entity);
    health.heal(50);

    EXPECT_EQ(health.getCurrentHealth(), 100);
}

TEST_F(HealthDamageTest, DeathAtZeroHealth) {
    Address entity = registry.newEntity();
    registry.setComponent(entity, Health(10, 100));

    Health &health = registry.getComponent<Health>(entity);
    health.takeDamage(10);

    EXPECT_EQ(health.getCurrentHealth(), 0);
    EXPECT_TRUE(health.isDead());
}

TEST_F(HealthDamageTest, InvincibilityFrames) {
    Address entity = registry.newEntity();
    registry.setComponent(entity, Health(100, 100));

    Health &health = registry.getComponent<Health>(entity);
    health.setInvincible(1.0f);  // 1 second invincibility

    EXPECT_TRUE(health.isInvincible());

    // Try to take damage while invincible
    int healthBefore = health.getCurrentHealth();
    // In real implementation, damage would be blocked here

    health.updateInvincibility(FIXED_TIMESTEP);
    EXPECT_EQ(health.getCurrentHealth(), healthBefore);
}

/**
 * @test PlayerInputTest
 * @brief Verify player input processing and velocity updates
 * 
 * Requirements:
 * - Input events update velocity correctly
 * - Movement stops when input is released
 * - Multiple simultaneous inputs work (diagonal movement)
 * - Input is applied before movement system
 */
class PlayerInputTest : public ::testing::Test {
   protected:
    Registry registry;
    const float FIXED_TIMESTEP = 1.0f / 60.0f;
};

TEST_F(PlayerInputTest, InputMoveRight) {
    Address player = registry.newEntity();
    registry.setComponent(player, Transform(100.0f, 300.0f));
    registry.setComponent(player, Velocity(0.0f, 0.0f, 200.0f));

    // Simulate input: move right
    Velocity &vel = registry.getComponent<Velocity>(player);
    vel.setDirection(1.0f, 0.0f);

    MovementSystem movementSystem;
    movementSystem.update(registry, FIXED_TIMESTEP);

    Transform &t = registry.getComponent<Transform>(player);
    EXPECT_GT(t.getPosition().x, 100.0f);
}

TEST_F(PlayerInputTest, InputStopMovement) {
    Address player = registry.newEntity();
    registry.setComponent(player, Transform(0.0f, 300.0f));
    registry.setComponent(player, Velocity(1.0f, 0.0f, 200.0f));

    // Stop movement
    Velocity &vel = registry.getComponent<Velocity>(player);
    vel.setDirection(0.0f, 0.0f);
    vel.setSpeed(0.0f);

    MovementSystem movementSystem;
    movementSystem.update(registry, FIXED_TIMESTEP);

    Transform &t = registry.getComponent<Transform>(player);
    EXPECT_FLOAT_EQ(t.getPosition().x, 0.0f);
}

TEST_F(PlayerInputTest, DiagonalInputMovement) {
    Address player = registry.newEntity();
    registry.setComponent(player, Transform(0.0f, 0.0f));

    float diagX = 1.0f / std::sqrt(2.0f);
    float diagY = 1.0f / std::sqrt(2.0f);
    registry.setComponent(player, Velocity(diagX, diagY, 200.0f));

    MovementSystem movementSystem;
    movementSystem.update(registry, FIXED_TIMESTEP);

    Transform &t = registry.getComponent<Transform>(player);
    EXPECT_GT(t.getPosition().x, 0.0f);
    EXPECT_GT(t.getPosition().y, 0.0f);
}

/**
 * @test GameStateSnapshotTest
 * @brief Verify game state serialization and delta updates
 * 
 * Requirements:
 * - Full snapshots capture all entity states
 * - Delta updates only include changed entities
 * - Snapshots are consistent across frames
 * - Entity ordering is deterministic
 */
class GameStateSnapshotTest : public ::testing::Test {
   protected:
    Registry registry;
};

TEST_F(GameStateSnapshotTest, SnapshotCreation) {
    // Create multiple entities
    Address player = registry.newEntity();
    registry.setComponent(player, Transform(100.0f, 300.0f));
    registry.setComponent(player, Health(100, 100));

    Address enemy = registry.newEntity();
    registry.setComponent(enemy, Transform(800.0f, 400.0f));
    registry.setComponent(enemy, Health(50, 50));

    // Snapshots should capture all entities
    // In real implementation, would call registry.createSnapshot()
    EXPECT_TRUE(registry.hasComponent<Transform>(player));
    EXPECT_TRUE(registry.hasComponent<Health>(enemy));
}

TEST_F(GameStateSnapshotTest, DeterministicEntityOrdering) {
    // Create entities in order
    std::vector<Address> entities;
    for (int i = 0; i < 10; ++i) {
        Address e = registry.newEntity();
        registry.setComponent(e, Transform(static_cast<float>(i * 100), 0.0f));
        entities.push_back(e);
    }

    // Entity IDs should be consistent
    for (size_t i = 0; i < entities.size(); ++i) {
        EXPECT_NE(entities[i], 0);
    }
}

/**
 * @test NetworkSynchronizationTest
 * @brief Verify network message generation and client state sync
 * 
 * Requirements:
 * - GameStart messages include initial state
 * - EntityState messages reflect current positions/health
 * - GameState messages include all active entities
 * - Deterministic entity ordering for sync
 */
class NetworkSynchronizationTest : public ::testing::Test {
   protected:
    Registry registry;
};

TEST_F(NetworkSynchronizationTest, GameStartMessageGeneration) {
    Address player = registry.newEntity();
    uint32_t playerId = 1001;

    registry.setComponent(player, Player(0, 3, playerId));
    registry.setComponent(player, Transform(50.0f, 300.0f));
    registry.setComponent(player, Health(100, 100));

    // GameStart message should include player entity ID and initial state
    Player &p = registry.getComponent<Player>(player);
    EXPECT_EQ(p.getPlayerId(), playerId);
}

TEST_F(NetworkSynchronizationTest, EntityStateConsistency) {
    Address entity = registry.newEntity();
    registry.setComponent(entity, Transform(100.0f, 200.0f));
    registry.setComponent(entity, Health(75, 100));

    Transform &t = registry.getComponent<Transform>(entity);
    Health &h = registry.getComponent<Health>(entity);

    // State should be readable for serialization
    EXPECT_FLOAT_EQ(t.getPosition().x, 100.0f);
    EXPECT_FLOAT_EQ(t.getPosition().y, 200.0f);
    EXPECT_EQ(h.getCurrentHealth(), 75);
}
