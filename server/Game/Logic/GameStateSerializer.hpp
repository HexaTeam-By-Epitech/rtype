/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameStateSerializer.hpp - Game state serialization for network transmission
*/

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace ecs::wrapper {
    class ECSWorld;
}

namespace server {

    /**
     * @struct EntitySnapshot
     * @brief Serialized entity state for network transmission
     */
    struct EntitySnapshot {
        uint32_t entityId;
        float posX;
        float posY;
        float velX;
        float velY;
        int currentHealth;
        int maxHealth;
        uint32_t playerId;  // If player entity
        bool isAlive;
    };

    /**
     * @struct GameStateSnapshot
     * @brief Complete game state at a given tick
     */
    struct GameStateSnapshot {
        uint32_t serverTick;
        std::vector<EntitySnapshot> entities;
        uint32_t activePlayerCount;
    };

    /**
     * @class GameStateSerializer
     * @brief Serializes and deserializes game state for network transmission
     *
     * Responsibilities:
     * - Create full game state snapshots
     * - Generate delta updates (only changed entities)
     * - Serialize entity components to network format
     * - Ensure deterministic entity ordering
     *
     * Network Efficiency:
     * - Full snapshots sent on connect or major state changes
     * - Delta updates sent every frame (only changed entities)
     * - Position/health deltas with compression
     */
    class GameStateSerializer {
       public:
        /**
         * @brief Create a full game state snapshot
         * @param world ECS world wrapper
         * @param serverTick Current game tick
         * @return Complete game state snapshot
         */
        static GameStateSnapshot createFullSnapshot(ecs::wrapper::ECSWorld &world, uint32_t serverTick);

        /**
         * @brief Create a delta update (changed entities only)
         * @param world ECS world wrapper
         * @param serverTick Current game tick
         * @param lastSnapshot Previous snapshot for comparison
         * @return Delta snapshot with only changed entities
         */
        static GameStateSnapshot createDeltaUpdate(ecs::wrapper::ECSWorld &world, uint32_t serverTick,
                                                   const GameStateSnapshot &lastSnapshot);

        /**
         * @brief Serialize entity to network format
         * @param world ECS world wrapper
         * @param entityId Entity address
         * @return Serialized entity snapshot
         */
        static EntitySnapshot serializeEntity(ecs::wrapper::ECSWorld &world, uint32_t entityId);
    };

}  // namespace server
