/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** FrameTimer.hpp
*/

#pragma once

#include <chrono>
#include <thread>

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
        void reset() { _start = std::chrono::steady_clock::now(); }

        /**
         * @brief Get the elapsed time in seconds since last reset
         * @return elapsed time in seconds
         */
        double elapsed() const {
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - _start);
            return duration.count() / 1000000.0;
        }

        /**
         * @brief Get elapsed time and automatically reset (optimized for game loops)
         * @return elapsed time in seconds
         */
        double tick() {
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - _start);
            _start = now;  // Reset immediately
            return duration.count() / 1000000.0;
        }

        /**
         * @brief Sleep for specified milliseconds (centralized time management)
         * @param milliseconds Duration to sleep
         */
        static void sleepMilliseconds(int milliseconds) {
            std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        }

       private:
        std::chrono::steady_clock::time_point _start{std::chrono::steady_clock::now()};
    };

}  // namespace server
