/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** OrbitalSystem - Implementation
*/

#include "OrbitalSystem.hpp"
#include <cmath>
#include "../../Components/IComponent.hpp"
#include "../../Components/PendingDestroy.hpp"
#include "common/Logger/Logger.hpp"

namespace ecs {
    /**
     * @brief Updates all orbital modules for the current frame.
     */
    void OrbitalSystem::update(Registry &registry, float deltaTime) {
        auto entities = registry.getEntitiesWithMask(this->getComponentMask());

        for (auto entityId : entities) {
            auto &orbital = registry.getComponent<OrbitalModule>(entityId);
            auto &transform = registry.getComponent<Transform>(entityId);

            updateOrbitalPosition(registry, entityId, orbital, transform, deltaTime);
        }
    }

    /**
     * @brief Updates a single orbital module's position.
     */
    void OrbitalSystem::updateOrbitalPosition(Registry &registry, Address moduleEntity,
                                              OrbitalModule &orbital, Transform &transform, float deltaTime) {
        uint32_t parentId = orbital.getParentEntityId();

        // Check if parent still exists
        if (!registry.hasComponent<Transform>(parentId)) {
            // Parent destroyed, destroy this module too
            if (!registry.hasComponent<PendingDestroy>(moduleEntity)) {
                registry.setComponent(moduleEntity, PendingDestroy());
                LOG_DEBUG("[OrbitalSystem] Parent entity ", parentId, " destroyed, removing orbital module ",
                          moduleEntity);
            }
            return;
        }

        // Get parent position
        const Transform &parentTransform = registry.getComponent<Transform>(parentId);
        auto parentPos = parentTransform.getPosition();

        // Update orbital angle
        float newAngle = orbital.getCurrentAngle() + (orbital.getOrbitSpeed() * deltaTime);

        // Normalize angle to [0, 2π]
        const float TWO_PI = 2.0f * 3.14159265359f;
        if (newAngle >= TWO_PI) {
            newAngle -= TWO_PI;
        } else if (newAngle < 0.0f) {
            newAngle += TWO_PI;
        }

        orbital.setCurrentAngle(newAngle);

        // Calculate new position using circular motion
        float radius = orbital.getOrbitRadius();
        float newX = parentPos.x + radius * std::cos(newAngle);
        float newY = parentPos.y + radius * std::sin(newAngle);

        transform.setPosition(newX, newY);
    }

    ComponentMask OrbitalSystem::getComponentMask() const {
        return (1ULL << getComponentType<OrbitalModule>()) | (1ULL << getComponentType<Transform>());
    }
}  // namespace ecs
