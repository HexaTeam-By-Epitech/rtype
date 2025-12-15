/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** EntityCreatedEvent.hpp
*/

#pragma once

#include "server/Events/IEvent.hpp"

namespace server {

    /**
     * @class EntityCreatedEvent
     * @brief Triggered when a new entity is created
     */
    class EntityCreatedEvent : public IEvent {
       public:
        /**
         * @brief Construct a new EntityCreatedEvent
         * @param entityId The ID of the created entity
         */
        explicit EntityCreatedEvent(int entityId) : _entityId(entityId) {}
        ~EntityCreatedEvent() override = default;

        /**
         * @brief Get the ID of the created entity
         */
        int getEntityId() const { return _entityId; }

       private:
        int _entityId;
    };

}  // namespace server
