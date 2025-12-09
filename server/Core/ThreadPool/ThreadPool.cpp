/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** ThreadPool.cpp
*/

#include "ThreadPool.hpp"
#include <thread>

namespace server {

    ThreadPool::ThreadPool(size_t threadCount) : _threadCount(threadCount) {}

    ThreadPool::~ThreadPool() {}

    void ThreadPool::enqueue([[maybe_unused]] Task task) {
        // Prototype only: implementation will be added later
    }

    void ThreadPool::start() {
        // Prototype only: implementation will be added later
    }

    void ThreadPool::stop() {
        // Prototype only: implementation will be added later
    }

    size_t ThreadPool::size() const {
        return _threadCount;
    }

}  // namespace server
