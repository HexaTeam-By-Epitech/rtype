/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Collider
*/

#ifndef COLLIDER_HPP_
#define COLLIDER_HPP_

#include <cstdint>
#include "IComponent.hpp"

namespace ecs {
    /**
     * @class Collider
     * @brief Component for collision detection and physics interactions.
     * 
     * Defines a bounding box for collision detection with layer-based filtering.
     * Supports both solid collisions and trigger zones.
     */
    class Collider : public IComponent {
       public:
        /**
        * @struct Vector2
        * @brief 2D vector for size and offset.
        */
        struct Vector2 {
            float x;  ///< X component
            float y;  ///< Y component
        };

        /**
         * @brief Constructor with full collision parameters.
         * @param sizeX Width of collision box
         * @param sizeY Height of collision box
         * @param offsetX X offset from entity position
         * @param offsetY Y offset from entity position
         * @param layer Collision layer (for filtering)
         * @param mask Bitmask of layers this collider can interact with
         * @param isTrigger True for trigger zones (no physics), false for solid
         */
        Collider(float sizeX, float sizeY, float offsetX, float offsetY, std::uint32_t layer,
                 std::uint32_t mask, bool isTrigger)
            : _size{sizeX, sizeY},
              _offset{offsetX, offsetY},
              _layer(layer),
              _mask(mask),
              _isTrigger(isTrigger) {}
        ~Collider() override = default;

        /**
         * @brief Get collision box size.
         * @return Vector2 Width and height of the collider.
         */
        Vector2 getSize() const { return _size; }

        /**
         * @brief Get position offset.
         * @return Vector2 Offset from entity position.
         */
        Vector2 getOffset() const { return _offset; }

        /**
         * @brief Get collision layer.
         * @return std::uint32_t Layer identifier.
         */
        std::uint32_t getLayer() const { return _layer; }

        /**
         * @brief Get collision mask.
         * @return std::uint32_t Bitmask of interactable layers.
         */
        std::uint32_t getMask() const { return _mask; }

        /**
         * @brief Check if this is a trigger.
         * @return bool True if trigger zone, false if solid.
         */
        bool isTrigger() const { return _isTrigger; }

        /**
         * @brief Set collision box size.
         * @param sizeX New width
         * @param sizeY New height
         */
        void setSize(float sizeX, float sizeY) {
            _size.x = sizeX;
            _size.y = sizeY;
        }

        /**
         * @brief Set position offset.
         * @param offsetX New X offset
         * @param offsetY New Y offset
         */
        void setOffset(float offsetX, float offsetY) {
            _offset.x = offsetX;
            _offset.y = offsetY;
        }

        /**
         * @brief Set collision layer.
         * @param layer New layer identifier
         */
        void setLayer(std::uint32_t layer) { _layer = layer; }

        /**
         * @brief Set collision mask.
         * @param mask New interaction bitmask
         */
        void setMask(std::uint32_t mask) { _mask = mask; }

        /**
         * @brief Set trigger status.
         * @param isTrigger True for trigger, false for solid
         */
        void setTrigger(bool isTrigger) { _isTrigger = isTrigger; }

        /**
         * @brief Get the component type ID.
         * @return ComponentType Unique ID for Collider component.
         */
        ComponentType getType() const override { return getComponentType<Collider>(); }

       private:
        Vector2 _size;         ///< Collision box dimensions (width, height)
        Vector2 _offset;       ///< Offset from entity position
        std::uint32_t _layer;  ///< Collision layer identifier
        std::uint32_t _mask;   ///< Bitmask of interactable layers
        bool _isTrigger;       ///< True=trigger zone, false=solid collision
    };
}  // namespace ecs

#endif
