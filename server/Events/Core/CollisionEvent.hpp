/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** CollisionEvent.hpp
*/

#pragma once

#include "IEvent.hpp"

namespace server {

    /**
     * @class CollisionEvent
     * @brief Represents a collision between two entities
     */
    class CollisionEvent : public IEvent {
       public:
        CollisionEvent() = default;
        ~CollisionEvent() override = default;

        /**
         * @brief Get the first entity ID involved in the collision
         */
        int getEntityA() const;

        /**
         * @brief Get the second entity ID involved in the collision
         */
        int getEntityB() const;
    };

}  // namespace server
