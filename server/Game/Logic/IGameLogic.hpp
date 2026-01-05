/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** IGameLogic.hpp - Interface for server-side game logic
*/

#pragma once

#include <cstdint>
#include <memory>

namespace ecs {
    class Registry;
    class ISystem;
}  // namespace ecs

namespace ecs::wrapper {
    class ECSWorld;
}

namespace server {

    /**
     * @interface IGameLogic
     * @brief Interface for server-side game logic orchestration
     *
     * Manages the deterministic game loop, entity lifecycle, and game state.
     * Coordinates all ECS systems in a fixed update order.
     *
     * Update order:
     * 1. Process player input
     * 2. Movement system (update positions)
     * 3. Collision detection
     * 4. Health/damage system
     * 5. Spawning system
     * 6. AI system
     * 7. Projectile system
     * 8. Boundary system (remove off-screen entities)
     * 9. Serialization (state snapshots)
     */
    class IGameLogic {
       public:
        virtual ~IGameLogic() = default;

        /**
         * @brief Initialize game logic and ECS systems
         * @return true if successful
         */
        virtual bool initialize() = 0;

        /**
         * @brief Update game state for one frame
         * @param deltaTime Time since last frame in seconds
         */
        virtual void update(float deltaTime) = 0;

        /**
         * @brief Spawn a player entity
         * @param playerId Unique player ID
         * @param playerName Player's display name
         * @return Entity address or 0 if failed
         */
        virtual uint32_t spawnPlayer(uint32_t playerId, const std::string &playerName) = 0;

        /**
         * @brief Remove a player from the game
         * @param playerId Player ID to remove
         */
        virtual void despawnPlayer(uint32_t playerId) = 0;

        /**
         * @brief Process player input event
         * @param playerId Player ID
         * @param inputX Horizontal input (-1, 0, 1)
         * @param inputY Vertical input (-1, 0, 1)
         * @param isShooting Whether player is shooting
         */
        virtual void processPlayerInput(uint32_t playerId, int inputX, int inputY, bool isShooting) = 0;

        /**
         * @brief Get reference to the ECS registry
         * @return Registry reference
         */
        virtual ecs::Registry &getRegistry() = 0;

        /**
         * @brief Get the current game tick
         * @return uint32_t Current server tick count
         */
        virtual uint32_t getCurrentTick() const = 0;

        /**
         * @brief Check if the game is active
         * @return true if game is running
         */
        virtual bool isGameActive() const = 0;

        /**
         * @brief Reset game state (new game)
         */
        virtual void resetGame() = 0;
    };

}  // namespace server
