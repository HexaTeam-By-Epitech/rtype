/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ServerLoop.hpp - Fixed timestep game loop for deterministic gameplay
*/

#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <thread>

#include "../../Game/Logic/IGameLogic.hpp"
#include "../../Game/World/IWorld.hpp"
#include "../Clock/Clock.hpp"
#include "IServerLoop.hpp"

namespace server {

    class EventBus;

    /**
     * @class ServerLoop
     * @brief Deterministic fixed-timestep game loop
     *
     * Ensures all clients see the same game state by running at a fixed 60 Hz
     * with deterministic updates. Implements time accumulator pattern.
     * Implements IServerLoop interface.
     *
     * Features:
     * - Fixed 60 Hz update rate (1/60 = 0.0166667 seconds per frame)
     * - Deterministic state progression
     * - Separate update/render timing
     * - Real-time network synchronization hooks
     * - Thread-safe operation
     * - Frame skipping if lag exceeds threshold
     */
    class ServerLoop : public IServerLoop {
       public:
        /**
         * @brief Constructor
         * @param gameLogic The game logic to run
         * @param eventBus Event bus for inter-system communication
         * @param world Game world wrapping ECS registry
         */
        explicit ServerLoop(std::unique_ptr<IGameLogic> gameLogic, std::shared_ptr<EventBus> eventBus,
                            std::shared_ptr<IWorld> world);

        /**
         * @brief Destructor - ensures clean shutdown
         */
        ~ServerLoop() override;

        /**
         * @brief Initialize the game loop
         * @return true if successful
         */
        bool initialize();

        /**
         * @brief Start the game loop (IServerLoop implementation)
         * Runs in background thread
         */
        void start() override;

        /**
         * @brief Stop the game loop (IServerLoop implementation)
         */
        void stop() override;

        /**
         * @brief Check if game loop is running (IServerLoop implementation)
         * @return true if running
         */
        bool isRunning() const override { return _running; }

        /**
         * @brief Get the current server tick
         * @return uint32_t Current tick number
         */
        uint32_t getCurrentTick() const;

        /**
         * @brief Get reference to game logic
         * @return IGameLogic reference
         */
        IGameLogic &getGameLogic() { return *_gameLogic; }

        /**
         * @brief Get reference to game world
         * @return IWorld pointer (may be null)
         */
        IWorld *getWorld() { return _world.get(); }

       private:
        /**
         * @brief Main game loop function (runs in thread)
         */
        void _gameLoopThread();

        /**
         * @brief Process a single fixed timestep update
         */
        void _fixedUpdate();

        /**
         * @brief Synchronize game state to clients (hooks into network manager)
         */
        void _synchronizeState();

        // Game logic
        std::unique_ptr<IGameLogic> _gameLogic;
        std::shared_ptr<EventBus> _eventBus;
        std::shared_ptr<IWorld> _world;
        Clock _clock;

        // Threading
        std::thread _loopThread;
        std::atomic<bool> _running{false};
        std::atomic<bool> _initialized{false};

        // Timing
        static constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;  // 60 Hz
        static constexpr float MAX_FRAME_ACCUMULATOR = 0.2f;   // Skip frames if lag > 200ms

        double _timeAccumulator{0.0};
        uint32_t _frameCount{0};
        uint32_t _skippedFrames{0};

        // Synchronization
        mutable std::mutex _stateMutex;
    };

}  // namespace server
