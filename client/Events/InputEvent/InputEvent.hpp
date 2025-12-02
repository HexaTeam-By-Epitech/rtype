/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** InputEvent.hpp
*/

#ifndef INPUTEVENT_HPP
#define INPUTEVENT_HPP

#include "../IEvent.hpp"

/**
 * @enum InputAction
 * @brief Available player input actions
 * 
 * Defines all possible actions a player can perform in R-Type.
 */
enum class InputAction {
    MOVE_UP,     ///< Move player ship upward
    MOVE_DOWN,   ///< Move player ship downward
    MOVE_LEFT,   ///< Move player ship to the left
    MOVE_RIGHT,  ///< Move player ship to the right
    SHOOT,       ///< Fire weapon
    PAUSE,       ///< Pause/unpause the game
    QUIT         ///< Request to quit the game
};

/**
 * @enum InputState
 * @brief State of an input action
 * 
 * Represents the current state of a button or key.
 */
enum class InputState {
    PRESSED,   ///< Input just pressed this frame
    RELEASED,  ///< Input just released this frame
    HELD       ///< Input held down (continuous)
};

/**
 * @class InputEvent
 * @brief Event representing a player input action
 * 
 * InputEvent encapsulates all information about a player input:
 * - What action was performed (move, shoot, etc.)
 * - The state of the input (pressed, released, held)
 * - The frame number for network synchronization
 * 
 * This event is used throughout the system:
 * - Published by input handling code
 * - Subscribed by InputBuffer for prediction
 * - Subscribed by Replicator for network transmission
 * 
 * @note Frame number is critical for client-side prediction and reconciliation
 */
class InputEvent : public IEvent {
   public:
    /**
     * @brief Construct an input event
     * 
     * @param action The player action (MOVE_UP, SHOOT, etc.)
     * @param state The input state (PRESSED, RELEASED, HELD)
     * @param frameNumber Frame number for network sync (default: 0)
     */
    InputEvent(InputAction action, InputState state, uint32_t frameNumber = 0)
        : _action(action), _state(state), _frameNumber(frameNumber) {}

    /**
     * @brief Get the input action
     * @return The action performed
     */
    InputAction getAction() const { return _action; }

    /**
     * @brief Get the input state
     * @return The state of the input
     */
    InputState getState() const { return _state; }

    /**
     * @brief Get the frame number
     * @return Frame number when input occurred
     */
    uint32_t getFrameNumber() const { return _frameNumber; }

   private:
    InputAction _action;    ///< The action performed
    InputState _state;      ///< The state of the input
    uint32_t _frameNumber;  ///< Frame number for network synchronization
};

#endif
