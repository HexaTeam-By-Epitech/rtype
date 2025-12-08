/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** Clock.cpp
*/

#include "Clock.hpp"

namespace server {

    FrameTimer &Clock::getFrameTimer() {
        return _frameTimer;
    }

}  // namespace server
