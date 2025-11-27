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

class EventBus {
public:
    template <typename T>
    using EventCallback = std::function<void(const T&)>;

    EventBus() = default;
    ~EventBus() = default;

    template <typename T>
    size_t subscribe(EventCallback<T> callback);

    template <typename T>
    void publish(const T& event);

    void clear();

private:
    std::unordered_map<std::type_index, std::vector<std::function<void(const IEvent&)>>> _subscribers;
};

#endif