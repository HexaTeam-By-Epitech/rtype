/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** MatchmakingTest.cpp - Unit tests for MatchmakingService
*/

#include <gtest/gtest.h>
#include <chrono>
#include <memory>
#include <thread>
#include "server/Rooms/Matchmaking/MatchmakingService.hpp"

using namespace server;

// ============================================================================
// MatchmakingService Tests
// ============================================================================

class MatchmakingTest : public ::testing::Test {
   protected:
    void SetUp() override {
        matchmaking = std::make_unique<MatchmakingService>(2, 4);  // Min 2, Max 4
    }

    void TearDown() override { matchmaking.reset(); }

    std::unique_ptr<MatchmakingService> matchmaking;
};

TEST_F(MatchmakingTest, Construction) {
    EXPECT_NE(matchmaking, nullptr);
    EXPECT_EQ(matchmaking->getQueueSize(), 0);
}

TEST_F(MatchmakingTest, AddPlayer) {
    matchmaking->addPlayer(1);
    EXPECT_EQ(matchmaking->getQueueSize(), 1);

    matchmaking->addPlayer(2);
    EXPECT_EQ(matchmaking->getQueueSize(), 2);
}

TEST_F(MatchmakingTest, AddSamePlayerTwice) {
    matchmaking->addPlayer(1);
    EXPECT_EQ(matchmaking->getQueueSize(), 1);

    matchmaking->addPlayer(1);  // Should not add again
    EXPECT_EQ(matchmaking->getQueueSize(), 1);
}

TEST_F(MatchmakingTest, RemovePlayer) {
    matchmaking->addPlayer(1);
    matchmaking->addPlayer(2);
    matchmaking->addPlayer(3);
    EXPECT_EQ(matchmaking->getQueueSize(), 3);

    matchmaking->removePlayer(2);
    EXPECT_EQ(matchmaking->getQueueSize(), 2);
}

TEST_F(MatchmakingTest, RemoveNonExistentPlayer) {
    matchmaking->addPlayer(1);
    EXPECT_EQ(matchmaking->getQueueSize(), 1);

    matchmaking->removePlayer(999);  // Not in queue
    EXPECT_EQ(matchmaking->getQueueSize(), 1);
}

TEST_F(MatchmakingTest, MatchCreatedCallback) {
    bool callbackCalled = false;
    std::shared_ptr<Room> createdRoom = nullptr;

    matchmaking->setMatchCreatedCallback([&](std::shared_ptr<Room> room) {
        callbackCalled = true;
        createdRoom = room;
    });

    // Add minimum players
    matchmaking->addPlayer(1);
    matchmaking->addPlayer(2);

    // Trigger matchmaking
    matchmaking->tick();

    EXPECT_TRUE(callbackCalled);
    EXPECT_NE(createdRoom, nullptr);
}

TEST_F(MatchmakingTest, NoMatchWithInsufficientPlayers) {
    bool callbackCalled = false;

    matchmaking->setMatchCreatedCallback([&](std::shared_ptr<Room> room) { callbackCalled = true; });

    // Add only 1 player (minimum is 2)
    matchmaking->addPlayer(1);
    matchmaking->tick();

    EXPECT_FALSE(callbackCalled);
    EXPECT_EQ(matchmaking->getQueueSize(), 1);
}

TEST_F(MatchmakingTest, MatchWithMaxPlayers) {
    bool callbackCalled = false;
    std::shared_ptr<Room> createdRoom = nullptr;

    matchmaking->setMatchCreatedCallback([&](std::shared_ptr<Room> room) {
        callbackCalled = true;
        createdRoom = room;
    });

    // Add max players (4)
    matchmaking->addPlayer(1);
    matchmaking->addPlayer(2);
    matchmaking->addPlayer(3);
    matchmaking->addPlayer(4);

    matchmaking->tick();

    EXPECT_TRUE(callbackCalled);
    EXPECT_NE(createdRoom, nullptr);
    EXPECT_EQ(matchmaking->getQueueSize(), 0);  // All matched
}

