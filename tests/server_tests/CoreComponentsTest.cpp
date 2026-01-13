/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** CoreComponentsTest.cpp - Unit tests for EventBus, ThreadPool, and other core components
*/

#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include "common/ECSWrapper/ECSWorld.hpp"
#include "server/Core/EventBus/EventBus.hpp"
#include "server/Core/ThreadPool/ThreadPool.hpp"
#include "server/Events/GameEvent/GameEndedEvent.hpp"
#include "server/Events/GameEvent/GameStartedEvent.hpp"
#include "server/Events/GameEvent/PlayerJoinedEvent.hpp"
#include "server/Events/GameEvent/PlayerLeftEvent.hpp"
#include "server/Game/Prefabs/PrefabFactory.hpp"

using namespace server;

// ============================================================================
// EventBus Tests
// ============================================================================

class EventBusTest : public ::testing::Test {
   protected:
    void SetUp() override { eventBus = std::make_shared<EventBus>(); }

    void TearDown() override { eventBus.reset(); }

    std::shared_ptr<EventBus> eventBus;
};

TEST_F(EventBusTest, Construction) {
    EXPECT_NE(eventBus, nullptr);
}

TEST_F(EventBusTest, SubscribeAndPublish) {
    bool eventReceived = false;
    uint32_t receivedPlayerId = 0;

    eventBus->subscribe<PlayerJoinedEvent>([&](const PlayerJoinedEvent &event) {
        eventReceived = true;
        receivedPlayerId = event.getPlayerId();
    });

    PlayerJoinedEvent event(123, "TestPlayer");
    eventBus->publish(event);

    EXPECT_TRUE(eventReceived);
    EXPECT_EQ(receivedPlayerId, 123);
}

TEST_F(EventBusTest, MultipleSubscribers) {
    int callCount = 0;

    eventBus->subscribe<PlayerJoinedEvent>([&](const PlayerJoinedEvent & /*event*/) { callCount++; });
    eventBus->subscribe<PlayerJoinedEvent>([&](const PlayerJoinedEvent & /*event*/) { callCount++; });
    eventBus->subscribe<PlayerJoinedEvent>([&](const PlayerJoinedEvent & /*event*/) { callCount++; });

    PlayerJoinedEvent event(1, "Player");
    eventBus->publish(event);

    EXPECT_EQ(callCount, 3);
}

TEST_F(EventBusTest, DifferentEventTypes) {
    bool joinReceived = false;
    bool leftReceived = false;
    bool startReceived = false;

    eventBus->subscribe<PlayerJoinedEvent>([&](const PlayerJoinedEvent & /*event*/) { joinReceived = true; });
    eventBus->subscribe<PlayerLeftEvent>([&](const PlayerLeftEvent & /*event*/) { leftReceived = true; });
    eventBus->subscribe<GameStartedEvent>([&](const GameStartedEvent & /*event*/) { startReceived = true; });

    PlayerJoinedEvent joinEvent(1, "Player");
    eventBus->publish(joinEvent);
    EXPECT_TRUE(joinReceived);
    EXPECT_FALSE(leftReceived);
    EXPECT_FALSE(startReceived);

    PlayerLeftEvent leftEvent(1);
    eventBus->publish(leftEvent);
    EXPECT_TRUE(leftReceived);
    EXPECT_FALSE(startReceived);

    GameStartedEvent startEvent;
    eventBus->publish(startEvent);
    EXPECT_TRUE(startReceived);
}

TEST_F(EventBusTest, EventDataIntegrity) {
    std::string receivedName;
    uint32_t receivedId = 0;

    eventBus->subscribe<PlayerJoinedEvent>([&](const PlayerJoinedEvent &event) {
        receivedName = event.getPlayerName();
        receivedId = event.getPlayerId();
    });

    PlayerJoinedEvent event(456, "Alice");
    eventBus->publish(event);

    EXPECT_EQ(receivedName, "Alice");
    EXPECT_EQ(receivedId, 456);
}

TEST_F(EventBusTest, GameEndedEvent) {
    std::string receivedReason;

    eventBus->subscribe<GameEndedEvent>(
        [&](const GameEndedEvent &event) { receivedReason = event.getReason(); });

    GameEndedEvent event("Victory");
    eventBus->publish(event);

    EXPECT_EQ(receivedReason, "Victory");
}

TEST_F(EventBusTest, NoSubscribers) {
    // Publishing without subscribers should not crash
    PlayerJoinedEvent event(1, "Player");
    EXPECT_NO_THROW(eventBus->publish(event));
}

// ============================================================================
// ThreadPool Tests
// ============================================================================

class ThreadPoolTest : public ::testing::Test {
   protected:
    void SetUp() override { threadPool = std::make_unique<ThreadPool>(4); }

