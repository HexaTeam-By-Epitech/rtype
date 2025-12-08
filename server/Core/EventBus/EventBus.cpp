/*
** EPITECH PROJECT, 2025
** EventBus.cpp
** Server-side EventBus implementation
*/

#include "EventBus.hpp"

namespace Server {

    /**
     * @file EventBus.cpp
     * @brief Implementation of EventBus class.
     * 
     * This file provides minimal implementations for EventBus member functions.
     * The actual subscription and publishing logic is implemented as templates
     * in the header file, since template functions must be defined in the header.
     */

    /// Clear all subscriptions from the bus
    void EventBus::clear() {
        _subscribers.clear();
    }

}  // namespace Server
