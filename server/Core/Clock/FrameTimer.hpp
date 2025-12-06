/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** FrameTimer.hpp
*/

#pragma once

#include <chrono>

namespace server {

/**
 * @class FrameTimer
 * @brief Utility class to measure frame durations
 *
 * Provides a way to track elapsed time between frames.
 */
class FrameTimer {
public:
    FrameTimer() = default;
    ~FrameTimer() = default;

    /**
     * @brief Reset the timer to the current time
     */
    void reset();

    /**
     * @brief Get the elapsed time in seconds since last reset
     * @return elapsed time in seconds
     */
    double elapsed() const;

private:
    std::chrono::steady_clock::time_point _start;
};

} // namespace server

