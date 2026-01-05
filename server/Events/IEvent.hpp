/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** IEvent.hpp
*/

#pragma once

namespace server {

    /**
     * @class IEvent
     * @brief Base interface for all events
     *
     * Serves as a polymorphic base for all event types.
     */
    class IEvent {
       public:
        IEvent() = default;
        virtual ~IEvent() = default;
    };

}  // namespace server
