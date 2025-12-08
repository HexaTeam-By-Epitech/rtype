/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** IEventBus.hpp
*/

#pragma once

namespace server {

    /**
     * @class IEventBus
     * @brief Minimal interface for the EventBus
     *
     * This interface only exposes the clear() method, as template-based
     * subscribe/publish functions cannot be virtualized. Implementations
     * extending this interface must provide the internal storage and
     * logic for event subscriptions.
     */
    class IEventBus {
       public:
        virtual ~IEventBus() = default;

        /**
         * @brief Remove all event subscriptions
         *
         * Clears every registered handler for all event types.
         * Called typically during shutdown or reinitialization.
         */
        virtual void clear() = 0;
    };

}  // namespace server
