/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 08/12/2025.
** File description:
** ThreadSafeQueueTest.cpp
*/

#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <vector>
#include "ThreadSafeQueue.hpp"

// ============================================================================
// Basic ThreadSafeQueue Tests
// ============================================================================

TEST(ThreadSafeQueueTest, Construction) {
    EXPECT_NO_THROW({ ThreadSafeQueue<int> queue; });
}

TEST(ThreadSafeQueueTest, PushAndPop) {
    ThreadSafeQueue<int> queue;
    queue.push(42);

    auto value = queue.pop();
    EXPECT_EQ(value, 42);
}

TEST(ThreadSafeQueueTest, PushMultipleAndPop) {
    ThreadSafeQueue<int> queue;
    queue.push(1);
    queue.push(2);
    queue.push(3);

    EXPECT_EQ(queue.pop(), 1);
    EXPECT_EQ(queue.pop(), 2);
    EXPECT_EQ(queue.pop(), 3);
}

TEST(ThreadSafeQueueTest, EmptyQueue) {
    ThreadSafeQueue<int> queue;
    EXPECT_TRUE(queue.empty());

    queue.push(1);
    EXPECT_FALSE(queue.empty());

    queue.pop();
    EXPECT_TRUE(queue.empty());
}

TEST(ThreadSafeQueueTest, TryPop) {
    ThreadSafeQueue<int> queue;

    auto value = queue.tryPop();
    EXPECT_FALSE(value.has_value());

    queue.push(99);
    value = queue.tryPop();
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ(*value, 99);
}

TEST(ThreadSafeQueueTest, TryPopOnEmptyQueue) {
    ThreadSafeQueue<std::string> queue;

    auto value = queue.tryPop();
    EXPECT_FALSE(value.has_value());
}

TEST(ThreadSafeQueueTest, PushDifferentTypes) {
    ThreadSafeQueue<std::string> stringQueue;
    stringQueue.push("Hello");
    EXPECT_EQ(stringQueue.pop(), "Hello");

    ThreadSafeQueue<double> doubleQueue;
    doubleQueue.push(3.14);
    EXPECT_DOUBLE_EQ(doubleQueue.pop(), 3.14);

    ThreadSafeQueue<bool> boolQueue;
    boolQueue.push(true);
    EXPECT_TRUE(boolQueue.pop());
}

// ============================================================================
// Multithreading Tests
// ============================================================================

TEST(ThreadSafeQueueTest, MultipleProducers) {
    ThreadSafeQueue<int> queue;
    const int numThreads = 10;
    const int itemsPerThread = 100;

    std::vector<std::thread> threads;
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&queue, t, itemsPerThread]() {
            for (int i = 0; i < itemsPerThread; ++i) {
                queue.push(t * itemsPerThread + i);
            }
        });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    // Verify all items are in the queue
    int count = 0;
    while (!queue.empty()) {
        queue.pop();
        ++count;
    }

    EXPECT_EQ(count, numThreads * itemsPerThread);
}

TEST(ThreadSafeQueueTest, MultipleConsumers) {
    ThreadSafeQueue<int> queue;
    const int totalItems = 1000;

    // Fill queue
    for (int i = 0; i < totalItems; ++i) {
        queue.push(i);
    }

    std::atomic<int> consumedCount{0};
    const int numThreads = 10;

    std::vector<std::thread> threads;
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&queue, &consumedCount]() {
            while (auto value = queue.tryPop()) {
                ++consumedCount;
            }
        });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    EXPECT_EQ(consumedCount, totalItems);
}

