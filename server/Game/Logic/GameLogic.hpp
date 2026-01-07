/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameLogic.hpp - Server-side game logic implementation
*/

#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include "common/ECSWrapper/ECSWorld.hpp"
#include "server/Game/Logic/IGameLogic.hpp"
#include "server/Game/Rules/GameRules.hpp"
#include "server/Game/StateManager/GameStateManager.hpp"

namespace ecs {
    class ISystem;
}

namespace server {
    class ThreadPool;
    class EventBus;

    /**
     * @class GameLogic
     * @brief Deterministic, authoritative server game logic
     *
     * Coordinates all ECS systems in a strict order:
     * 1. Input processing
     * 2. Movement system
     * 3. Collision detection
     * 4. Health/damage system
     * 5. Spawning system
     * 6. AI system
     * 7. Projectile system
     * 8. Boundary system
     * 9. State serialization
     *
     * Features:
     * - Fixed timestep (60 Hz / 1/60s per frame)
     * - Deterministic updates (same input = same output)
     * - Multi-threaded safe (internal synchronization)
     * - Player entity management
     * - Game state snapshots
     * - Uses World abstraction layer for entity management
     */
    class GameLogic : public IGameLogic {
       public:
        /**
         * @brief Constructor
         * @param world Optional ECSWorld instance (creates one if not provided)
         * @param threadPool Optional ThreadPool for parallel system execution
         * @param eventBus Optional EventBus for publishing game events
         */
        explicit GameLogic(std::shared_ptr<ecs::wrapper::ECSWorld> world = nullptr,
                           std::shared_ptr<ThreadPool> threadPool = nullptr,
                           std::shared_ptr<EventBus> eventBus = nullptr);
        ~GameLogic() override;

        bool initialize() override;
        void update(float deltaTime, uint32_t currentTick) override;
        uint32_t spawnPlayer(uint32_t playerId, const std::string &playerName) override;
        void despawnPlayer(uint32_t playerId) override;
        void processPlayerInput(uint32_t playerId, int inputX, int inputY, bool isShooting) override;

        ecs::Registry &getRegistry() override { return _world->getRegistry(); }
        bool isGameActive() const override { return _gameActive; }
        void resetGame() override;

        /**
         * @brief Get the ECS world instance
         * @return Shared pointer to ECSWorld
         */
        std::shared_ptr<ecs::wrapper::ECSWorld> getECSWorld() { return _world; }

        /**
         * @brief Get the game state manager
         * @return Shared pointer to GameStateManager
         */
        std::shared_ptr<GameStateManager> getStateManager() { return _stateManager; }

        /**
         * @brief Get the game rules
         * @return Reference to game rules
         */
        const GameRules &getGameRules() const override { return _gameRules; }

       private:
        /**
         * @brief Execute all systems in order
         * @param deltaTime Frame delta time
         */
        void _executeSystems(float deltaTime);

        /**
         * @brief Process accumulated player input
         */
        void _processInput();

        /**
         * @brief Clean up dead entities
         */
        void _cleanupDeadEntities();

        /**
         * @brief Check if all players are dead and trigger game over
         */
        void _checkGameOverCondition();

        // ECS World
        std::shared_ptr<ecs::wrapper::ECSWorld> _world;

        // Player management
        std::unordered_map<uint32_t, ecs::Address> _playerMap;  // playerId -> entityAddress

        // Input queue
        struct PlayerInput {
            uint32_t playerId;
            int inputX;
            int inputY;
            bool isShooting;
        };
        std::vector<PlayerInput> _pendingInput;

        // Game state
        std::shared_ptr<GameStateManager> _stateManager;
        std::shared_ptr<ThreadPool> _threadPool;  // Optional: for parallel system execution
        std::shared_ptr<EventBus> _eventBus;      // Optional: for publishing events
        bool _gameActive{false};
        std::atomic<bool> _initialized{false};

        // Thread synchronization
        std::mutex _inputMutex;   // Protects _pendingInput
        std::mutex _playerMutex;  // Protects _playerMap

        // Game rules
        GameRules _gameRules;

        // Constants
        static constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;  // 60 Hz
    };

}  // namespace server
