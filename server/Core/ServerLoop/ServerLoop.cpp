/*
** EPITECH PROJECT, 2025
** Created by hugo on 12/12/2025
** File description:
** ServerLoop.cpp - Fixed timestep game loop implementation
*/

#include "server/Core/ServerLoop/ServerLoop.hpp"
#include "common/Logger/Logger.hpp"
#include "server/Game/Logic/GameLogic.hpp"

namespace server {

    ServerLoop::ServerLoop(std::unique_ptr<IGameLogic> gameLogic, std::shared_ptr<EventBus> eventBus)
        : _gameLogic(std::move(gameLogic)), _eventBus(eventBus) {}

    ServerLoop::~ServerLoop() {
        ServerLoop::stop();
    }

    bool ServerLoop::initialize() {
        if (_initialized.exchange(true)) {
            return true;
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

    void ServerLoop::start() {
        if (_loopThread.joinable()) {
            LOG_WARNING("Game loop already running");
            return;
        }

        LOG_INFO("Starting game loop thread...");

        try {
            _timeAccumulator = 0.0;
            _frameCount = 0;
            _skippedFrames = 0;

            _frameTimer.reset();
            _loopThread = std::jthread([this](std::stop_token st) { _gameLoopThread(st); });

            LOG_INFO("✓ Game loop thread started");
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to start game loop: ", e.what());
            throw;
        }
    }

    void ServerLoop::stop() {
        LOG_INFO("Stopping game loop...");
        _loopThread.request_stop();
    }

    uint32_t ServerLoop::getCurrentTick() const {
        return _frameCount;
    }

    std::shared_ptr<ecs::wrapper::ECSWorld> ServerLoop::getECSWorld() {
        if (auto *gameLogic = dynamic_cast<server::GameLogic *>(_gameLogic.get())) {
            return gameLogic->getECSWorld();
        }
        return nullptr;
    }

    void ServerLoop::_gameLoopThread(std::stop_token stopToken) {
        LOG_DEBUG("Game loop thread started");

        while (!stopToken.stop_requested()) {
            try {
                double frameTime = _frameTimer.tick();

                // Cap frame time to prevent spiral of death (lag recovery)
                if (frameTime > 0.1) {
                    LOG_WARNING("Frame time exceeded 100ms (", frameTime * 1000.0, "ms)");
                    frameTime = 0.1;
                }

                _timeAccumulator += frameTime;

                int frameSkips = 0;
                while (_timeAccumulator >= FIXED_TIMESTEP && frameSkips < 5) {
                    _fixedUpdate();
                    _timeAccumulator -= FIXED_TIMESTEP;
                    _frameCount++;
                    frameSkips++;
                }

                if (frameSkips > 1) {
                    _skippedFrames++;
                }

                FrameTimer::sleepMilliseconds(1);

            } catch (const std::exception &e) {
                LOG_ERROR("Thread exception: ", e.what());
                LOG_ERROR("Continuing...");
            }
        }
    }

    void ServerLoop::_fixedUpdate() {
        std::scoped_lock lock(_stateMutex);

        try {
            _gameLogic->update(FIXED_TIMESTEP, _frameCount);
        } catch (const std::exception &e) {
            LOG_ERROR("Game logic update failed: ", e.what());
        }
    }

}  // namespace server