TEST(ThreadSafeQueueTest, ProducerConsumer) {
    ThreadSafeQueue<int> queue;
    const int itemCount = 1000;
    std::atomic<bool> done{false};
    std::atomic<int> consumedSum{0};

    // Producer thread
    std::thread producer([&queue, itemCount, &done]() {
        for (int i = 0; i < itemCount; ++i) {
            queue.push(i);
        }
        done = true;
    });

    // Consumer thread
    std::thread consumer([&queue, &done, &consumedSum]() {
        while (!done || !queue.empty()) {
            auto value = queue.tryPop();
            if (value.has_value()) {
                consumedSum += *value;
            } else {
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();

    // Sum of 0 to itemCount-1 = itemCount * (itemCount - 1) / 2
    int expectedSum = itemCount * (itemCount - 1) / 2;
    EXPECT_EQ(consumedSum, expectedSum);
}

TEST(ThreadSafeQueueTest, MultipleProducersConsumers) {
    ThreadSafeQueue<int> queue;
    const int numProducers = 5;
    const int numConsumers = 5;
    const int itemsPerProducer = 200;
    std::atomic<int> consumedCount{0};
    std::atomic<bool> productionDone{false};

    // Producer threads
    std::vector<std::thread> producers;
    for (int p = 0; p < numProducers; ++p) {
        producers.emplace_back([&queue, p, itemsPerProducer]() {
            for (int i = 0; i < itemsPerProducer; ++i) {
                queue.push(p * itemsPerProducer + i);
            }
        });
    }

    // Consumer threads
    std::vector<std::thread> consumers;
    for (int c = 0; c < numConsumers; ++c) {
        consumers.emplace_back([&queue, &consumedCount, &productionDone]() {
            while (!productionDone || !queue.empty()) {
                auto value = queue.tryPop();
                if (value.has_value()) {
                    ++consumedCount;
                } else {
                    std::this_thread::yield();
                }
            }
        });
    }

    // Wait for producers
    for (auto &thread : producers) {
        thread.join();
    }
    productionDone = true;

    // Wait for consumers
    for (auto &thread : consumers) {
        thread.join();
    }

    EXPECT_EQ(consumedCount, numProducers * itemsPerProducer);
}

TEST(ThreadSafeQueueTest, ConcurrentPushPop) {
    ThreadSafeQueue<int> queue;
    const int operations = 10000;
    std::atomic<int> balance{0};

    // Thread that pushes and increments balance
    std::thread pusher([&queue, &balance, operations]() {
        for (int i = 0; i < operations; ++i) {
            queue.push(1);
            ++balance;
        }
    });

    // Thread that pops and decrements balance
    std::thread popper([&queue, &balance, operations]() {
        for (int i = 0; i < operations; ++i) {
            while (true) {
                auto value = queue.tryPop();
                if (value.has_value()) {
                    --balance;
                    break;
                }
                std::this_thread::yield();
            }
        }
    });

    pusher.join();
    popper.join();

    EXPECT_EQ(balance, 0);
    EXPECT_TRUE(queue.empty());
}

// ============================================================================
// Stress Tests
// ============================================================================

TEST(ThreadSafeQueueTest, HighVolumeStressTest) {
    ThreadSafeQueue<int> queue;
    const int itemCount = 100000;

    std::thread producer([&queue, itemCount]() {
        for (int i = 0; i < itemCount; ++i) {
            queue.push(i);
        }
    });

    std::thread consumer([&queue, itemCount]() {
        int received = 0;
        while (received < itemCount) {
            auto value = queue.tryPop();
            if (value.has_value()) {
                ++received;
            }
        }
    });

    producer.join();
    consumer.join();

    EXPECT_TRUE(queue.empty());
}

TEST(ThreadSafeQueueTest, ComplexDataType) {
    struct ComplexData {
        int id;
        std::string name;
        std::vector<double> values;

        bool operator==(const ComplexData &other) const {
            return id == other.id && name == other.name && values == other.values;
        }
    };

    ThreadSafeQueue<ComplexData> queue;

    ComplexData data1{1, "Test", {1.1, 2.2, 3.3}};
    ComplexData data2{2, "Another", {4.4, 5.5}};

    queue.push(data1);
    queue.push(data2);

    auto result1 = queue.pop();
    auto result2 = queue.pop();

    EXPECT_EQ(result1, data1);
    EXPECT_EQ(result2, data2);
}

TEST(ThreadSafeQueueTest, MoveSemantics) {
    ThreadSafeQueue<std::unique_ptr<int>> queue;

    auto ptr = std::make_unique<int>(42);
    queue.push(std::move(ptr));

    auto result = queue.pop();
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(*result, 42);
}

TEST(ThreadSafeQueueTest, EmptyCheckThreadSafety) {
    ThreadSafeQueue<int> queue;
    std::atomic<bool> stop{false};
    std::atomic<int> emptyCount{0};
    std::atomic<int> notEmptyCount{0};

    std::thread checker([&queue, &stop, &emptyCount, &notEmptyCount]() {
        while (!stop) {
            if (queue.empty()) {
                ++emptyCount;
            } else {
                ++notEmptyCount;
            }
        }
    });

    std::thread modifier([&queue]() {
        for (int i = 0; i < 1000; ++i) {
            queue.push(i);
            queue.tryPop();
        }
    });

    modifier.join();
    stop = true;
    checker.join();

    // Just verify no crashes occurred
    EXPECT_GE(emptyCount + notEmptyCount, 0);
}
