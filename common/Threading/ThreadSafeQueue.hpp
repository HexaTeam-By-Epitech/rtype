/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** ThreadSafeQueue.hpp
*/

#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

/**
 * @class ThreadSafeQueue
 * @brief Thread-safe queue for inter-thread communication
 * 
 * This queue allows one thread to push items and another to pop them
 * without race conditions. Uses mutex and condition variable for synchronization.
 * 
 * @tparam T Type of elements stored in the queue
 */
template <typename T>
class ThreadSafeQueue {
   public:
    ThreadSafeQueue() = default;
    ~ThreadSafeQueue() = default;

    // Delete copy constructor and assignment
    ThreadSafeQueue(const ThreadSafeQueue &) = delete;
    ThreadSafeQueue &operator=(const ThreadSafeQueue &) = delete;

    /**
     * @brief Push an item to the queue
     * @param item Item to push
     * 
     * Thread-safe. Notifies waiting threads.
     */
    void push(T item) {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(std::move(item));
        _cv.notify_one();
    }

    /**
     * @brief Try to pop an item without blocking
     * @return Item if available, std::nullopt otherwise
     * 
     * Thread-safe. Returns immediately.
     */
    std::optional<T> tryPop() {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_queue.empty()) {
            return std::nullopt;
        }
        T item = std::move(_queue.front());
        _queue.pop();
        return item;
    }

    /**
     * @brief Pop an item, blocking until one is available
     * @return Item from the queue
     * 
     * Thread-safe. Blocks until an item is available.
     */
    T pop() {
        std::unique_lock<std::mutex> lock(_mutex);
        _cv.wait(lock, [this] { return !_queue.empty(); });
        T item = std::move(_queue.front());
        _queue.pop();
        return item;
    }

    /**
     * @brief Check if queue is empty
     * @return true if empty, false otherwise
     * 
     * Thread-safe.
     */
    bool empty() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.empty();
    }

    /**
     * @brief Get queue size
     * @return Number of items in queue
     * 
     * Thread-safe.
     */
    size_t size() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.size();
    }

    /**
     * @brief Clear all items from queue
     * 
     * Thread-safe.
     */
    void clear() {
        std::lock_guard<std::mutex> lock(_mutex);
        std::queue<T> emptyQueue;
        std::swap(_queue, emptyQueue);
    }

   private:
    mutable std::mutex _mutex;
    std::condition_variable _cv;
    std::queue<T> _queue;
};