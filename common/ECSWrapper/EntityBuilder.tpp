/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** EntityBuilder template implementations
*/

#pragma once

#include "EntityBuilder.hpp"

namespace ecs::wrapper {

    template <typename T>
    EntityBuilder& EntityBuilder::with(const T& component) {
        _entity.with(component);
        return *this;
    }

    template <typename T>
    EntityBuilder& EntityBuilder::with(std::function<T()> factory) {
        T component = factory();
        _entity.with(component);
        return *this;
    }

}  // namespace ecs::wrapper
