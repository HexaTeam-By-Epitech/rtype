/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameLogicExtendedTest.cpp - Extended tests for GameLogic to increase coverage
*/

#include <gtest/gtest.h>
#include <memory>
#include "Game/Logic/GameLogic.hpp"

using namespace server;

class GameLogicExtendedTest : public ::testing::Test {
   protected:
    std::shared_ptr<GameLogic> gameLogic;

    void SetUp() override {
        gameLogic = std::make_shared<GameLogic>();
        ASSERT_TRUE(gameLogic->initialize());
    }
};

// Tests basiques
TEST_F(GameLogicExtendedTest, Initialization) {
    EXPECT_TRUE(gameLogic->isGameActive());
}

TEST_F(GameLogicExtendedTest, SpawnPlayer) {
    uint32_t entity = gameLogic->spawnPlayer(1, "TestPlayer");
    EXPECT_NE(entity, 0);
}

TEST_F(GameLogicExtendedTest, SpawnMultiplePlayers) {
    uint32_t p1 = gameLogic->spawnPlayer(1, "Player1");
    uint32_t p2 = gameLogic->spawnPlayer(2, "Player2");
    uint32_t p3 = gameLogic->spawnPlayer(3, "Player3");

    EXPECT_NE(p1, 0);
    EXPECT_NE(p2, 0);
    EXPECT_NE(p3, 0);
}

TEST_F(GameLogicExtendedTest, DespawnPlayer) {
    uint32_t player = gameLogic->spawnPlayer(1, "TestPlayer");
    ASSERT_NE(player, 0);

    EXPECT_NO_THROW(gameLogic->despawnPlayer(1));
}

TEST_F(GameLogicExtendedTest, DespawnNonExistentPlayer) {
    EXPECT_NO_THROW(gameLogic->despawnPlayer(999));
}

TEST_F(GameLogicExtendedTest, ProcessPlayerInputRight) {
    gameLogic->spawnPlayer(1, "TestPlayer");
    EXPECT_NO_THROW(gameLogic->processPlayerInput(1, 1, 0, false, 0));
    EXPECT_NO_THROW(gameLogic->update(1.0f / 60.0f, 0));
}

TEST_F(GameLogicExtendedTest, ProcessPlayerInputLeft) {
    gameLogic->spawnPlayer(1, "TestPlayer");
    EXPECT_NO_THROW(gameLogic->processPlayerInput(1, -1, 0, false, 0));
    EXPECT_NO_THROW(gameLogic->update(1.0f / 60.0f, 0));
}

TEST_F(GameLogicExtendedTest, ProcessPlayerInputUp) {
    gameLogic->spawnPlayer(1, "TestPlayer");
    EXPECT_NO_THROW(gameLogic->processPlayerInput(1, 0, -1, false, 0));
    EXPECT_NO_THROW(gameLogic->update(1.0f / 60.0f, 0));
}

TEST_F(GameLogicExtendedTest, ProcessPlayerInputDown) {
    gameLogic->spawnPlayer(1, "TestPlayer");
    EXPECT_NO_THROW(gameLogic->processPlayerInput(1, 0, 1, false, 0));
    EXPECT_NO_THROW(gameLogic->update(1.0f / 60.0f, 0));
}

TEST_F(GameLogicExtendedTest, ProcessPlayerInputShoot) {
    gameLogic->spawnPlayer(1, "TestPlayer");
    EXPECT_NO_THROW(gameLogic->processPlayerInput(1, 0, 0, true, 0));
    EXPECT_NO_THROW(gameLogic->update(1.0f / 60.0f, 0));
}

TEST_F(GameLogicExtendedTest, ProcessPlayerInputDiagonal) {
    gameLogic->spawnPlayer(1, "TestPlayer");
    EXPECT_NO_THROW(gameLogic->processPlayerInput(1, 1, 1, false, 0));
    EXPECT_NO_THROW(gameLogic->update(1.0f / 60.0f, 0));
}

TEST_F(GameLogicExtendedTest, ProcessInputForNonExistentPlayer) {
    EXPECT_NO_THROW(gameLogic->processPlayerInput(999, 1, 0, false, 0));
    EXPECT_NO_THROW(gameLogic->update(1.0f / 60.0f, 0));
}

TEST_F(GameLogicExtendedTest, UpdateWithZeroDelta) {
    gameLogic->spawnPlayer(1, "TestPlayer");
    EXPECT_NO_THROW(gameLogic->update(0.0f, 0));
}

TEST_F(GameLogicExtendedTest, UpdateWithLargeDelta) {
    gameLogic->spawnPlayer(1, "TestPlayer");
    EXPECT_NO_THROW(gameLogic->update(1.0f, 0));
}

