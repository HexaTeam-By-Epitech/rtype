/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** NetworkSyncTest.cpp - Tests for network state synchronization
*/

#include <gtest/gtest.h>
#include <memory>

#include "ECS/Components/Health.hpp"
#include "ECS/Components/Player.hpp"
#include "ECS/Components/Transform.hpp"
#include "ECS/Registry.hpp"
#include "Game/Logic/GameLogic.hpp"
#include "Game/Logic/GameStateSerializer.hpp"

using namespace ecs;
using namespace server;

/**
 * @test NetworkSyncTest
 * @brief Tests for game state serialization and network synchronization
 * 
 * Requirements:
 * - Game snapshots include all entity data
 * - Delta updates only include changed entities
 * - Entity serialization preserves state
 * - Snapshot ordering is deterministic
 */
class NetworkSyncTest : public ::testing::Test {
   protected:
    Registry registry;
    GameLogic gameLogic;

    void SetUp() override { ASSERT_TRUE(gameLogic.initialize()); }
};

TEST_F(NetworkSyncTest, FullSnapshotCreation) {
    // Create some entities
    uint32_t player = gameLogic.spawnPlayer(1001, "TestPlayer");
    ASSERT_NE(player, 0);

    // Update game state
    gameLogic.update(1.0f / 60.0f);

    // Create snapshot
    auto snapshot = GameStateSerializer::createFullSnapshot(gameLogic.getRegistry(), 0);

    EXPECT_EQ(snapshot.serverTick, 0);
    EXPECT_GT(snapshot.entities.size(), 0);  // Should have at least the player
    EXPECT_GT(snapshot.activePlayerCount, 0);
}

TEST_F(NetworkSyncTest, EntitySnapshotSerialization) {
    uint32_t player = gameLogic.spawnPlayer(1001, "TestPlayer");
    ASSERT_NE(player, 0);

    auto snapshot = GameStateSerializer::serializeEntity(gameLogic.getRegistry(), player);

    EXPECT_EQ(snapshot.entityId, player);
    EXPECT_EQ(snapshot.currentHealth, 100);
    EXPECT_EQ(snapshot.maxHealth, 100);
    EXPECT_EQ(snapshot.playerId, 1001);
    EXPECT_TRUE(snapshot.isAlive);
}

TEST_F(NetworkSyncTest, EntityPositionInSnapshot) {
    uint32_t player = gameLogic.spawnPlayer(1001, "TestPlayer");
    ASSERT_NE(player, 0);

    // Move player
    gameLogic.processPlayerInput(1001, 1, 0, false);
    gameLogic.update(1.0f / 60.0f);

    auto snapshot = GameStateSerializer::serializeEntity(gameLogic.getRegistry(), player);

    // Player should have moved from spawn position (50, 300)
    EXPECT_GT(snapshot.posX, 50.0f);
    EXPECT_FLOAT_EQ(snapshot.posY, 300.0f);
}

TEST_F(NetworkSyncTest, HealthChangeInSnapshot) {
    uint32_t player = gameLogic.spawnPlayer(1001, "TestPlayer");
    ASSERT_NE(player, 0);

    auto &registry = gameLogic.getRegistry();
    auto &health = registry.getComponent<Health>(player);

    health.takeDamage(25);

    auto snapshot = GameStateSerializer::serializeEntity(registry, player);

    EXPECT_EQ(snapshot.currentHealth, 75);
    EXPECT_TRUE(snapshot.isAlive);
}

TEST_F(NetworkSyncTest, DeadPlayerSnapshot) {
    uint32_t player = gameLogic.spawnPlayer(1001, "TestPlayer");
    ASSERT_NE(player, 0);

    auto &registry = gameLogic.getRegistry();
    auto &health = registry.getComponent<Health>(player);

    health.takeDamage(100);

    auto snapshot = GameStateSerializer::serializeEntity(registry, player);

    EXPECT_EQ(snapshot.currentHealth, 0);
    EXPECT_FALSE(snapshot.isAlive);
}

TEST_F(NetworkSyncTest, MultipleEntitySnapshot) {
    // Create multiple players
    uint32_t player1 = gameLogic.spawnPlayer(1001, "Player1");
    uint32_t player2 = gameLogic.spawnPlayer(1002, "Player2");

    ASSERT_NE(player1, 0);
    ASSERT_NE(player2, 0);

    gameLogic.update(1.0f / 60.0f);

    auto snapshot = GameStateSerializer::createFullSnapshot(gameLogic.getRegistry(), 0);

    // Should have snapshots for both players
    EXPECT_GE(snapshot.entities.size(), 2);
    EXPECT_EQ(snapshot.activePlayerCount, 2);
}

TEST_F(NetworkSyncTest, SnapshotAfterPlayerMovement) {
    uint32_t player = gameLogic.spawnPlayer(1001, "TestPlayer");
    ASSERT_NE(player, 0);

    // Move for several frames
    for (int i = 0; i < 10; ++i) {
        gameLogic.processPlayerInput(1001, 1, 0, false);  // Right
        gameLogic.update(1.0f / 60.0f);
    }

    auto snapshot = GameStateSerializer::createFullSnapshot(gameLogic.getRegistry(), 10);

    EXPECT_EQ(snapshot.serverTick, 10);
    EXPECT_GT(snapshot.entities[0].posX, 50.0f);  // Moved from spawn position
}
