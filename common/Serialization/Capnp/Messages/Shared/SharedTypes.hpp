/*
** EPITECH PROJECT, 2025
** Created on 09/12/2025.
** File description:
** SharedTypes.hpp 
*/

#pragma once

#include <cstdint>
#include <vector>
#include "schemas/shared_types.capnp.h"

namespace RType::Messages::Shared {

    /**
     * @brief C++ wrapper for Vec2 position
     * 
     * Provides easy conversion between C++ floats and Cap'n Proto messages.
     */
    struct Vec2 {
        float x;
        float y;

        Vec2() : x(0.0f), y(0.0f) {}
        Vec2(float x, float y) : x(x), y(y) {}

        // Convert to Cap'n Proto
        void toCapnp(::Vec2::Builder builder) const {
            builder.setX(x);
            builder.setY(y);
        }

        // Convert from Cap'n Proto
        static Vec2 fromCapnp(::Vec2::Reader reader) { return Vec2(reader.getX(), reader.getY()); }
    };

    /**
     * @brief Player action enum - matches Cap'n Proto enum
     */
    enum class Action { MoveUp = 0, MoveDown = 1, MoveLeft = 2, MoveRight = 3, Shoot = 4 };

    /**
     * @brief Entity type enum - matches Cap'n Proto enum
     */
    enum class EntityType { Player = 0, EnemyType1 = 1, PlayerBullet = 2, EnemyBullet = 3 };

    /**
     * @brief Destroy reason enum - matches Cap'n Proto enum
     */
    enum class DestroyReason { KilledByPlayer = 0, OutOfBounds = 1, Collision = 2 };

    // Conversion helpers for enums
    inline ::Action toCapnpAction(Action action) {
        return static_cast<::Action>(action);
    }

    inline Action fromCapnpAction(::Action action) {
        return static_cast<Action>(action);
    }

    inline ::EntityType toCapnpEntityType(EntityType type) {
        return static_cast<::EntityType>(type);
    }

    inline EntityType fromCapnpEntityType(::EntityType type) {
        return static_cast<EntityType>(type);
    }

    inline ::DestroyReason toCapnpDestroyReason(DestroyReason reason) {
        return static_cast<::DestroyReason>(reason);
    }

    inline DestroyReason fromCapnpDestroyReason(::DestroyReason reason) {
        return static_cast<DestroyReason>(reason);
    }

}  // namespace RType::Messages::Shared
