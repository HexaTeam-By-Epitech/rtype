/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Collider
*/

#ifndef COLLIDER_HPP_
#define COLLIDER_HPP_

#include "IComponent.hpp"
#include <cstdint>

namespace ecs {
    class Collider : public IComponent {
       public:
       struct Vector2 {
            int x;
            int y;
        };
       
        Collider(int sizeX, int sizeY, int offsetX, int offsetY, std::uint32_t layer, std::uint32_t mask, bool isTrigger)
            : _size{sizeX, sizeY}, _offset{offsetX, offsetY}, _layer(layer), _mask(mask), _isTrigger(isTrigger) {}
        ~Collider() override = default;

        // Getters
        Vector2 getSize() const { return _size; }
        Vector2 getOffset() const { return _offset; }
        std::uint32_t getLayer() const { return _layer; }
        std::uint32_t getMask() const { return _mask; }
        bool isTrigger() const { return _isTrigger; }

        // Setters
        void setSize(int sizeX, int sizeY) {
            _size.x = sizeX;
            _size.y = sizeY;
        }
        void setOffset(int offsetX, int offsetY) {
            _offset.x = offsetX;
            _offset.y = offsetY;
        }
        void setLayer(std::uint32_t layer) {
            _layer = layer;
        }
        void setMask(std::uint32_t mask) {
            _mask = mask;
        }
        void setTrigger(bool isTrigger) {
            _isTrigger = isTrigger;
        }

        // Override getType method
        ComponentType getType() const override {
            return getComponentType<Collider>();
        }

       private:
       Vector2 _size;
       Vector2 _offset;
       std::uint32_t _layer;
       std::uint32_t _mask;
       bool _isTrigger;
    };
}

#endif
