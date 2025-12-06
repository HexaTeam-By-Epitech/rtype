/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** IEvent.hpp
*/

#pragma once

#include <chrono>

namespace server {

/**
 * @class IEvent
 * @brief Base interface for all events
 *
 * Provides timestamp information and serves as a
 * polymorphic base for all event types.
 */
class IEvent {
public:
    IEvent() : _timestamp(std::chrono::system_clock::now()) {}
    virtual ~IEvent() = default;

    /**
     * @brief Get the event creation timestamp
     * @return std::chrono::system_clock::time_point
     */
    std::chrono::system_clock::time_point getTimestamp() const { return _timestamp; }

private:
    std::chrono::system_clock::time_point _timestamp;
};

} // namespace server

