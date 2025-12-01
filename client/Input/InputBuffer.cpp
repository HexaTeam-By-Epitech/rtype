/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** InputBuffer.cpp
*/

#include "InputBuffer.hpp"

void InputBuffer::addInput(uint32_t frameNumber, InputAction action, InputState state) {}

std::vector<InputBuffer::StoredInput> InputBuffer::getInputsSince(uint32_t startFrame) const {
    return {};
}

void InputBuffer::clearUntil(uint32_t frameNumber) {}

std::optional<InputBuffer::StoredInput> InputBuffer::getLastInput() const {
    return std::nullopt;
}

void InputBuffer::clear() {}

size_t InputBuffer::size() const {
    return 0;
}
