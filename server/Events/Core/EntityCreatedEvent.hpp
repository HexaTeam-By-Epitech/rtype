/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** EntityCreatedEvent.hpp
*/

#pragma once

#include "IEvent.hpp"

namespace server {

    /**
     * @class EntityCreatedEvent
     * @brief Triggered when a new entity is created
     */
    class EntityCreatedEvent : public IEvent {
       public:
        EntityCreatedEvent() = default;
        ~EntityCreatedEvent() override = default;

        /**
         * @brief Get the ID of the created entity
         */
        int getEntityId() const;
    };

}  // namespace server
