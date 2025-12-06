/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** IEventBus.hpp — Interface for server-side EventBus
*/

#pragma once

#include <cstddef>

namespace Server {

/**
 * @class IEventBus
 * @brief Interface for a type-safe event bus
 * 
 * Defines the basic methods that any EventBus should implement.
 * Derived classes must implement subscribe, publish, and clear.
 */
class IEventBus {
public:
    virtual ~IEventBus() = default;

    /**
     * @brief Subscribe to an event type
     * @tparam T Event type (must inherit from IEvent)
     * @param callback Function called when an event of type T is published
     * @return Subscription identifier
     */
    template <typename T>
    virtual size_t subscribe(void (*callback)(const T &)) = 0;

    /**
     * @brief Publish an event
     * @tparam T Event type (must inherit from IEvent)
     * @param event Event instance to publish
     */
    template <typename T>
    virtual void publish(const T &event) = 0;

    /**
     * @brief Clear all subscriptions
     */
    virtual void clear() = 0;
};

} // namespace Server

