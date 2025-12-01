/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** EventBus.hpp
*/

#ifndef EVENTBUS_HPP
#define EVENTBUS_HPP

#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include "../../Events/IEvent.hpp"

/**
 * @class EventBus
 * @brief Type-safe event publication/subscription system
 * 
 * EventBus enables decoupled communication between different game components.
 * It uses the Observer pattern to distribute events to interested subscribers.
 * 
 * Architecture:
 * - Components subscribe to specific event types
 * - When an event is published, all concerned subscribers are notified
 * - Type-safe thanks to C++ templates
 * 
 * Usage example:
 * @code
 * EventBus eventBus;
 * 
 * // Subscribe to an event
 * eventBus.subscribe<InputEvent>([](const InputEvent& e) {
 *     std::cout << "Input received!" << std::endl;
 * });
 * 
 * // Publish an event
 * eventBus.publish(InputEvent(InputAction::SHOOT, InputState::PRESSED));
 * @endcode
 */
class EventBus {
   public:
    /**
     * @brief Callback type for events
     * @tparam T Event type (must inherit from IEvent)
     */
    template <typename T>
    using EventCallback = std::function<void(const T &)>;

    /**
     * @brief Default constructor
     */
    EventBus() = default;

    /**
     * @brief Default destructor
     */
    ~EventBus() = default;

    /**
     * @brief Subscribe to a specific event type
     * 
     * Registers a callback that will be called each time an event
     * of type T is published on this bus.
     * 
     * @tparam T Event type (must inherit from IEvent)
     * @param callback Function called when event is published
     * @return Subscription identifier (for future unsubscription)
     * 
     * @note A component can subscribe multiple times to the same type
     * @note Callbacks are called in registration order
     */
    template <typename T>
    size_t subscribe(EventCallback<T> callback);

    /**
     * @brief Publish an event to all subscribers
     * 
     * Notifies all registered callbacks for this event type.
     * Callbacks are called synchronously in registration order.
     * 
     * @tparam T Event type (must inherit from IEvent)
     * @param event Event to publish
     * 
     * @note If no subscriber exists, the event is ignored
     * @note Exceptions in callbacks are not handled
     */
    template <typename T>
    void publish(const T &event);

    /**
     * @brief Clear all subscriptions
     * 
     * Removes all registered callbacks for all event types.
     * Useful for system reset or avoiding memory leaks.
     */
    void clear();

   private:
    std::unordered_map<std::type_index, std::vector<std::function<void(const IEvent &)>>> _subscribers;
};

#endif