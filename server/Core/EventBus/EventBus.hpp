/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** EventBus.hpp
*/

#pragma once

#include <any>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include "IEventBus.hpp"

namespace server {

    /**
     * @class EventBus
     * @brief Type-safe event bus for the server
     * 
     * EventBus allows different components of the server to communicate via events
     * without introducing direct dependencies. Each event type T can have multiple 
     * subscribers (callbacks) that are invoked when the event is published.
     * 
     * Subscribers are stored in a container keyed by typeid(T) and called in the
     * order they were registered.
     * 
     * Example usage:
     * @code
     * Server::EventBus bus;
     * bus.subscribe<MyEvent>([](const MyEvent &e){ std::cout << e.value << std::endl; });
     * MyEvent evt{42};
     * bus.publish(evt);
     * @endcode
     */
    class EventBus : public IEventBus {
       public:
        EventBus() = default;
        ~EventBus() override = default;

        /**
         * @brief Callback type for an event
         * @tparam T Event type
         * 
         * EventCallback is a function taking a const reference to an event.
         * It will be called whenever the corresponding event type is published.
         */
        template <typename T>
        using EventCallback = std::function<void(const T &)>;

        /**
         * @brief Subscribe to an event type
         * @tparam T Event type
         * @param callback Function to call when an event of type T is published
         * @return Subscription ID
         * 
         * Adds the callback to the list of subscribers for type T.
         * Callbacks are stored using type-erasure with std::any.
         * The wrapper converts std::any back to T before invoking user callbacks.
         */
        template <typename T>
        size_t subscribe(EventCallback<T> callback) {
            auto &vec = _subscribers[std::type_index(typeid(T))];

            vec.push_back([callback = std::move(callback)](const std::any &a) {
                callback(std::any_cast<const T &>(a));
            });

            return vec.size() - 1;
        }

        /**
         * @brief Publish an event
         * @tparam T Event type
         * @param event Event instance to publish
         * 
         * Invokes all subscribers for type T with the provided event.
         * If no subscribers exist, the function does nothing.
         */
        template <typename T>
        void publish(const T &event) {
            auto it = _subscribers.find(std::type_index(typeid(T)));
            if (it == _subscribers.end())
                return;

            const std::any box = event;

            for (auto &cb : it->second)
                cb(box);
        }

        /**
         * @brief Clear all subscriptions
         * 
         * Completely clears the event bus, removing all subscribers for all event types.
         */
        void clear() override { _subscribers.clear(); }

       private:
        /**
         * @brief Container of subscribers per event type
         * 
         * Each event type T is identified by typeid(T). Callbacks are stored using
         * type-erasure (std::function<void(const std::any &)>), allowing multiple
         * types to coexist in the same bus.
         */
        std::unordered_map<std::type_index, std::vector<std::function<void(const std::any &)>>> _subscribers;
    };

}  // namespace server
