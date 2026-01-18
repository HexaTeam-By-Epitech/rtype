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

// Cross-platform unreachable macro
#ifdef _MSC_VER
#define UNREACHABLE() __assume(0)
#else
#define UNREACHABLE() __builtin_unreachable()
#endif

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
    enum class EntityType { Player = 0, EnemyType1 = 1, PlayerBullet = 2, EnemyBullet = 3, Wall = 4 };

    /**
     * @brief Destroy reason enum - matches Cap'n Proto enum
     */
    enum class DestroyReason { KilledByPlayer = 0, OutOfBounds = 1, Collision = 2 };

    // Conversion helpers for enums
    inline ::Action toCapnpAction(Action action) {
        switch (action) {
            case Action::MoveUp:
                return ::Action::MOVE_UP;
            case Action::MoveDown:
                return ::Action::MOVE_DOWN;
            case Action::MoveLeft:
                return ::Action::MOVE_LEFT;
            case Action::MoveRight:
                return ::Action::MOVE_RIGHT;
            case Action::Shoot:
                return ::Action::SHOOT;
        }
        UNREACHABLE();
    }

    inline Action fromCapnpAction(::Action action) {
        switch (action) {
            case ::Action::MOVE_UP:
                return Action::MoveUp;
            case ::Action::MOVE_DOWN:
                return Action::MoveDown;
            case ::Action::MOVE_LEFT:
                return Action::MoveLeft;
            case ::Action::MOVE_RIGHT:
                return Action::MoveRight;
            case ::Action::SHOOT:
                return Action::Shoot;
        }
        UNREACHABLE();
    }

    inline ::EntityType toCapnpEntityType(EntityType type) {
        switch (type) {
            case EntityType::Player:
                return ::EntityType::PLAYER;
            case EntityType::EnemyType1:
                return ::EntityType::ENEMY_TYPE1;
            case EntityType::PlayerBullet:
                return ::EntityType::PLAYER_BULLET;
            case EntityType::EnemyBullet:
                return ::EntityType::ENEMY_BULLET;
            case EntityType::Wall:
                return ::EntityType::WALL;
        }
        UNREACHABLE();
    }

    inline EntityType fromCapnpEntityType(::EntityType type) {
        switch (type) {
            case ::EntityType::PLAYER:
                return EntityType::Player;
            case ::EntityType::ENEMY_TYPE1:
                return EntityType::EnemyType1;
            case ::EntityType::PLAYER_BULLET:
                return EntityType::PlayerBullet;
            case ::EntityType::ENEMY_BULLET:
                return EntityType::EnemyBullet;
            case ::EntityType::WALL:
                return EntityType::Wall;
        }
        UNREACHABLE();
    }

    inline ::DestroyReason toCapnpDestroyReason(DestroyReason reason) {
        switch (reason) {
            case DestroyReason::KilledByPlayer:
                return ::DestroyReason::KILLED_BY_PLAYER;
            case DestroyReason::OutOfBounds:
                return ::DestroyReason::OUT_OF_BOUNDS;
            case DestroyReason::Collision:
                return ::DestroyReason::COLLISION;
        }
        UNREACHABLE();
    }

    inline DestroyReason fromCapnpDestroyReason(::DestroyReason reason) {
        switch (reason) {
            case ::DestroyReason::KILLED_BY_PLAYER:
                return DestroyReason::KilledByPlayer;
            case ::DestroyReason::OUT_OF_BOUNDS:
                return DestroyReason::OutOfBounds;
            case ::DestroyReason::COLLISION:
                return DestroyReason::Collision;
        }
        UNREACHABLE();
    }

    /**
     * @brief Data Transfer Object for player input
     * 
     * Contains the sequence ID and a list of actions for the player.
     */
    struct PlayerInputDTO {
        uint32_t sequenceId;
        std::vector<Action> actions{};

        PlayerInputDTO() : sequenceId(0) {}
        PlayerInputDTO(uint32_t sequenceId, const std::vector<Action> &actions)
            : sequenceId(sequenceId), actions(actions) {}
    };

}  // namespace RType::Messages::Shared
