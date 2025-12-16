/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** GameLoop.cpp
*/

#include "GameLoop.hpp"

GameLoop::GameLoop() = default;

GameLoop::~GameLoop() {
    shutdown();
}

bool GameLoop::initialize() {
    if (_initialized) {
        return true;
    }

    LOG_INFO("Initializing subsystems...");

    // 1. EventBus
    _eventBus = std::make_unique<EventBus>();
    LOG_INFO("✓ EventBus initialized");

    // 2. InputBuffer
    _inputBuffer = std::make_unique<InputBuffer>();
    LOG_INFO("✓ InputBuffer initialized");

    // 3. Replicator (will start network thread on connect)
    _replicator = std::make_unique<Replicator>(*_eventBus);
    LOG_INFO("✓ Replicator initialized");

    // 4. Rendering
    _rendering = std::make_unique<Rendering>(*_eventBus);
    LOG_INFO("✓ Rendering initialized");

    _initialized = true;
    LOG_INFO("All subsystems initialized successfully!");

    return true;
}

void GameLoop::run() {
    if (!_initialized) {
        LOG_ERROR("Cannot run, not initialized!");
        return;
    }

    LOG_INFO("Starting main loop...");
    LOG_INFO("Architecture:");
    LOG_INFO("  - THREAD 1 (Network): Replicator receiving packets");
    LOG_INFO("  - THREAD 2 (Main):    Game logic + Rendering");

    _rendering->Initialize(800, 600, "R-Type Client");

    _running = true;

    while (_running) {
        // Calculate delta time
        float deltaTime = calculateDeltaTime();
        _accumulator += deltaTime;

        // 1. Process network messages from network thread
        if (_replicator) {
            _replicator->processMessages();  // ← Reads from network thread queue
        }

        // 2. Process inputs
        processInput();

        // 3. Fixed timestep updates (physics, ECS, etc.)
        while (_accumulator >= _fixedTimestep) {
            fixedUpdate(_fixedTimestep);
            _accumulator -= _fixedTimestep;
            _currentFrame++;
        }

        // 4. Variable timestep update (interpolation, etc.)
        update(deltaTime);

        // 5. Render
        render();
    }

    LOG_INFO("Main loop stopped.");
}

void GameLoop::shutdown() {
    if (!_initialized) {
        return;
    }

    LOG_INFO("Shutting down subsystems...");

    // Stop in reverse order
    _rendering.reset();
    LOG_INFO("✓ Rendering stopped");

    _replicator.reset();  // Stops network thread automatically
    LOG_INFO("✓ Replicator stopped (network thread terminated)");

    _inputBuffer.reset();
    LOG_INFO("✓ InputBuffer stopped");

    _eventBus.reset();
    LOG_INFO("✓ EventBus stopped");

    _initialized = false;
    LOG_INFO("Shutdown complete.");
}

void GameLoop::stop() {
    LOG_INFO("Stop requested...");
    _running = false;
}

void GameLoop::update(float deltaTime) {
    // TODO: Variable timestep logic
    // - Interpolation between fixed steps
    // - Camera updates
    // - Animations
    (void)deltaTime;
}

void GameLoop::fixedUpdate(float fixedDeltaTime) {
    // TODO: Fixed timestep logic (deterministic)
    // - Physics simulation
    // - ECS systems update
    // - Collision detection
    // - Game state prediction
    (void)fixedDeltaTime;
}

void GameLoop::render() {
    if (!_rendering) {
        return;
    }

    _rendering->ClearWindow();
    _rendering->Render();
}

void GameLoop::processInput() {
    // TODO: Read keyboard/mouse
    // TODO: Publish InputEvent on EventBus
}

float GameLoop::calculateDeltaTime() {
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> delta = currentTime - lastTime;
    lastTime = currentTime;

    return delta.count();
}
