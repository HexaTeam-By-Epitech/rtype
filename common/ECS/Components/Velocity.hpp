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
    class Velocity : public IComponent {
       public:
        struct Vector2 {
            int x;
            int y;
        };

        // Constructors
        Velocity(int dirX, int dirY, int speed)
            : _direction{dirX, dirY}, _speed(speed) {}
        ~Velocity() override = default;

        // Getters
        Vector2 getDirection() const { return _direction; }
        int getSpeed() const { return _speed; }

        // Setters
        void setDirection(int dirX, int dirY) {
            _direction.x = dirX;
            _direction.y = dirY;
        }
        void setSpeed(int speed) { _speed = speed; }

        // Override getType method
        ComponentType getType() const override {
            return getComponentType<Velocity>();
        }
        
       private:
        Vector2 _direction;
        int _speed;
       
    };
}

#endif
