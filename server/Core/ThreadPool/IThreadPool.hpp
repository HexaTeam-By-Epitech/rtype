/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** IThreadPool.hpp
*/

#pragma once

#include <functional>
#include <vector>
#include <memory>

namespace server {

/**
 * @class IThreadPool
 * @brief Interface for a thread pool
 *
 * Defines the basic API for a thread pool managing multiple worker threads.
 */
class IThreadPool {
public:
    using Task = std::function<void()>;

    virtual ~IThreadPool() = default;

    /**
     * @brief Enqueue a task to be executed by the thread pool
     * @param task Task function
     */
    virtual void enqueue(Task task) = 0;

    /**
     * @brief Start all threads in the pool
     */
    virtual void start() = 0;

    /**
     * @brief Stop all threads and clean up resources
     */
    virtual void stop() = 0;

    /**
     * @brief Get the number of threads in the pool
     * @return number of threads
     */
    virtual size_t size() const = 0;
};

} // namespace server

