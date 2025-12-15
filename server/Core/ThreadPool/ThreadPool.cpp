/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** ThreadPool.cpp
*/

#include "server/Core/ThreadPool/ThreadPool.hpp"
#include "common/Logger/Logger.hpp"

namespace server {

    ThreadPool::ThreadPool(size_t threadCount) : _threadCount(threadCount) {
        LOG_DEBUG("ThreadPool created with ", _threadCount, " threads");
    }

    ThreadPool::~ThreadPool() {
        stop();
    }

    void ThreadPool::start() {
        if (_running.exchange(true)) {
            LOG_WARNING("ThreadPool already running");
            return;  // Already started
        }

        LOG_INFO("Starting ThreadPool with ", _threadCount, " workers...");

        _workers.reserve(_threadCount);
        for (size_t i = 0; i < _threadCount; ++i) {
            _workers.emplace_back([this, i] {
                LOG_DEBUG("Worker thread ", i, " started (TID: ", std::this_thread::get_id(), ")");
                _workerLoop();
                LOG_DEBUG("Worker thread ", i, " exiting");
            });
        }

        LOG_INFO("✓ ThreadPool started with ", _threadCount, " workers");
    }

    void ThreadPool::stop() {
        if (!_running.exchange(false)) {
            return;  // Already stopped
        }

        LOG_INFO("Stopping ThreadPool...");

        // Enqueue poison pills to wake up all workers
        for (size_t i = 0; i < _threadCount; ++i) {
            _taskQueue.push(nullptr);
        }

        // Wait for all workers to finish
        for (auto &worker : _workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }

        _workers.clear();
        LOG_INFO("✓ ThreadPool stopped");
    }

    void ThreadPool::enqueue(Task task) {
        if (!_running) {
            LOG_WARNING("Enqueuing task to stopped ThreadPool - task will not execute");
            return;
        }

        if (!task) {
            LOG_WARNING("Attempted to enqueue null task");
            return;
        }

        _taskQueue.push(std::move(task));
    }

    size_t ThreadPool::size() const {
        return _threadCount;
    }

    void ThreadPool::_workerLoop() {
        while (_running) {
            // Block until a task is available
            Task task = _taskQueue.pop();

            // nullptr is poison pill (shutdown signal)
            if (!task) {
                break;
            }

            try {
                task();
            } catch (const std::exception &e) {
                LOG_ERROR("Worker thread caught exception: ", e.what());
            } catch (...) {
                LOG_ERROR("Worker thread caught unknown exception");
            }
        }
    }

}  // namespace server
