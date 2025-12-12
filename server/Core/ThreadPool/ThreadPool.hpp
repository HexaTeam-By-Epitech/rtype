/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** ThreadPool.hpp
*/

#pragma once

#include <atomic>
#include <thread>
#include "../../../common/Threading/ThreadSafeQueue.hpp"
#include "IThreadPool.hpp"

namespace server {

    /**
     * @class ThreadPool
     * @brief Concrete implementation of IThreadPool
     *
     * Manages a fixed number of worker threads executing tasks concurrently.
     * Uses a thread-safe queue to distribute tasks among workers.
     */
    class ThreadPool : public IThreadPool {
       public:
        explicit ThreadPool(size_t threadCount);
        ~ThreadPool() override;

        void enqueue(Task task) override;
        void start() override;
        void stop() override;
        size_t size() const override;

       private:
        /**
         * @brief Worker thread main loop
         * Continuously pulls tasks from the queue and executes them
         */
        void _workerLoop();

        size_t _threadCount;
        std::vector<std::thread> _workers;
        ThreadSafeQueue<Task> _taskQueue;
        std::atomic<bool> _running{false};
    };

}  // namespace server
