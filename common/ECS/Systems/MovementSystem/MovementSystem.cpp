/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** MovementSystem
*/

#include "MovementSystem.hpp"

namespace ecs {
    /**
     * @brief Applies velocity to transform positions for all moving entities.
     */
    void MovementSystem::update(Registry &registry, float deltaTime) {
        auto entities = registry.getEntitiesWithMask(this->getComponentMask());

        for (auto entityId : entities) {
            auto &transform = registry.getComponent<Transform>(entityId);
            auto &velocity = registry.getComponent<Velocity>(entityId);

            auto direction = velocity.getDirection();
            float speed = velocity.getSpeed();
            auto pos = transform.getPosition();

            float newX = pos.x + direction.x * speed * deltaTime;
            float newY = pos.y + direction.y * speed * deltaTime;
            transform.setPosition(newX, newY);
        }
    }

    ComponentMask MovementSystem::getComponentMask() const {
        return (1ULL << getComponentType<Transform>()) | (1ULL << getComponentType<Velocity>());
    }
}  // namespace ecs
