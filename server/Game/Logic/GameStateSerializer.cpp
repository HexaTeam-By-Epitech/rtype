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
#include "common/ECSWrapper/ECSWorld.hpp"
#include "common/Logger/Logger.hpp"

namespace server {

    GameStateSnapshot GameStateSerializer::createFullSnapshot(ecs::wrapper::ECSWorld &world,
                                                              uint32_t serverTick) {
        GameStateSnapshot snapshot;
        snapshot.serverTick = serverTick;
        snapshot.activePlayerCount = 0;

        // Get all entities with Transform component (all visible entities)
        auto entities = world.query<ecs::Transform>();

        // Serialize each entity
        for (auto &entity : entities) {
            EntitySnapshot entitySnapshot = serializeEntity(world, entity.getAddress());
            snapshot.entities.push_back(entitySnapshot);

            // Count players
            if (entity.has<ecs::Player>()) {
                snapshot.activePlayerCount++;
            }
        }

        return snapshot;
    }

    GameStateSnapshot GameStateSerializer::createDeltaUpdate(
        ecs::wrapper::ECSWorld &world, uint32_t serverTick,
        [[maybe_unused]] const GameStateSnapshot &lastSnapshot) {
        // Currently returns full snapshot for simplicity and reliability
        // Delta compression would compare with lastSnapshot and only include:
        // - New entities (not in lastSnapshot)
        // - Entities with changed position/health/state
        // - Destroyed entities (in lastSnapshot but not in current world)
        // This optimization can be implemented when network bandwidth becomes a bottleneck
        return createFullSnapshot(world, serverTick);
    }

    EntitySnapshot GameStateSerializer::serializeEntity(ecs::wrapper::ECSWorld &world, uint32_t entityId) {
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
            ecs::wrapper::Entity entity = world.getEntity(entityId);

            // Try to get Transform
            if (entity.has<ecs::Transform>()) {
                const ecs::Transform &transform = entity.get<ecs::Transform>();
                snapshot.posX = transform.getPosition().x;
                snapshot.posY = transform.getPosition().y;
            }

            // Try to get Velocity
            if (entity.has<ecs::Velocity>()) {
                const ecs::Velocity &velocity = entity.get<ecs::Velocity>();
                ecs::Velocity::Vector2 dir = velocity.getDirection();
                float speed = velocity.getSpeed();
                snapshot.velX = dir.x * speed;
                snapshot.velY = dir.y * speed;
            }

            // Try to get Health
            if (entity.has<ecs::Health>()) {
                const ecs::Health &health = entity.get<ecs::Health>();
                snapshot.currentHealth = health.getCurrentHealth();
                snapshot.maxHealth = health.getMaxHealth();
                snapshot.isAlive = (health.getCurrentHealth() > 0);
            }

            // Try to get Player
            if (entity.has<ecs::Player>()) {
                const ecs::Player &player = entity.get<ecs::Player>();
                snapshot.playerId = player.getPlayerId();
            }
        } catch (const std::exception &e) {
            LOG_ERROR("Error serializing entity ", entityId, ": ", e.what());
        }

        return snapshot;
    }

}  // namespace server
