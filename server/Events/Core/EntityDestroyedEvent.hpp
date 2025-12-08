/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** EntityDestroyedEvent.hpp
*/

#pragma once

#include "IEvent.hpp"

namespace server {

    /**
     * @class EntityDestroyedEvent
     * @brief Triggered when an entity is destroyed
     */
    class EntityDestroyedEvent : public IEvent {
       public:
        EntityDestroyedEvent() = default;
        ~EntityDestroyedEvent() override = default;

        /**
         * @brief Get the ID of the destroyed entity
         */
        int getEntityId() const;
    };

}  // namespace server
