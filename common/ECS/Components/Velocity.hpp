/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Velocity
*/

#pragma once

#include "IComponent.hpp"

namespace ecs {
    /**
     * @class Velocity
     * @brief Component representing movement direction and speed.
     * 
     * This component stores the velocity data for moving entities.
     * Direction should typically be normalized, and speed determines magnitude.
     */
    class Velocity : public IComponent {
       public:
        /**
         * @struct Vector2
         * @brief 2D vector for direction.
         */
        struct Vector2 {
            float x;  ///< X component
            float y;  ///< Y component
        };

        /**
         * @brief Constructor with direction and speed.
         * @param dirX X direction component
         * @param dirY Y direction component
         * @param speed Movement speed in units per second
         */
        Velocity(float dirX, float dirY, float speed) : _direction{dirX, dirY}, _speed(speed) {}
        ~Velocity() override = default;

        /**
         * @brief Get the direction vector.
         * @return Vector2 The movement direction.
         */
        Vector2 getDirection() const { return _direction; }

        /**
         * @brief Get the movement speed.
         * @return float The speed value in units per second.
         */
        float getSpeed() const { return _speed; }

        /**
         * @brief Set the direction vector.
         * @param dirX New X direction
         * @param dirY New Y direction
         */
        void setDirection(float dirX, float dirY) {
            _direction.x = dirX;
            _direction.y = dirY;
        }

        /**
         * @brief Set the movement speed.
         * @param speed New speed value in units per second
         */
        void setSpeed(float speed) { _speed = speed; }

        /**
         * @brief Get the component type ID.
         * @return ComponentType Unique ID for Velocity component.
         */
        ComponentType getType() const override { return getComponentType<Velocity>(); }

       private:
        Vector2 _direction;  ///< Movement direction vector
        float _speed;        ///< Movement speed in units per second
    };
}  // namespace ecs
