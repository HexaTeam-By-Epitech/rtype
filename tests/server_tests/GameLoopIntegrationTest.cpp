/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameLoopIntegrationTest.cpp - Integration tests for complete game loop flow
*/

#include <gtest/gtest.h>
#include <chrono>
#include <memory>
#include <thread>

#include "Core/EventBus/EventBus.hpp"
#include "Core/ServerLoop/DeterministicGameLoop.hpp"
#include "ECS/Components/Health.hpp"
#include "ECS/Components/Player.hpp"
#include "ECS/Components/Transform.hpp"
#include "ECS/Components/Velocity.hpp"
#include "ECS/Registry.hpp"
#include "Game/Logic/GameLogic.hpp"

using namespace ecs;
using namespace server;

/**
 * @test GameLoopIntegrationTest
 * @brief Integration tests for complete server game loop workflow
 * 
 * Requirements:
 * - Game loop initializes properly
 * - Players spawn and join game
 * - Player input is processed correctly
 * - Game state updates deterministically
 * - Network snapshots are generated
 * - Tick counter increments
 */
class GameLoopIntegrationTest : public ::testing::Test {
   protected:
    std::shared_ptr<EventBus> eventBus;
    std::unique_ptr<GameLogic> gameLogic;
    std::unique_ptr<DeterministicGameLoop> gameLoop;

    void SetUp() override {
        eventBus = std::make_shared<EventBus>();
        gameLogic = std::make_unique<GameLogic>();
        gameLoop = std::make_unique<DeterministicGameLoop>(std::move(gameLogic), eventBus);
    }

