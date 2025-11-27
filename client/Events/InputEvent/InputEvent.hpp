/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** InputEvent.hpp
*/

#ifndef INPUTEVENT_HPP
#define INPUTEVENT_HPP

#include "../IEvent.hpp"

enum class InputAction {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    SHOOT,
    PAUSE,
    QUIT
};


enum class InputState {
    PRESSED,
    RELEASED,
    HELD
};

class InputEvent : public IEvent {
public:
    InputEvent(InputAction action, InputState state, uint32_t frameNumber = 0)
        : _action(action), _state(state), _frameNumber(frameNumber) {}

    InputAction getAction() const { return _action; }
    InputState getState() const { return _state; }
    uint32_t getFrameNumber() const { return _frameNumber; }

private:
    InputAction _action;
    InputState _state;
    // For network synchro
    uint32_t _frameNumber;
};

#endif
