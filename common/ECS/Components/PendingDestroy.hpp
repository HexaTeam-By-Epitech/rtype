/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** PendingDestroy - Marker component for entities to be destroyed
*/

#pragma once

#include "IComponent.hpp"

namespace ecs {
    /**
     * @enum DestroyReason
     * @brief Reason why an entity is being destroyed
     */
    enum class DestroyReason {
        OutOfBounds = 0,  ///< Entity went outside screen boundaries
        Killed = 1,       ///< Entity was killed (health <= 0)
        Expired = 2,      ///< Entity lifetime expired (e.g., projectile)
        Manual = 3        ///< Manually destroyed (script, etc.)
    };

    /**
     * @class PendingDestroy
     * @brief Marker component indicating entity should be destroyed.
     * 
     * When this component is added to an entity, the server will:
     * 1. Send an EntityDestroyed message to all clients
     * 2. Remove the entity from the registry
     * 
     * This allows proper client-side cleanup and prevents interpolation bugs.
     */
    class PendingDestroy : public IComponent {
       public:
        /**
         * @brief Default constructor with OutOfBounds reason.
         */
        PendingDestroy() : _reason(DestroyReason::OutOfBounds) {}

        /**
         * @brief Constructor with specific reason.
         * @param reason Why the entity is being destroyed
         */
        explicit PendingDestroy(DestroyReason reason) : _reason(reason) {}

        ~PendingDestroy() override = default;

        /**
         * @brief Get the destruction reason.
         * @return DestroyReason The reason for destruction
         */
        DestroyReason getReason() const { return _reason; }

        /**
         * @brief Get component type ID.
         * @return ComponentType The unique ID of this component type
         */
        ComponentType getType() const override { return getComponentType<PendingDestroy>(); }

       private:
        DestroyReason _reason;
    };
}  // namespace ecs
