/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** EventBus.cpp — Minimal server-side implementation
*/

#include "EventBus.hpp"

namespace Server {

// Empty prototypes for initial compilation
template <typename T>
size_t EventBus::subscribe(EventCallback<T> callback)
{
    (void)callback;
    return 0;
}

template <typename T>
void EventBus::publish(const T &event)
{
    (void)event;
}

void EventBus::clear()
{
    _subscribers.clear();
}

} // namespace Server