TEST_F(MatchmakingTest, PartialMatch) {
    bool callbackCalled = false;

    matchmaking->setMatchCreatedCallback([&](std::shared_ptr<Room> room) { callbackCalled = true; });

    // Add 5 players (max is 4)
    for (uint32_t i = 1; i <= 5; ++i) {
        matchmaking->addPlayer(i);
    }

    matchmaking->tick();

    EXPECT_TRUE(callbackCalled);
    EXPECT_EQ(matchmaking->getQueueSize(), 1);  // 1 player remains
}

TEST_F(MatchmakingTest, MultipleMatches) {
    int matchCount = 0;

    matchmaking->setMatchCreatedCallback([&](std::shared_ptr<Room> room) { matchCount++; });

    // Add 8 players (should create 2 matches of 4)
    for (uint32_t i = 1; i <= 8; ++i) {
        matchmaking->addPlayer(i);
    }

    matchmaking->tick();

    EXPECT_EQ(matchCount, 2);
    EXPECT_EQ(matchmaking->getQueueSize(), 0);
}

TEST_F(MatchmakingTest, SetMinPlayers) {
    matchmaking->setMinPlayers(3);

    // Add 2 players (below new minimum)
    matchmaking->addPlayer(1);
    matchmaking->addPlayer(2);

    bool callbackCalled = false;
    matchmaking->setMatchCreatedCallback([&](std::shared_ptr<Room> room) { callbackCalled = true; });

    matchmaking->tick();
    EXPECT_FALSE(callbackCalled);  // Not enough players

    // Add one more to reach minimum
    matchmaking->addPlayer(3);
    matchmaking->tick();
    EXPECT_TRUE(callbackCalled);
}

TEST_F(MatchmakingTest, SetMaxPlayers) {
    matchmaking->setMaxPlayers(2);

    std::shared_ptr<Room> createdRoom = nullptr;
    matchmaking->setMatchCreatedCallback([&](std::shared_ptr<Room> room) { createdRoom = room; });

    // Add 4 players
    for (uint32_t i = 1; i <= 4; ++i) {
        matchmaking->addPlayer(i);
    }

    matchmaking->tick();

    EXPECT_NE(createdRoom, nullptr);
    // Should have created match with 2 players (new max)
}

TEST_F(MatchmakingTest, GetWaitingPlayers) {
    matchmaking->addPlayer(1);
    matchmaking->addPlayer(2);
    matchmaking->addPlayer(3);

    auto waitingPlayers = matchmaking->getWaitingPlayers();
    EXPECT_EQ(waitingPlayers.size(), 3);

    // Verify player IDs
    std::vector<uint32_t> playerIds;
    for (const auto &info : waitingPlayers) {
        playerIds.push_back(info.playerId);
    }

    EXPECT_TRUE(std::find(playerIds.begin(), playerIds.end(), 1) != playerIds.end());
    EXPECT_TRUE(std::find(playerIds.begin(), playerIds.end(), 2) != playerIds.end());
    EXPECT_TRUE(std::find(playerIds.begin(), playerIds.end(), 3) != playerIds.end());
}

TEST_F(MatchmakingTest, GetStatistics) {
    matchmaking->addPlayer(1);
    matchmaking->addPlayer(2);
    matchmaking->addPlayer(3);

    std::string stats = matchmaking->getStatistics();
    EXPECT_FALSE(stats.empty());
}

TEST_F(MatchmakingTest, WaitTimeTracking) {
    matchmaking->addPlayer(1);

    auto waitingPlayers = matchmaking->getWaitingPlayers();
    ASSERT_EQ(waitingPlayers.size(), 1);

    auto joinTime = waitingPlayers[0].joinTime;
    auto now = std::chrono::steady_clock::now();

    // Join time should be recent (within last second)
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - joinTime);
    EXPECT_LT(elapsed.count(), 1);
}

