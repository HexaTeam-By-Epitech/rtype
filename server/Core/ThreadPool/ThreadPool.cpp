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
        try {
            ThreadPool::stop();
            // jthread automatically requests stop and joins in its destructor
        } catch (const std::exception &e) {
            LOG_ERROR("ThreadPool destructor caught exception: ", e.what());
        }
    }

    void ThreadPool::start() {
        if (!_workers.empty()) {
            LOG_WARNING("ThreadPool already running");
            return;  // Already started
        }

        LOG_INFO("Starting ThreadPool with ", _threadCount, " workers...");

        _workers.reserve(_threadCount);
        for (size_t i = 0; i < _threadCount; ++i) {
            // jthread automatically passes stop_token to the lambda
            _workers.emplace_back([this, i](std::stop_token stopToken) {
                LOG_DEBUG("Worker thread ", i, " started (TID: ", std::this_thread::get_id(), ")");
                _workerLoop(stopToken);
                LOG_DEBUG("Worker thread ", i, " exiting");
            });
        }

        LOG_INFO("✓ ThreadPool started with ", _threadCount, " workers");
    }

    void ThreadPool::stop() {
        if (_workers.empty()) {
            return;  // Already stopped
        }

        LOG_INFO("Stopping ThreadPool...");

        // Request stop for all worker threads
        for (auto &worker : _workers) {
            worker.request_stop();
        }

        // Wake up all workers by pushing poison pills (in case they're blocked on pop())
        for (size_t i = 0; i < _threadCount; ++i) {
            _taskQueue.push(nullptr);
        }

        // jthread joins automatically in destructor, but we join explicitly here for clean shutdown
        _workers.clear();  // This will trigger join on all jthreads

        LOG_INFO("✓ ThreadPool stopped");
    }

    void ThreadPool::enqueue(Task task) {
        if (_workers.empty()) {
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

    void ThreadPool::_workerLoop(std::stop_token stopToken) {
        while (!stopToken.stop_requested()) {
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
