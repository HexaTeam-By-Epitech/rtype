/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameStateSerializer.cpp - Game state serialization implementation
*/

#include "server/Game/Logic/GameStateSerializer.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/IComponent.hpp"
#include "common/ECS/Components/Player.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Components/Velocity.hpp"
#include "common/ECS/Registry.hpp"
#include "common/Logger/Logger.hpp"

namespace server {

    GameStateSnapshot GameStateSerializer::createFullSnapshot(ecs::Registry &registry, uint32_t serverTick) {
        GameStateSnapshot snapshot;
        snapshot.serverTick = serverTick;
        snapshot.activePlayerCount = 0;

        // Get all entities with Transform component (all visible entities)
        ecs::Signature transformMask = (1ULL << ecs::getComponentType<ecs::Transform>());
        auto entities = registry.getEntitiesWithMask(transformMask);

        // Serialize each entity
        for (auto entityId : entities) {
            EntitySnapshot entitySnapshot = serializeEntity(registry, entityId);
            snapshot.entities.push_back(entitySnapshot);

            // Count players
            if (registry.hasComponent<ecs::Player>(entityId)) {
                snapshot.activePlayerCount++;
            }
        }

        return snapshot;
    }

    GameStateSnapshot GameStateSerializer::createDeltaUpdate(
        ecs::Registry &registry, uint32_t serverTick,
        [[maybe_unused]] const GameStateSnapshot &lastSnapshot) {
        // For now, just return a full snapshot
        // TODO: Optimize by comparing with lastSnapshot and only including changed entities
        return createFullSnapshot(registry, serverTick);
    }

    EntitySnapshot GameStateSerializer::serializeEntity(ecs::Registry &registry, uint32_t entityId) {
        EntitySnapshot snapshot;
        snapshot.entityId = entityId;
        snapshot.posX = 0.0f;
        snapshot.posY = 0.0f;
        snapshot.velX = 0.0f;
        snapshot.velY = 0.0f;
        snapshot.currentHealth = 0;
        snapshot.maxHealth = 0;
        snapshot.playerId = 0;
        snapshot.isAlive = true;

        try {
            // Try to get Transform
            if (registry.hasComponent<ecs::Transform>(entityId)) {
                const auto &transform = registry.getComponent<ecs::Transform>(entityId);
                snapshot.posX = transform.getPosition().x;
                snapshot.posY = transform.getPosition().y;
            }

            // Try to get Velocity
            if (registry.hasComponent<ecs::Velocity>(entityId)) {
                const auto &velocity = registry.getComponent<ecs::Velocity>(entityId);
                auto dir = velocity.getDirection();
                float speed = velocity.getSpeed();
                snapshot.velX = dir.x * speed;
                snapshot.velY = dir.y * speed;
            }

            // Try to get Health
            if (registry.hasComponent<ecs::Health>(entityId)) {
                const auto &health = registry.getComponent<ecs::Health>(entityId);
                snapshot.currentHealth = health.getCurrentHealth();
                snapshot.maxHealth = health.getMaxHealth();
                snapshot.isAlive = (health.getCurrentHealth() > 0);
            }

            // Try to get Player
            if (registry.hasComponent<ecs::Player>(entityId)) {
                const auto &player = registry.getComponent<ecs::Player>(entityId);
                snapshot.playerId = player.getPlayerId();
            }
        } catch (const std::exception &e) {
            LOG_ERROR("Error serializing entity ", entityId, ": ", e.what());
        }

        return snapshot;
    }

}  // namespace server
