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
#include "common/ECSWrapper/ECSWorld.hpp"
#include "server/Core/Clock/FrameTimer.hpp"
#include "server/Core/ServerLoop/IServerLoop.hpp"
#include "server/Game/Logic/IGameLogic.hpp"

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
         * @param eventBus Event bus for publishing game events
         */
        explicit ServerLoop(std::unique_ptr<IGameLogic> gameLogic, std::shared_ptr<EventBus> eventBus);

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
        bool isRunning() const override { return _loopThread.joinable(); }

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
         * @brief Get reference to ECS world from GameLogic
         * @return ECSWorld shared pointer
         */
        std::shared_ptr<ecs::wrapper::ECSWorld> getECSWorld();

       private:
        /**
         * @brief Main game loop function (runs in thread)
         * @param stopToken Token to check for stop requests
         */
        void _gameLoopThread(std::stop_token stopToken);

        /**
         * @brief Process a single fixed timestep update
         */
        void _fixedUpdate();

        // Game logic
        std::unique_ptr<IGameLogic> _gameLogic;
        std::shared_ptr<EventBus> _eventBus;
        FrameTimer _frameTimer;

        // Threading
        std::jthread _loopThread;
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
