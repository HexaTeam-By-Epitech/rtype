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

    std::cout << "[GameLoop] Initializing subsystems..." << std::endl;

    // 1. EventBus
    _eventBus = std::make_unique<EventBus>();
    std::cout << "[GameLoop] ✓ EventBus initialized" << std::endl;

    // 2. InputBuffer
    _inputBuffer = std::make_unique<InputBuffer>();
    std::cout << "[GameLoop] ✓ InputBuffer initialized" << std::endl;

    // 3. Replicator (will start network thread on connect)
    _replicator = std::make_unique<Replicator>(*_eventBus);
    std::cout << "[GameLoop] ✓ Replicator initialized" << std::endl;

    // 4. Rendering
    _rendering = std::make_unique<Rendering>(*_eventBus);
    std::cout << "[GameLoop] ✓ Rendering initialized" << std::endl;

    _initialized = true;
    std::cout << "[GameLoop] All subsystems initialized successfully!" << std::endl;

    return true;
}

void GameLoop::run() {
    if (!_initialized) {
        std::cerr << "[GameLoop] ERROR: Cannot run, not initialized!" << std::endl;
        return;
    }

    std::cout << "[GameLoop] Starting main loop..." << std::endl;
    std::cout << "[GameLoop] Architecture:" << std::endl;
    std::cout << "[GameLoop]   - THREAD 1 (Network): Replicator receiving packets" << std::endl;
    std::cout << "[GameLoop]   - THREAD 2 (Main):    Game logic + Rendering" << std::endl;

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

    std::cout << "[GameLoop] Main loop stopped." << std::endl;
}

void GameLoop::shutdown() {
    if (!_initialized) {
        return;
    }

    std::cout << "[GameLoop] Shutting down subsystems..." << std::endl;

    // Stop in reverse order
    _rendering.reset();
    std::cout << "[GameLoop] ✓ Rendering stopped" << std::endl;

    _replicator.reset();  // Stops network thread automatically
    std::cout << "[GameLoop] ✓ Replicator stopped (network thread terminated)" << std::endl;

    _inputBuffer.reset();
    std::cout << "[GameLoop] ✓ InputBuffer stopped" << std::endl;

    _eventBus.reset();
    std::cout << "[GameLoop] ✓ EventBus stopped" << std::endl;

    _initialized = false;
    std::cout << "[GameLoop] Shutdown complete." << std::endl;
}

void GameLoop::stop() {
    std::cout << "[GameLoop] Stop requested..." << std::endl;
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
    // TODO: Delegate to rendering system
    // _rendering->render();
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
