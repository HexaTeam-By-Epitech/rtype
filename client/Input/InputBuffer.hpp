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
#include <optional>
#include <vector>
#include "../Events/InputEvent/InputEvent.hpp"

/**
 * @class InputBuffer
 * @brief Buffer for storing player input history
 * 
 * InputBuffer maintains a history of player inputs with their frame numbers.
 * This history is essential for:
 * - Client-side prediction
 * - Server reconciliation
 * - Input replay after server correction
 * 
 * Client prediction architecture:
 * 1. Player presses a key (frame N)
 * 2. Input stored in buffer
 * 3. Prediction applied immediately (responsive)
 * 4. Input sent to server
 * 5. Server responds with authoritative state (frame N-latency)
 * 6. Client compares and corrects if necessary
 * 7. Replay stored inputs from frame N
 */
class InputBuffer {
   public:
    /**
     * @struct StoredInput
     * @brief Structure representing a stored input with metadata
     */
    struct StoredInput {
        uint32_t frameNumber;  ///< Frame number where input was captured
        InputAction action;    ///< Action performed (MOVE, SHOOT, etc.)
        InputState state;      ///< Action state (PRESSED, RELEASED, HELD)
    };

    /**
     * @brief Default constructor
     */
    InputBuffer() = default;

    /**
     * @brief Default destructor
     */
    ~InputBuffer() = default;

    /**
     * @brief Add an input to the buffer
     * 
     * Stores a new input with its frame number for later replay.
     * Inputs are maintained in chronological order.
     * 
     * @param frameNumber Frame number where input was captured
     * @param action Player action (MOVE_UP, SHOOT, etc.)
     * @param state Input state (PRESSED, RELEASED, HELD)
     * 
     * @note Inputs are automatically sorted by frameNumber
     */
    void addInput(uint32_t frameNumber, InputAction action, InputState state);

    /**
     * @brief Get all inputs since a given frame
     * 
     * Returns all stored inputs from a specific frame onwards.
     * Used to replay inputs after a server correction.
     * 
     * @param startFrame Starting frame number (inclusive)
     * @return Vector of inputs from startFrame to current frame
     * 
     * @note Returned inputs are in chronological order
     * @note If startFrame is older than oldest input, returns from beginning
     */
    std::vector<StoredInput> getInputsSince(uint32_t startFrame) const;

    /**
     * @brief Clear inputs up to a given frame
     * 
     * Removes inputs before a certain frame to save memory.
     * Usually called after receiving a server ACK.
     * 
     * @param frameNumber Frame up to which to clear (exclusive)
     * 
     * @note Inputs from frameNumber onwards are kept
     * @note Allows limiting buffer size in production
     */
    void clearUntil(uint32_t frameNumber);

    /**
     * @brief Get the last stored input
     * 
     * @return Optional containing the last input, or empty if buffer is empty
     * 
     * @note Useful to know the last player state
     */
    std::optional<StoredInput> getLastInput() const;

    /**
     * @brief Clear the buffer completely
     * 
     * Removes all stored inputs. Used during disconnection
     * or game reset.
     */
    void clear();

    /**
     * @brief Get the number of stored inputs
     * 
     * @return Number of inputs in the buffer
     * 
     * @note Useful for debugging and memory monitoring
     */
    size_t size() const;

   private:
    std::deque<StoredInput> _inputs;
    uint32_t _oldestFrame = 0;
};

#endif