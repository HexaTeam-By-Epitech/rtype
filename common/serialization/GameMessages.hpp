/*
** EPITECH PROJECT, 2025
** Created by mael on 02/12/2025.
** File description:
** GameMessages.hpp
*/

#pragma once

#include "ISerializer.hpp"
#include <cstdint>

/**
 * @enum GameMessageType
 * @brief Identifier for high-level gameplay network messages.
 *
 * This enum is deliberately minimal and backend-agnostic. Concrete codecs
 * (e.g. Cap'n Proto) map these logical message types to their own schemas.
 */
enum class GameMessageType : uint16_t {
    SPAWN_ENTITY = 0,
    // Add other gameplay message kinds here (MOVE_ENTITY, DESTROY_ENTITY, ...)
};

/**
 * @class SpawnEntityMessage
 * @brief Example gameplay message used to demonstrate the codec workflow.
 *
 * This class derives from IMessage so it can be passed directly to an
 * ISerializer. It represents a high-level intent (spawn an entity) without
 * exposing any Cap'n Proto details to client/server code.
 */
class SpawnEntityMessage final : public IMessage {
public:
    uint32_t id{}; ///< Unique entity identifier
    float x{}; ///< Spawn position X
    float y{}; ///< Spawn position Y
};