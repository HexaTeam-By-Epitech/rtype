/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** CollisionEvent.hpp
*/

#pragma once

#include "../IEvent.hpp"

namespace server {

    /**
     * @class CollisionEvent
     * @brief Represents a collision between two entities
     */
    class CollisionEvent : public IEvent {
       public:
        /**
         * @brief Construct a collision event
         * @param entityA First entity ID
         * @param entityB Second entity ID
         */
        CollisionEvent(int entityA, int entityB) : _entityA(entityA), _entityB(entityB) {}
        ~CollisionEvent() override = default;

        /**
         * @brief Get the first entity ID involved in the collision
         */
        int getEntityA() const { return _entityA; }

        /**
         * @brief Get the second entity ID involved in the collision
         */
        int getEntityB() const { return _entityB; }

       private:
        int _entityA;
        int _entityB;
    };

}  // namespace server
