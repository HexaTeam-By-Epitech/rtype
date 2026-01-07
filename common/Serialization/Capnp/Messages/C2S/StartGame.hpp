/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** StartGame.hpp - Request to start the game in current room
*/

#pragma once

#include <cstdint>
#include <vector>

namespace RType::Messages::C2S {
    /**
     * @brief Request from host to start the game in their room
     * 
     * Only the room host can send this message.
     * Server will validate that sender is the host before starting.
     */
    struct StartGame {
        // No fields needed - player identity comes from session

        std::vector<uint8_t> serialize() const {
            // Empty message - just the message type is enough
            return std::vector<uint8_t>();
        }

        static StartGame deserialize(const std::vector<uint8_t> & /*data*/) { return StartGame(); }
    };
}  // namespace RType::Messages::C2S
