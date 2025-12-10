/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Transform
*/

#pragma once

#include "IComponent.hpp"

namespace ecs {
    /**
     * @class Transform
     * @brief Component representing position, rotation and scale in 2D space.
     * 
     * This component stores the spatial transformation data for an entity.
     * It includes position (x, y), rotation angle, and scale factors.
     */
    class Transform : public IComponent {
       public:
        /**
         * @struct Vector2
         * @brief 2D vector structure for positions and scales.
         */
        struct Vector2 {
            float x;  ///< X coordinate
            float y;  ///< Y coordinate
        };

        /**
         * @brief Default constructor.
         * Initializes position to (0, 0), rotation to 0, and scale to (1, 1).
         */
        Transform() : _position{0.0f, 0.0f}, _rotation(0.0f), _scale{1.0f, 1.0f} {}

        /**
         * @brief Constructor with position only.
         * @param posX X position
         * @param posY Y position
         */
        Transform(float posX, float posY) : _position{posX, posY}, _rotation(0.0f), _scale{1.0f, 1.0f} {}

        /**
         * @brief Full constructor with all parameters.
         * @param posX X position
         * @param posY Y position
         * @param rotation Rotation angle in degrees
         * @param scaleX X scale factor
         * @param scaleY Y scale factor
         */
        Transform(float posX, float posY, float rotation, float scaleX, float scaleY)
            : _position{posX, posY}, _rotation(rotation), _scale{scaleX, scaleY} {}

        ~Transform() override = default;

        /**
         * @brief Get the position vector.
         * @return Vector2 The current position.
         */
        Vector2 getPosition() const { return _position; }

        /**
         * @brief Get the rotation angle.
         * @return float The rotation in degrees.
         */
        float getRotation() const { return _rotation; }

        /**
         * @brief Get the scale vector.
         * @return Vector2 The current scale factors.
         */
        Vector2 getScale() const { return _scale; }

        /**
         * @brief Set the position.
         * @param posX New X position
         * @param posY New Y position
         */
        void setPosition(float posX, float posY) {
            _position.x = posX;
            _position.y = posY;
        }

        /**
         * @brief Set the rotation angle.
         * @param rotation New rotation in degrees
         */
        void setRotation(float rotation) { _rotation = rotation; }

        /**
         * @brief Set the scale factors.
         * @param scaleX New X scale factor
         * @param scaleY New Y scale factor
         */
        void setScale(float scaleX, float scaleY) {
            _scale.x = scaleX;
            _scale.y = scaleY;
        }

        /**
         * @brief Get the component type ID.
         * @return ComponentType Unique ID for Transform component.
         */
        ComponentType getType() const override { return getComponentType<Transform>(); }

       private:
        Vector2 _position;  ///< Entity position in world space
        float _rotation;    ///< Rotation angle in degrees
        Vector2 _scale;     ///< Scale factors (default: 1, 1)
    };
}  // namespace ecs
