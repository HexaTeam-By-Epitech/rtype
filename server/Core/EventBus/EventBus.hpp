/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** EventBus.hpp — Server-side EventBus
*/

#pragma once

#include "IEventBus.hpp"
#include "../../Events/IEvent.hpp"
#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>

namespace Server {

/**
 * @class EventBus
 * @brief Type-safe event bus
 * 
 * Allows components to subscribe to and publish different event types.
 * Callbacks are stored per event type and invoked in registration order.
 */
class EventBus : public IEventBus {
public:
    EventBus() = default;
    ~EventBus() override = default;

    /**
     * @brief Callback type for an event
     * @tparam T Event type
     */
    template <typename T>
    using EventCallback = std::function<void(const T &)>;

    /**
     * @brief Subscribe to an event
     * @tparam T Event type
     * @param callback Function called when the event is published
     * @return Subscription identifier
     */
    template <typename T>
    size_t subscribe(EventCallback<T> callback);

    /**
     * @brief Publish an event
     * @tparam T Event type
     * @param event Event to publish
     */
    template <typename T>
    void publish(const T &event);

    /**
     * @brief Clear all subscriptions
     */
    void clear() override;

private:
    std::unordered_map<std::type_index, std::vector<std::function<void(const IEvent &)>>> _subscribers;
};

} // namespace Server

