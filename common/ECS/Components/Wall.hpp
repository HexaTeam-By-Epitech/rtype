/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Wall.hpp - Component for wall/obstacle entities
*/

#pragma once

#include "IComponent.hpp"

namespace ecs {
    /**
     * @class Wall
     * @brief Component for static or destructible walls/obstacles.
     * 
     * Walls are obstacles that can be destructible or indestructible.
     * They use Transform, Collider, and optionally Health components.
     */
    class Wall : public IComponent {
       public:
        /**
         * @brief Constructor
         * @param destructible Whether the wall can be destroyed
         */
        explicit Wall(bool destructible = false) : _destructible(destructible) {}

        ~Wall() override = default;

        /**
         * @brief Check if the wall is destructible.
         * @return True if destructible
         */
        bool isDestructible() const { return _destructible; }

        /**
         * @brief Set destructible state.
         * @param destructible New state
         */
        void setDestructible(bool destructible) { _destructible = destructible; }

        /**
         * @brief Get the component type ID.
         * @return ComponentType Unique ID for Wall component.
         */
        ComponentType getType() const override { return getComponentType<Wall>(); }

       private:
        bool _destructible;  ///< Can this wall be destroyed?
    };

}  // namespace ecs
