/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Sprite
*/

#pragma once

#include <string>
#include "IComponent.hpp"

namespace ecs {
    /**
     * @struct Rectangle
     * @brief Rectangle structure defining a region in a texture.
     */
    struct Rectangle {
        int x;       ///< X coordinate of the top-left corner
        int y;       ///< Y coordinate of the top-left corner
        int width;   ///< Width of the rectangle
        int height;  ///< Height of the rectangle
    };

    /**
     * @class Sprite
     * @brief Component representing a visual sprite from a texture.
     * 
     * Stores visual rendering data including texture identifier, source rectangle,
     * scale, rotation, flip states, and rendering layer for Z-ordering.
     */
    class Sprite : public IComponent {
       public:
        /**
         * @brief Constructor with all sprite parameters.
         * @param textureKey Texture identifier (e.g., "r-typesheet1")
         * @param sourceRect Source rectangle in the texture
         * @param scale Scale factor (1.0 = original size)
         * @param rotation Rotation angle in degrees
         * @param flipX Horizontal flip flag
         * @param flipY Vertical flip flag
         * @param layer Rendering layer for Z-ordering
         */
        Sprite(const std::string &textureKey, Rectangle sourceRect, float scale = 1.0f, float rotation = 0.0f,
               bool flipX = false, bool flipY = false, int layer = 0)
            : _textureKey(textureKey),
              _sourceRect(sourceRect),
              _scale(scale),
              _rotation(rotation),
              _flipX(flipX),
              _flipY(flipY),
              _layer(layer) {}

        ~Sprite() override = default;

        /**
         * @brief Get the texture key.
         * @return std::string The texture identifier.
         */
        std::string getTextureKey() const { return _textureKey; }

        /**
         * @brief Get the source rectangle.
         * @return Rectangle The source rectangle in the texture.
         */
        Rectangle getSourceRect() const { return _sourceRect; }

        /**
         * @brief Get the scale factor.
         * @return float The scale value.
         */
        float getScale() const { return _scale; }

        /**
         * @brief Get the rotation angle.
         * @return float The rotation in degrees.
         */
        float getRotation() const { return _rotation; }

        /**
         * @brief Check if horizontally flipped.
         * @return bool True if flipped on X axis.
         */
        bool isFlippedX() const { return _flipX; }

        /**
         * @brief Check if vertically flipped.
         * @return bool True if flipped on Y axis.
         */
        bool isFlippedY() const { return _flipY; }

        /**
         * @brief Get the rendering layer.
         * @return int The Z-ordering layer value.
         */
        int getLayer() const { return _layer; }

        /**
         * @brief Set the texture key.
         * @param textureKey New texture identifier
         */
        void setTextureKey(const std::string &textureKey) { _textureKey = textureKey; }

        /**
         * @brief Set the source rectangle.
         * @param sourceRect New source rectangle
         */
        void setSourceRect(Rectangle sourceRect) { _sourceRect = sourceRect; }

        /**
         * @brief Set the scale factor.
         * @param scale New scale value
         */
        void setScale(float scale) { _scale = scale; }

        /**
         * @brief Set the rotation angle.
         * @param rotation New rotation in degrees
         */
        void setRotation(float rotation) { _rotation = rotation; }

        /**
         * @brief Set horizontal flip state.
         * @param flipX True to flip horizontally
         */
        void setFlipX(bool flipX) { _flipX = flipX; }

        /**
         * @brief Set vertical flip state.
         * @param flipY True to flip vertically
         */
        void setFlipY(bool flipY) { _flipY = flipY; }

        /**
         * @brief Set the rendering layer.
         * @param layer New layer value for Z-ordering
         */
        void setLayer(int layer) { _layer = layer; }

        /**
         * @brief Get the component type ID.
         * @return ComponentType Unique ID for Sprite component.
         */
        ComponentType getType() const override { return getComponentType<Sprite>(); }

       private:
        std::string _textureKey;  ///< Texture identifier (e.g., "r-typesheet1")
        Rectangle _sourceRect;    ///< Source rectangle in the texture
        float _scale;             ///< Scale factor (1.0 = original size)
        float _rotation;          ///< Rotation angle in degrees
        bool _flipX;              ///< Horizontal flip flag
        bool _flipY;              ///< Vertical flip flag
        int _layer;               ///< Rendering layer for Z-ordering
    };
}  // namespace ecs
