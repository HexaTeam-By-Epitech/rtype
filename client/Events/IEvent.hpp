/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** IEvent.hpp
*/

#ifndef IEVENT_HPP_
#define IEVENT_HPP_

/**
 * @interface IEvent
 * @brief Base interface for all event types
 * 
 * IEvent is the root interface that all event types must inherit from.
 * It enables type-safe event handling through the EventBus system.
 * 
 * Usage:
 * - All custom events must inherit from IEvent
 * - Provides polymorphic behavior for event handling
 * - Enables runtime type identification via templates
 * 
 * @note This is a pure interface with no data or behavior
 * @see EventBus for event distribution mechanism
 */
class IEvent {
   public:
    /**
     * @brief Virtual destructor
     * 
     * Ensures proper cleanup of derived event types.
     */
    virtual ~IEvent() = default;
};

#endif
