/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** Clock.hpp
*/

#pragma once

#include "server/Core/Clock/FrameTimer.hpp"

namespace server {

    /**
     * @class Clock
     * @brief Main clock for server timing
     *
     * Manages frame timing and provides access to a FrameTimer.
     */
    class Clock {
       public:
        Clock() = default;
        ~Clock() = default;

        /**
         * @brief Get reference to the frame timer
         * @return FrameTimer reference
         */
        FrameTimer &getFrameTimer() { return _frameTimer; }

       private:
        FrameTimer _frameTimer;
    };

}  // namespace server