TEST_F(GameLogicExtendedTest, UpdateMultipleTimes) {
    gameLogic->spawnPlayer(1, "TestPlayer");
    for (int i = 0; i < 100; ++i) {
        EXPECT_NO_THROW(gameLogic->update(1.0f / 60.0f, i));
    }
}

TEST_F(GameLogicExtendedTest, UpdateWithoutPlayers) {
    EXPECT_NO_THROW(gameLogic->update(1.0f / 60.0f, 0));
}

TEST_F(GameLogicExtendedTest, IsGameActive) {
    EXPECT_TRUE(gameLogic->isGameActive());

    gameLogic->spawnPlayer(1, "TestPlayer");
    EXPECT_TRUE(gameLogic->isGameActive());
}

TEST_F(GameLogicExtendedTest, GetRegistry) {
    auto &reg = gameLogic->getRegistry();
    (void)reg;  // Use it
    SUCCEED();
}

TEST_F(GameLogicExtendedTest, CompleteGameScenario) {
    gameLogic->spawnPlayer(1, "Player1");
    gameLogic->spawnPlayer(2, "Player2");

    for (int frame = 0; frame < 100; ++frame) {
        if (frame % 10 == 0) {
            gameLogic->processPlayerInput(1, 1, 0, frame % 20 == 0, frame);
            gameLogic->processPlayerInput(2, -1, 0, frame % 20 == 10, frame);
        }
        gameLogic->update(1.0f / 60.0f, frame);
    }

    EXPECT_TRUE(gameLogic->isGameActive());
}

TEST_F(GameLogicExtendedTest, MultiplePlayers) {
    gameLogic->spawnPlayer(1, "P1");
    gameLogic->spawnPlayer(2, "P2");
    gameLogic->spawnPlayer(3, "P3");
    gameLogic->spawnPlayer(4, "P4");

    for (int i = 0; i < 50; ++i) {
        gameLogic->update(1.0f / 60.0f, i);
    }
}

TEST_F(GameLogicExtendedTest, ResetGame) {
    gameLogic->spawnPlayer(1, "Player1");
    gameLogic->update(1.0f / 60.0f, 0);

    EXPECT_NO_THROW(gameLogic->resetGame());
    EXPECT_TRUE(gameLogic->isGameActive());
}

TEST_F(GameLogicExtendedTest, SpawnAndDespawnCycle) {
    for (int i = 0; i < 5; ++i) {
        uint32_t entity = gameLogic->spawnPlayer(i + 1, "Player");
        EXPECT_NE(entity, 0);
        gameLogic->despawnPlayer(i + 1);
    }
}

TEST_F(GameLogicExtendedTest, ContinuousInput) {
    gameLogic->spawnPlayer(1, "Player");

    for (int i = 0; i < 60; ++i) {
        gameLogic->processPlayerInput(1, 1, 0, i % 5 == 0, i);
        gameLogic->update(1.0f / 60.0f, i);
    }
}

TEST_F(GameLogicExtendedTest, AllDirections) {
    gameLogic->spawnPlayer(1, "Player");

    gameLogic->processPlayerInput(1, 1, 0, false, 1);  // Right
    gameLogic->update(1.0f / 60.0f, 0);

    gameLogic->processPlayerInput(1, -1, 0, false, 2);  // Left
    gameLogic->update(1.0f / 60.0f, 1);

    gameLogic->processPlayerInput(1, 0, 1, false, 3);  // Down
    gameLogic->update(1.0f / 60.0f, 2);

    gameLogic->processPlayerInput(1, 0, -1, false, 4);  // Up
    gameLogic->update(1.0f / 60.0f, 3);
}

TEST_F(GameLogicExtendedTest, ShootingPattern) {
    gameLogic->spawnPlayer(1, "Player");

    for (int i = 0; i < 20; ++i) {
        gameLogic->processPlayerInput(1, 0, 0, true, i);
        gameLogic->update(1.0f / 60.0f, i);
    }
}

TEST_F(GameLogicExtendedTest, LongRunningSession) {
    gameLogic->spawnPlayer(1, "P1");
    gameLogic->spawnPlayer(2, "P2");

    for (int i = 0; i < 300; ++i) {
        if (i % 5 == 0) {
            gameLogic->processPlayerInput(1, (i % 2) ? 1 : -1, 0, i % 10 == 0, i);
            gameLogic->processPlayerInput(2, (i % 2) ? -1 : 1, 0, i % 15 == 0, i);
        }
        gameLogic->update(1.0f / 60.0f, i);
    }
}