/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** GameLoop.cpp
*/

#include "GameLoop.hpp"

GameLoop::GameLoop(EventBus &eventBus, Replicator &replicator)
    : _eventBus(&eventBus), _replicator(&replicator) {}

GameLoop::~GameLoop() {
    shutdown();
}

bool GameLoop::initialize() {
    if (_initialized) {
        return true;
    }

    LOG_INFO("Initializing subsystems...");

    // 1. InputBuffer
    _inputBuffer = std::make_unique<InputBuffer>();
    LOG_INFO("InputBuffer initialized");

    // 2. Rendering
    _rendering = std::make_unique<Rendering>(*_eventBus);
    LOG_INFO("Rendering initialized");

    // 3. Subscribe to network events for entity updates
    _eventBus->subscribe<NetworkEvent>([this](const NetworkEvent &event) { handleNetworkMessage(event); });
    LOG_INFO("Subscribed to NetworkEvent");

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

        // 2. Fixed timestep updates (physics, ECS, input sending)
        while (_accumulator >= _fixedTimestep) {
            // Process and send inputs at fixed rate (60 Hz)
            processInput();

            fixedUpdate(_fixedTimestep);
            _accumulator -= _fixedTimestep;
            _currentFrame++;
        }

        // 3. Variable timestep update (interpolation, etc.)
        update(deltaTime);

        // 4. Render
        render();
    }

    LOG_INFO("Main loop stopped.");
}

void GameLoop::shutdown() {
    if (!_initialized) {
        return;
    }

    LOG_INFO("Shutting down subsystems...");

    _rendering.reset();
    LOG_INFO("Rendering stopped");

    _inputBuffer.reset();
    LOG_INFO("InputBuffer stopped");

    // Note: EventBus and Replicator are owned by Client, don't delete them
    LOG_INFO("GameLoop subsystems stopped");

    _initialized = false;
    _running = false;
    LOG_INFO("Shutdown complete.");
}

void GameLoop::stop() {
    LOG_INFO("Stop requested...");
    _running = false;
}

void GameLoop::update(float deltaTime) {
    // Variable timestep logic
    // - Interpolation between fixed steps
    // - Camera updates
    // - Animations

    // Update entity interpolation for smooth rendering
    // Only update if rendering system is fully initialized
    if (_rendering && _rendering->IsWindowOpen()) {
        _rendering->UpdateInterpolation(deltaTime);
    }

    if (_rendering && _rendering->WindowShouldClose()) {
        shutdown();
    }
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
    if (!_rendering || !_replicator) {
        return;
    }

    // Collect all currently pressed actions
    std::vector<RType::Messages::Shared::Action> actions;

    // ZQSD movement (French keyboard layout)
    if (_rendering->IsKeyDown(KEY_W) || _rendering->IsKeyDown(KEY_Z)) {
        actions.push_back(RType::Messages::Shared::Action::MoveUp);
    }
    if (_rendering->IsKeyDown(KEY_S)) {
        actions.push_back(RType::Messages::Shared::Action::MoveDown);
    }
    if (_rendering->IsKeyDown(KEY_A) || _rendering->IsKeyDown(KEY_Q)) {
        actions.push_back(RType::Messages::Shared::Action::MoveLeft);
    }
    if (_rendering->IsKeyDown(KEY_D)) {
        actions.push_back(RType::Messages::Shared::Action::MoveRight);
    }
    if (_rendering->IsKeyDown(KEY_SPACE)) {
        actions.push_back(RType::Messages::Shared::Action::Shoot);
    }

    RType::Messages::C2S::PlayerInput input;
    input._sequenceId = _inputSequenceId++;
    input.actions = actions;

    // Serialize and wrap in network message
    std::vector<uint8_t> payload = input.serialize();
    std::vector<uint8_t> packet =
        NetworkMessages::createMessage(NetworkMessages::MessageType::C2S_PLAYER_INPUT, payload);

    // Send to server (packet already contains type, so pass empty type)
    _replicator->sendPacket(static_cast<NetworkMessageType>(0), packet);

    // Log occasionally for debugging (only when there are actions)
    if (!actions.empty()) {
        static uint32_t logCounter = 0;
        if (++logCounter % 60 == 0) {
            LOG_DEBUG("Sent input seq=", input._sequenceId, " actions=", actions.size());
        }
    }
}

float GameLoop::calculateDeltaTime() {
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> delta = currentTime - lastTime;
    lastTime = currentTime;

    return delta.count();
}

void GameLoop::handleNetworkMessage(const NetworkEvent &event) {
    auto messageType = NetworkMessages::getMessageType(event.getData());

    // Handle GameStart message (sent once when player connects)
    if (messageType == NetworkMessages::MessageType::S2C_GAME_START) {
        auto payload = NetworkMessages::getPayload(event.getData());
        try {
            auto gameStart = RType::Messages::S2C::GameStart::deserialize(payload);

            LOG_INFO("GameStart received: yourEntityId=", gameStart.yourEntityId);

            // Set our player ID for visual differentiation
            _rendering->SetMyEntityId(gameStart.yourEntityId);

            // Load all initial entities
            for (const auto &entity : gameStart.initialState.entities) {
                _rendering->UpdateEntity(entity.entityId, entity.type, entity.position.x, entity.position.y,
                                         entity.health.value_or(-1));
            }

            LOG_INFO("Loaded ", gameStart.initialState.entities.size(), " entities from GameStart");
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to parse GameStart: ", e.what());
        }
    }

    // Handle GameState message (broadcast continuously by server)
    else if (messageType == NetworkMessages::MessageType::S2C_GAME_STATE) {
        auto payload = NetworkMessages::getPayload(event.getData());
        try {
            auto gameState = RType::Messages::S2C::GameState::deserialize(payload);

            for (const auto &entity : gameState.entities) {
                _rendering->UpdateEntity(entity.entityId, entity.type, entity.position.x, entity.position.y,
                                         entity.health.value_or(-1));
            }

            static uint32_t logCounter = 0;
            if (++logCounter % 60 == 0) {
                LOG_DEBUG("GameState tick=", gameState.serverTick, " entities=", gameState.entities.size());
            }
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to parse GameState: ", e.what());
        }
    }
}
