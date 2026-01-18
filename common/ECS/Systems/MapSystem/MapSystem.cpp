/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** MapSystem.cpp - System managing map scrolling, transitions and state
*/

#include "MapSystem.hpp"
#include "../../Components/Player.hpp"
#include "common/Logger/Logger.hpp"

namespace ecs {

    void MapSystem::update(Registry &registry, float deltaTime) {
        auto mapEntities = registry.getEntitiesWithMask(this->getComponentMask());

        // Early exit if no map is active
        if (mapEntities.empty()) {
            return;
        }

        // Process each map entity (typically there should be only one active map)
        for (auto mapEntityId : mapEntities) {
            auto &mapData = registry.getComponent<MapData>(mapEntityId);

            // Skip if map is already completed
            if (mapData.isCompleted()) {
                continue;
            }

            // Update elapsed time
            mapData.updateElapsedTime(deltaTime);

            // Apply scrolling to all entities (except players)
            float scrollSpeed = mapData.getScrollSpeed();
            if (scrollSpeed > 0.0f) {
                _applyScrolling(registry, scrollSpeed, deltaTime);
            }

            // Check if map duration is reached (0 = infinite)
            float duration = mapData.getDuration();
            if (duration > 0.0f && mapData.getElapsedTime() >= duration) {
                LOG_INFO("Map '", mapData.getMapId(), "' completed after ", mapData.getElapsedTime(),
                         " seconds");
                mapData.markCompleted();

                // TODO: Trigger map transition event here
                // Can be done via EventBus or GameStateManager
                const std::string &nextMap = mapData.getNextMapId();
                if (!nextMap.empty()) {
                    LOG_INFO("Next map: ", nextMap);
                    // Transition logic will be handled by MapLoader/GameLogic
                }
            }
        }
    }

    void MapSystem::_applyScrolling(Registry &registry, float scrollSpeed, float deltaTime) {
        // Get all entities with Transform component
        ComponentMask transformMask = (1ULL << getComponentType<Transform>());
        auto entities = registry.getEntitiesWithMask(transformMask);

        // Calculate scroll offset for this frame
        float scrollOffset = -scrollSpeed * deltaTime;

        for (auto entityId : entities) {
            // Skip player entities - they move independently
            if (registry.hasComponent<Player>(entityId)) {
                continue;
            }

            auto &transform = registry.getComponent<Transform>(entityId);
            auto pos = transform.getPosition();
            transform.setPosition(pos.x + scrollOffset, pos.y);
        }
    }

    ComponentMask MapSystem::getComponentMask() const {
        return (1ULL << getComponentType<MapData>());
    }

}  // namespace ecs
