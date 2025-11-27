/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** InputBuffer.hpp
*/

#ifndef INPUTBUFFER_HPP
#define INPUTBUFFER_HPP

#include <cstdint>
#include <deque>
#include <vector>
#include <optional>
#include "../Events/InputEvent/InputEvent.hpp"

class InputBuffer {
public:

    struct StoredInput {
        uint32_t frameNumber;
        InputAction action;
        InputState state;
    };

    InputBuffer() = default;
    ~InputBuffer() = default;

    void addInput(uint32_t frameNumber, InputAction action, InputState state);

    std::vector<StoredInput> getInputsSince(uint32_t startFrame) const;
    void clearUntil(uint32_t frameNumber);
    std::optional<StoredInput> getLastInput() const;
    void clear();
    size_t size() const;

private:
    std::deque<StoredInput> _inputs;
    uint32_t _oldestFrame = 0;
};

#endif