TEST_F(MatchmakingTest, EmptyQueueTick) {
    // Tick with no players should not crash
    EXPECT_NO_THROW(matchmaking->tick());
    EXPECT_EQ(matchmaking->getQueueSize(), 0);
}

TEST_F(MatchmakingTest, MultipleTicksCalls) {
    matchmaking->addPlayer(1);

    // Multiple ticks should not cause issues
    matchmaking->tick();
    matchmaking->tick();
    matchmaking->tick();

    EXPECT_EQ(matchmaking->getQueueSize(), 1);  // Player still waiting
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(MatchmakingTest, RemoveAllPlayers) {
    matchmaking->addPlayer(1);
    matchmaking->addPlayer(2);
    matchmaking->addPlayer(3);

    matchmaking->removePlayer(1);
    matchmaking->removePlayer(2);
    matchmaking->removePlayer(3);

    EXPECT_EQ(matchmaking->getQueueSize(), 0);
}

TEST_F(MatchmakingTest, AddRemoveAddSamePlayer) {
    matchmaking->addPlayer(1);
    EXPECT_EQ(matchmaking->getQueueSize(), 1);

    matchmaking->removePlayer(1);
    EXPECT_EQ(matchmaking->getQueueSize(), 0);

    matchmaking->addPlayer(1);  // Add again
    EXPECT_EQ(matchmaking->getQueueSize(), 1);
}

TEST_F(MatchmakingTest, LargeNumberOfPlayers) {
    // Add 100 players
    for (uint32_t i = 1; i <= 100; ++i) {
        matchmaking->addPlayer(i);
    }

    EXPECT_EQ(matchmaking->getQueueSize(), 100);

    int matchCount = 0;
    matchmaking->setMatchCreatedCallback([&](std::shared_ptr<Room> room) { matchCount++; });

    matchmaking->tick();

    // Should create 25 matches (100 players / 4 per match)
    EXPECT_EQ(matchCount, 25);
    EXPECT_EQ(matchmaking->getQueueSize(), 0);
}

TEST_F(MatchmakingTest, MinEqualsMax) {
    matchmaking->setMinPlayers(3);
    matchmaking->setMaxPlayers(3);

    std::shared_ptr<Room> createdRoom = nullptr;
    matchmaking->setMatchCreatedCallback([&](std::shared_ptr<Room> room) { createdRoom = room; });

    matchmaking->addPlayer(1);
    matchmaking->addPlayer(2);
    matchmaking->addPlayer(3);

    matchmaking->tick();

    EXPECT_NE(createdRoom, nullptr);
}

// ============================================================================
// Concurrency Safety Tests (Basic)
// ============================================================================

TEST_F(MatchmakingTest, ConcurrentAddPlayers) {
    std::thread t1([this]() {
        for (uint32_t i = 1; i <= 10; ++i) {
            matchmaking->addPlayer(i);
        }
    });

    std::thread t2([this]() {
        for (uint32_t i = 11; i <= 20; ++i) {
            matchmaking->addPlayer(i);
        }
    });

    t1.join();
    t2.join();

    EXPECT_EQ(matchmaking->getQueueSize(), 20);
}

TEST_F(MatchmakingTest, ConcurrentAddRemove) {
    // Add initial players
    for (uint32_t i = 1; i <= 20; ++i) {
        matchmaking->addPlayer(i);
    }

    std::thread t1([this]() {
        for (uint32_t i = 1; i <= 10; ++i) {
            matchmaking->removePlayer(i);
        }
    });

    std::thread t2([this]() {
        for (uint32_t i = 21; i <= 30; ++i) {
            matchmaking->addPlayer(i);
        }
    });

    t1.join();
    t2.join();

    // Should have 20 players (10 remaining + 10 new)
    EXPECT_EQ(matchmaking->getQueueSize(), 20);
}
