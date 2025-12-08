/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Velocity
*/

#ifndef VELOCITY_HPP_
#define VELOCITY_HPP_

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
            int x;  ///< X component
            int y;  ///< Y component
        };

        /**
         * @brief Constructor with direction and speed.
         * @param dirX X direction component
         * @param dirY Y direction component
         * @param speed Movement speed
         */
        Velocity(int dirX, int dirY, int speed) : _direction{dirX, dirY}, _speed(speed) {}
        ~Velocity() override = default;

        /**
         * @brief Get the direction vector.
         * @return Vector2 The movement direction.
         */
        Vector2 getDirection() const { return _direction; }

        /**
         * @brief Get the movement speed.
         * @return int The speed value.
         */
        int getSpeed() const { return _speed; }

        /**
         * @brief Set the direction vector.
         * @param dirX New X direction
         * @param dirY New Y direction
         */
        void setDirection(int dirX, int dirY) {
            _direction.x = dirX;
            _direction.y = dirY;
        }

        /**
         * @brief Set the movement speed.
         * @param speed New speed value
         */
        void setSpeed(int speed) { _speed = speed; }

        /**
         * @brief Get the component type ID.
         * @return ComponentType Unique ID for Velocity component.
         */
        ComponentType getType() const override { return getComponentType<Velocity>(); }

       private:
        Vector2 _direction;  ///< Movement direction vector
        int _speed;          ///< Movement speed
    };
}  // namespace ecs

#endif
