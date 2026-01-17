/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** BoundarySystem
*/

#include "BoundarySystem.hpp"
#include <vector>
#include "../../Components/IComponent.hpp"
#include "../../Components/PendingDestroy.hpp"

namespace ecs {
    BoundarySystem::BoundarySystem(int screenWidth, int screenHeight)
        : _screenWidth(screenWidth), _screenHeight(screenHeight) {}

    /**
     * @brief Checks entities against boundaries and marks out-of-bounds entities for destruction.
     * 
     * Instead of destroying entities directly, this system adds a PendingDestroy component.
     * The server will then:
     * 1. Send EntityDestroyed messages to clients
     * 2. Actually destroy the entities
     * 
     * This prevents interpolation bugs where clients try to interpolate to old positions.
     */
    void BoundarySystem::update(Registry &registry, [[maybe_unused]] float deltaTime) {
        auto entities = registry.getEntitiesWithMask(this->getComponentMask());
        std::vector<std::uint32_t> toMarkForDestruction;

        for (auto entityId : entities) {
            // Skip entities already marked for destruction
            if (registry.hasComponent<PendingDestroy>(entityId)) {
                continue;
            }

            auto &transform = registry.getComponent<Transform>(entityId);
            auto pos = transform.getPosition();

            if (pos.x < -100 || pos.x > _screenWidth + 100 || pos.y < -100 || pos.y > _screenHeight + 100) {
                toMarkForDestruction.push_back(entityId);
            }
        }

        // Mark entities for destruction instead of destroying directly
        for (auto entityId : toMarkForDestruction) {
            registry.setComponent<PendingDestroy>(entityId, PendingDestroy(DestroyReason::OutOfBounds));
        }
    }

    void BoundarySystem::setScreenSize(int width, int height) {
        _screenWidth = width;
        _screenHeight = height;
    }

    ComponentMask BoundarySystem::getComponentMask() const {
        return (1ULL << getComponentType<Transform>());
    }
}  // namespace ecs