    void TearDown() override {
        if (gameLoop && gameLoop->isRunning()) {
            gameLoop->stop();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
};

TEST_F(GameLoopIntegrationTest, GameLoopInitialization) {
    EXPECT_TRUE(gameLoop->initialize());
    EXPECT_FALSE(gameLoop->isRunning());  // Not running yet
}

TEST_F(GameLoopIntegrationTest, GameLoopStart) {
    ASSERT_TRUE(gameLoop->initialize());
    EXPECT_TRUE(gameLoop->start());
    EXPECT_TRUE(gameLoop->isRunning());
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    gameLoop->stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(gameLoop->isRunning());
}

TEST_F(GameLoopIntegrationTest, PlayerSpawning) {
    ASSERT_TRUE(gameLoop->initialize());

    IGameLogic &logic = gameLoop->getGameLogic();
    uint32_t playerEntity = logic.spawnPlayer(1001, "TestPlayer");

    EXPECT_NE(playerEntity, 0);
    EXPECT_TRUE(logic.isGameActive());

    Registry &registry = logic.getRegistry();
    EXPECT_TRUE(registry.hasComponent<Transform>(playerEntity));
    EXPECT_TRUE(registry.hasComponent<Health>(playerEntity));
    EXPECT_TRUE(registry.hasComponent<Player>(playerEntity));
}

TEST_F(GameLoopIntegrationTest, MultiplePlayersSpawning) {
    ASSERT_TRUE(gameLoop->initialize());

    IGameLogic &logic = gameLoop->getGameLogic();

    // Spawn multiple players
    uint32_t player1 = logic.spawnPlayer(1001, "Player1");
    uint32_t player2 = logic.spawnPlayer(1002, "Player2");
    uint32_t player3 = logic.spawnPlayer(1003, "Player3");

    EXPECT_NE(player1, 0);
    EXPECT_NE(player2, 0);
    EXPECT_NE(player3, 0);

    // All players should have different entity IDs
    EXPECT_NE(player1, player2);
    EXPECT_NE(player2, player3);
}

TEST_F(GameLoopIntegrationTest, PlayerInputProcessing) {
    ASSERT_TRUE(gameLoop->initialize());

    IGameLogic &logic = gameLoop->getGameLogic();
    uint32_t player = logic.spawnPlayer(1001, "TestPlayer");

    Registry &registry = logic.getRegistry();
    Transform &initialTransform = registry.getComponent<Transform>(player);
    float initialX = initialTransform.getPosition().x;

    // Process movement input
    logic.processPlayerInput(1001, 1, 0, false);  // Move right
    logic.update(1.0f / 60.0f);                   // One frame

    Transform &updatedTransform = registry.getComponent<Transform>(player);
    float newX = updatedTransform.getPosition().x;

    // Position should have changed
    EXPECT_GT(newX, initialX);
}

TEST_F(GameLoopIntegrationTest, GameTickIncrement) {
    ASSERT_TRUE(gameLoop->initialize());

    IGameLogic &logic = gameLoop->getGameLogic();
    logic.spawnPlayer(1001, "TestPlayer");

    uint32_t tick1 = logic.getCurrentTick();

    logic.update(1.0f / 60.0f);
    uint32_t tick2 = logic.getCurrentTick();

    logic.update(1.0f / 60.0f);
    uint32_t tick3 = logic.getCurrentTick();

    EXPECT_EQ(tick2, tick1 + 1);
    EXPECT_EQ(tick3, tick2 + 1);
}

TEST_F(GameLoopIntegrationTest, MultiframeMovement) {
    ASSERT_TRUE(gameLoop->initialize());

    IGameLogic &logic = gameLoop->getGameLogic();
    uint32_t player = logic.spawnPlayer(1001, "TestPlayer");

    Registry &registry = logic.getRegistry();
    Transform &t = registry.getComponent<Transform>(player);
    float startX = t.getPosition().x;

    // Process right movement for 10 frames
    for (int i = 0; i < 10; ++i) {
        logic.processPlayerInput(1001, 1, 0, false);
        logic.update(1.0f / 60.0f);
    }

    Transform &finalTransform = registry.getComponent<Transform>(player);
    float endX = finalTransform.getPosition().x;

    // Should have moved right
    EXPECT_GT(endX, startX);

    // At 200 units/sec speed, over 10 frames (10/60 = 0.1667s):
    // Distance = 200 * 0.1667 = 33.34 units
    float expectedDistance = 200.0f * (10.0f / 60.0f);
    EXPECT_FLOAT_EQ(endX - startX, expectedDistance);
}

TEST_F(GameLoopIntegrationTest, PlayerDespawn) {
    ASSERT_TRUE(gameLoop->initialize());

    IGameLogic &logic = gameLoop->getGameLogic();
    uint32_t player = logic.spawnPlayer(1001, "TestPlayer");
    EXPECT_NE(player, 0);

    logic.despawnPlayer(1001);
    // After despawn, attempting to process input for this player should be safe
    logic.processPlayerInput(1001, 1, 0, false);  // Should not crash
    logic.update(1.0f / 60.0f);
}

TEST_F(GameLoopIntegrationTest, GameReset) {
    ASSERT_TRUE(gameLoop->initialize());

    IGameLogic &logic = gameLoop->getGameLogic();
    logic.spawnPlayer(1001, "TestPlayer");

    uint32_t tickBefore = logic.getCurrentTick();
    logic.resetGame();
    uint32_t tickAfter = logic.getCurrentTick();

    EXPECT_EQ(tickAfter, 0);
    EXPECT_TRUE(logic.isGameActive());
}

TEST_F(GameLoopIntegrationTest, DeterministicStateProgression) {
    // Run two identical simulations and verify they produce same results

    auto runSimulation = [](const std::string &scenario) -> std::vector<float> {
        auto eventBus1 = std::make_shared<EventBus>();
        auto gameLogic1 = std::make_unique<GameLogic>();
        auto gameLoop1 = std::make_unique<DeterministicGameLoop>(std::move(gameLogic1), eventBus1);

        EXPECT_TRUE(gameLoop1->initialize());
        IGameLogic &logic1 = gameLoop1->getGameLogic();

        uint32_t player = logic1.spawnPlayer(1001, "TestPlayer");
        Registry &registry1 = logic1.getRegistry();

        std::vector<float> positions;

        // Run identical sequence: 1 right, 1 left, 1 up, 1 right for 5 frames each
        for (int i = 0; i < 5; ++i) {
            logic1.processPlayerInput(1001, 1, 0, false);  // Right
            logic1.update(1.0f / 60.0f);
            positions.push_back(registry1.getComponent<Transform>(player).getPosition().x);
        }

        gameLoop1->stop();
        return positions;
    };

    std::vector<float> sim1 = runSimulation("sim1");
    std::vector<float> sim2 = runSimulation("sim2");

    ASSERT_EQ(sim1.size(), sim2.size());
    for (size_t i = 0; i < sim1.size(); ++i) {
        EXPECT_FLOAT_EQ(sim1[i], sim2[i]) << "Position mismatch at frame " << i;
    }
}

TEST_F(GameLoopIntegrationTest, ThreadSafety) {
    ASSERT_TRUE(gameLoop->initialize());
    EXPECT_TRUE(gameLoop->start());

    IGameLogic &logic = gameLoop->getGameLogic();
    uint32_t player = logic.spawnPlayer(1001, "TestPlayer");

    // Simulate concurrent input processing
    for (int i = 0; i < 50; ++i) {
        logic.processPlayerInput(1001, 1, 0, false);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    gameLoop->stop();

    // Should complete without crashes
    EXPECT_FALSE(gameLoop->isRunning());
}
