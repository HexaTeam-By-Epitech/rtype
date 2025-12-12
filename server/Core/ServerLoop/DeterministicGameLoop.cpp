/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** DeterministicGameLoop.cpp - Fixed timestep game loop implementation
*/

#include "DeterministicGameLoop.hpp"
#include <chrono>
#include "../../../common/Logger/Logger.hpp"

namespace server {

    DeterministicGameLoop::DeterministicGameLoop(std::unique_ptr<IGameLogic> gameLogic,
                                                 std::shared_ptr<EventBus> eventBus)
        : _gameLogic(std::move(gameLogic)), _eventBus(eventBus) {}

    DeterministicGameLoop::~DeterministicGameLoop() {
        stop();
        if (_loopThread.joinable()) {
            _loopThread.join();
        }
    }

    bool DeterministicGameLoop::initialize() {
        if (_initialized.exchange(true)) {
            return true;  // Already initialized
        }

        LOG_INFO("Initializing deterministic game loop...");

        try {
            if (!_gameLogic->initialize()) {
                LOG_ERROR("Failed to initialize game logic");
                _initialized.store(false);
                return false;
            }

            LOG_INFO("✓ Game logic initialized");
            LOG_INFO("✓ Fixed timestep: ", FIXED_TIMESTEP, "s (60 Hz)");

            return true;
        } catch (const std::exception &e) {
            LOG_ERROR("Initialization failed: ", e.what());
            _initialized.store(false);
            return false;
        }
    }

    bool DeterministicGameLoop::start() {
        if (_running.exchange(true)) {
            return true;  // Already running
        }

        LOG_INFO("Starting game loop thread...");

        try {
            _timeAccumulator = 0.0;
            _frameCount = 0;
            _skippedFrames = 0;

            _clock.getFrameTimer().reset();
            _loopThread = std::thread([this] { _gameLoopThread(); });

            LOG_INFO("✓ Game loop thread started");
            return true;
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to start game loop: ", e.what());
            _running.store(false);
            return false;
        }
    }

    void DeterministicGameLoop::stop() {
        LOG_INFO("Stopping game loop...");
        _running.store(false);
    }

    uint32_t DeterministicGameLoop::getCurrentTick() const {
        return _gameLogic->getCurrentTick();
    }

    void DeterministicGameLoop::_gameLoopThread() {
        LOG_DEBUG("Game loop thread started (TID: ", std::this_thread::get_id(), ")");

        while (_running) {
            try {
                // Measure frame time
                double frameTime = _clock.getFrameTimer().elapsed();
                _clock.getFrameTimer().reset();

                // Cap frame time to prevent spiral of death (lag recovery)
                if (frameTime > 0.1) {
                    LOG_WARNING("Frame time exceeded 100ms (", frameTime * 1000.0, "ms)");
                    frameTime = 0.1;  // Cap at 100ms
                }

                // Accumulate time
                _timeAccumulator += frameTime;

                // Process fixed updates
                int frameSkips = 0;
                while (_timeAccumulator >= FIXED_TIMESTEP && frameSkips < 5) {
                    _fixedUpdate();
                    _timeAccumulator -= FIXED_TIMESTEP;
                    _frameCount++;
                    frameSkips++;
                }

                // Track frame skips
                if (frameSkips > 1) {
                    _skippedFrames++;
                }

                // Synchronize state to network
                _synchronizeState();

                // Yield to prevent busy-loop
                std::this_thread::sleep_for(std::chrono::milliseconds(1));

            } catch (const std::exception &e) {
                LOG_ERROR("Thread exception: ", e.what());
                LOG_ERROR("Continuing...");
            }
        }

        LOG_DEBUG("Thread exiting. Frames: ", _frameCount, ", Skipped: ", _skippedFrames);
    }

    void DeterministicGameLoop::_fixedUpdate() {
        std::lock_guard<std::mutex> lock(_stateMutex);

        try {
            _gameLogic->update(FIXED_TIMESTEP);
        } catch (const std::exception &e) {
            LOG_ERROR("Game logic update failed: ", e.what());
        }
    }

    void DeterministicGameLoop::_synchronizeState() {
        std::lock_guard<std::mutex> lock(_stateMutex);

        try {
            // TODO: Create game state snapshot and queue for network broadcast
            // This is called after fixed updates to send state to clients
            // - Generate delta from previous tick (only changed entities)
            // - Serialize entity states (position, health, animation frame, etc.)
            // - Queue packets for network manager to send
        } catch (const std::exception &e) {
            LOG_ERROR("State synchronization failed: ", e.what());
        }
    }

}  // namespace server