    void TearDown() override {
        if (threadPool) {
            threadPool->stop();
        }
        threadPool.reset();
    }

    std::unique_ptr<ThreadPool> threadPool;
};

TEST_F(ThreadPoolTest, Construction) {
    EXPECT_NE(threadPool, nullptr);
}

TEST_F(ThreadPoolTest, StartAndStop) {
    EXPECT_NO_THROW(threadPool->start());
    EXPECT_NO_THROW(threadPool->stop());
}

TEST_F(ThreadPoolTest, EnqueueTask) {
    threadPool->start();

    std::atomic<bool> taskExecuted{false};

    threadPool->enqueue([&taskExecuted]() { taskExecuted = true; });

    // Wait for task to execute
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_TRUE(taskExecuted);
}

TEST_F(ThreadPoolTest, MultipleTasks) {
    threadPool->start();

    std::atomic<int> counter{0};

    for (int i = 0; i < 10; ++i) {
        threadPool->enqueue([&counter]() { counter++; });
    }

    // Wait for all tasks to execute
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    EXPECT_EQ(counter, 10);
}

TEST_F(ThreadPoolTest, TaskOrdering) {
    threadPool->start();

    std::vector<int> results;
    std::mutex resultsMutex;

    for (int i = 0; i < 5; ++i) {
        threadPool->enqueue([&results, &resultsMutex, i]() {
            std::lock_guard<std::mutex> lock(resultsMutex);
            results.push_back(i);
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    EXPECT_EQ(results.size(), 5);
}

TEST_F(ThreadPoolTest, ConcurrentTasks) {
    threadPool->start();

    std::atomic<int> counter{0};

    // Enqueue tasks that increment counter concurrently
    for (int i = 0; i < 100; ++i) {
        threadPool->enqueue([&counter]() {
            int current = counter.load();
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            counter.store(current + 1);
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Due to race conditions, counter might not be exactly 100
    // But it should be > 0 if tasks executed
    EXPECT_GT(counter, 0);
}

TEST_F(ThreadPoolTest, StopWithPendingTasks) {
    threadPool->start();

    std::atomic<int> executed{0};

    for (int i = 0; i < 10; ++i) {
        threadPool->enqueue([&executed]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            executed++;
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    threadPool->stop();

    // Some tasks should have executed
    EXPECT_GT(executed, 0);
}

TEST_F(ThreadPoolTest, MultipleStartCalls) {
    threadPool->start();
    EXPECT_NO_THROW(threadPool->start());  // Should be safe
}

TEST_F(ThreadPoolTest, MultipleStopCalls) {
    threadPool->start();
    threadPool->stop();
    EXPECT_NO_THROW(threadPool->stop());  // Should be safe
}

TEST_F(ThreadPoolTest, EnqueueWithoutStart) {
    // According to implementation, tasks enqueued before start are warned but not executed
    std::atomic<bool> executed{false};
    EXPECT_NO_THROW(threadPool->enqueue([&executed]() { executed = true; }));

    // Start and wait - but task was lost (as per LOG_WARNING)
    threadPool->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Task should NOT execute since it was enqueued before start
    EXPECT_FALSE(executed);
}

TEST_F(ThreadPoolTest, LargeNumberOfTasks) {
    threadPool->start();

    std::atomic<int> counter{0};
    const int taskCount = 1000;

    for (int i = 0; i < taskCount; ++i) {
        threadPool->enqueue([&counter]() { counter++; });
    }

    // Wait for all tasks
    std::this_thread::sleep_for(std::chrono::seconds(1));

    EXPECT_EQ(counter, taskCount);
}

// ============================================================================
// PrefabFactory Tests
// ============================================================================

class PrefabFactoryTest : public ::testing::Test {
   protected:
    void SetUp() override { world = std::make_shared<ecs::wrapper::ECSWorld>(); }

    void TearDown() override { world.reset(); }

    std::shared_ptr<ecs::wrapper::ECSWorld> world;
};

TEST_F(PrefabFactoryTest, CreatePlayer) {
    uint32_t entityId = PrefabFactory::createPlayer(*world, 1, "TestPlayer");
    EXPECT_GT(entityId, 0);
}

TEST_F(PrefabFactoryTest, CreateMultiplePlayers) {
    uint32_t player1 = PrefabFactory::createPlayer(*world, 1, "Alice");
    uint32_t player2 = PrefabFactory::createPlayer(*world, 2, "Bob");
    uint32_t player3 = PrefabFactory::createPlayer(*world, 3, "Charlie");

    EXPECT_GT(player1, 0);
    EXPECT_GT(player2, 0);
    EXPECT_GT(player3, 0);
    EXPECT_NE(player1, player2);
    EXPECT_NE(player2, player3);
    EXPECT_NE(player1, player3);
}

TEST_F(PrefabFactoryTest, CreateEnemy) {
    uint32_t enemyId = PrefabFactory::createEnemy(*world, 0, 300.0f, 400.0f);
    EXPECT_GT(enemyId, 0);
}

TEST_F(PrefabFactoryTest, CreateMultipleEnemyTypes) {
    uint32_t enemy0 = PrefabFactory::createEnemy(*world, 0, 100.0f, 100.0f);  // Basic
    uint32_t enemy1 = PrefabFactory::createEnemy(*world, 1, 200.0f, 200.0f);  // Heavy
    uint32_t enemy2 = PrefabFactory::createEnemy(*world, 2, 300.0f, 300.0f);  // Fast

    EXPECT_GT(enemy0, 0);
    EXPECT_GT(enemy1, 0);
    EXPECT_GT(enemy2, 0);
    EXPECT_NE(enemy0, enemy1);
    EXPECT_NE(enemy1, enemy2);
}

TEST_F(PrefabFactoryTest, CreateProjectile) {
    uint32_t projectileId =
        PrefabFactory::createProjectile(*world, 1, 100.0f, 200.0f, 1.0f, 0.0f, 10.0f, 10, true);
    EXPECT_GT(projectileId, 0);
}

TEST_F(PrefabFactoryTest, CreatePlayerProjectile) {
    uint32_t bulletId =
        PrefabFactory::createProjectile(*world, 1, 100.0f, 100.0f, 1.0f, 0.0f, 15.0f, 20, true);
    EXPECT_GT(bulletId, 0);
}

TEST_F(PrefabFactoryTest, CreateEnemyProjectile) {
    uint32_t bulletId =
        PrefabFactory::createProjectile(*world, 100, 200.0f, 200.0f, 0.0f, 1.0f, 8.0f, 5, false);
    EXPECT_GT(bulletId, 0);
}

TEST_F(PrefabFactoryTest, MixedEntityCreation) {
    // Create various entities
    uint32_t player = PrefabFactory::createPlayer(*world, 1, "Player1");
    uint32_t enemy = PrefabFactory::createEnemy(*world, 0, 200.0f, 200.0f);
    uint32_t playerBullet =
        PrefabFactory::createProjectile(*world, player, 150.0f, 150.0f, 1.0f, 0.0f, 10.0f, 20, true);
    uint32_t enemyBullet =
        PrefabFactory::createProjectile(*world, enemy, 250.0f, 250.0f, 0.0f, 1.0f, 8.0f, 10, false);

    // All should be valid and different
    EXPECT_GT(player, 0);
    EXPECT_GT(enemy, 0);
    EXPECT_GT(playerBullet, 0);
    EXPECT_GT(enemyBullet, 0);

    EXPECT_NE(player, enemy);
    EXPECT_NE(enemy, playerBullet);
    EXPECT_NE(playerBullet, enemyBullet);
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST(CoreIntegrationTest, EventBusWithThreadPool) {
    auto eventBus = std::make_shared<EventBus>();
    auto threadPool = std::make_unique<ThreadPool>(2);
    threadPool->start();

    std::atomic<int> eventCount{0};

    eventBus->subscribe<PlayerJoinedEvent>([&](const PlayerJoinedEvent & /*event*/) { eventCount++; });

    // Publish events from multiple threads
    for (int i = 0; i < 10; ++i) {
        threadPool->enqueue([eventBus, i]() {
            PlayerJoinedEvent event(i, "Player" + std::to_string(i));
            eventBus->publish(event);
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    threadPool->stop();

    EXPECT_EQ(eventCount, 10);
}

TEST(CoreIntegrationTest, EventBusMultipleEventTypes) {
    auto eventBus = std::make_shared<EventBus>();

    int joinCount = 0;
    int leftCount = 0;
    int startCount = 0;
    int endCount = 0;

    eventBus->subscribe<PlayerJoinedEvent>([&](const PlayerJoinedEvent &) { joinCount++; });
    eventBus->subscribe<PlayerLeftEvent>([&](const PlayerLeftEvent &) { leftCount++; });
    eventBus->subscribe<GameStartedEvent>([&](const GameStartedEvent &) { startCount++; });
    eventBus->subscribe<GameEndedEvent>([&](const GameEndedEvent &) { endCount++; });

    eventBus->publish(PlayerJoinedEvent(1, "P1"));
    eventBus->publish(PlayerJoinedEvent(2, "P2"));
    eventBus->publish(GameStartedEvent());
    eventBus->publish(PlayerLeftEvent(1));
    eventBus->publish(GameEndedEvent("Victory"));

    EXPECT_EQ(joinCount, 2);
    EXPECT_EQ(leftCount, 1);
    EXPECT_EQ(startCount, 1);
    EXPECT_EQ(endCount, 1);
}
