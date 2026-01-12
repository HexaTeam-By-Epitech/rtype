/*
** EPITECH PROJECT, 2025
** Created on 09/12/2025.
** File description:
** EntityState.hpp
*/

#pragma once

#include <cstdint>
#include <optional>
#include "../Shared/SharedTypes.hpp"
#include "schemas/s2c_messages.capnp.h"

namespace RType::Messages::S2C {

    /**
     * @class EntityState
     * @brief State of a single entity
     * 
     * Represents the server's authoritative state for an entity.
     */
    class EntityState {
       public:
        uint32_t entityId;
        Shared::EntityType type;
        Shared::Vec2 position;
        std::optional<int32_t> health;
        std::string currentAnimation;

        EntityState() : entityId(0), type(Shared::EntityType::Player), currentAnimation("idle") {}

        void toCapnp(::EntityState::Builder builder) const {
            builder.setEntityId(entityId);
            builder.setType(Shared::toCapnpEntityType(type));

            auto posBuilder = builder.initPosition();
            position.toCapnp(posBuilder);

            builder.setHealth(health.value_or(-1));
            builder.setCurrentAnimation(currentAnimation);
        }

        static EntityState fromCapnp(::EntityState::Reader reader) {
            EntityState result;
            result.entityId = reader.getEntityId();
            result.type = Shared::fromCapnpEntityType(reader.getType());
            result.position = Shared::Vec2::fromCapnp(reader.getPosition());

            int32_t healthValue = reader.getHealth();
            if (healthValue >= 0) {
                result.health = healthValue;
            }

            result.currentAnimation = reader.getCurrentAnimation();

            return result;
        }
    };

}  // namespace RType::Messages::S2C
