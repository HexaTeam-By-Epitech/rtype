/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** InputBuffer.cpp
*/

#include "InputBuffer.hpp"

void InputBuffer::addInput(uint32_t frameNumber, InputAction action, InputState state) {
    if (_inputs.empty() || frameNumber >= _oldestFrame) {
        _inputs.push_back({frameNumber, action, state});
        if (_inputs.size() == 1) {
            _oldestFrame = frameNumber;
        }
    } else {
        auto it = _inputs.begin();
        while (it != _inputs.end() && it->frameNumber < frameNumber) {
            ++it;
        }
        _inputs.insert(it, {frameNumber, action, state});
        if (frameNumber < _oldestFrame) {
            _oldestFrame = frameNumber;
        }
    }
}

std::vector<InputBuffer::StoredInput> InputBuffer::getInputsSince(uint32_t startFrame) const {
    std::vector<StoredInput> result;

    for (const auto &input : _inputs) {
        if (input.frameNumber >= startFrame) {
            result.push_back(input);
        }
    }

    return result;
}

void InputBuffer::clearUntil(uint32_t frameNumber) {
    while (!_inputs.empty() && _inputs.front().frameNumber < frameNumber) {
        _inputs.pop_front();
    }

    if (!_inputs.empty()) {
        _oldestFrame = _inputs.front().frameNumber;
    } else {
        _oldestFrame = frameNumber;
    }
}

std::optional<InputBuffer::StoredInput> InputBuffer::getLastInput() const {
    if (_inputs.empty()) {
        return std::nullopt;
    }
    return _inputs.back();
}

void InputBuffer::clear() {
    _inputs.clear();
}

size_t InputBuffer::size() const {
    return _inputs.size();
}
