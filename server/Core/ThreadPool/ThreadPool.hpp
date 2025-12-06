/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** ThreadPool.hpp
*/

#pragma once

#include "IThreadPool.hpp"

namespace server {

/**
 * @class ThreadPool
 * @brief Concrete implementation of IThreadPool
 *
 * Manages a fixed number of worker threads executing tasks concurrently.
 */
class ThreadPool : public IThreadPool {
public:
    ThreadPool(size_t threadCount);
    ~ThreadPool() override;

    void enqueue(Task task) override;
    void start() override;
    void stop() override;
    size_t size() const override;

private:
    size_t _threadCount;
    std::vector<std::thread> _workers;
};

} // namespace server

