/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Transform
*/

#ifndef TRANSFORM_HPP_
#define TRANSFORM_HPP_

#include "IComponent.hpp"

namespace ecs {
    class Transform : public IComponent {
        public:
        struct Vector2 {
            int x;
            int y;
        };

        // Constructors
        Transform() : _position{0, 0}, _rotation(0), _scale{1, 1} {}
        Transform(int posX, int posY)
            : _position{posX, posY}, _rotation(0), _scale{1, 1} {}
        Transform(int posX, int posY, int rotation, int scaleX, int scaleY)
            : _position{posX, posY}, _rotation(rotation), _scale{scaleX, scaleY} {}
        ~Transform() override = default;

        // Getters
        Vector2 getPosition() const { return _position; }
        int getRotation() const { return _rotation; }
        Vector2 getScale() const { return _scale; }

        // Setters
        void setPosition(int posX, int posY) {
            _position.x = posX;
            _position.y = posY;
        }
        void setRotation(int rotation) { _rotation = rotation; }
        void setScale(int scaleX, int scaleY) {
            _scale.x = scaleX;
            _scale.y = scaleY;
        }

        // Override getType method
        ComponentType getType() const override {
            return getComponentType<Transform>();
        }

        private:
        Vector2 _position;
        int _rotation;
        Vector2 _scale;

    };
}

#endif
