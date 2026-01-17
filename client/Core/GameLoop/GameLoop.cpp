/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** GameLoop.cpp
*/

#include "GameLoop.hpp"
#include <cmath>
#include "../ClientGameRules.hpp"
#include "GameruleKeys.hpp"

GameLoop::GameLoop(EventBus &eventBus, Replicator &replicator, const std::string &playerName)
    : _eventBus(&eventBus), _replicator(&replicator), _playerName(playerName) {}

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

    // 4. Subscribe to UI events
    _eventBus->subscribe<UIEvent>([this](const UIEvent &event) { handleUIEvent(event); });
    LOG_INFO("Subscribed to UIEvent");

    _initialized = true;
    LOG_INFO("All subsystems initialized successfully!");

    return true;
}

void GameLoop::handleUIEvent(const UIEvent &event) {
    if (event.getType() == UIEventType::JOIN_GAME) {
        LOG_INFO("[GameLoop] Joining game requested by UI");
        if (_replicator) {
            // Get room ID from event data, fallback to "default" if empty
            std::string roomId = event.getData();
            if (roomId.empty()) {
                roomId = "default";
                LOG_WARNING("[GameLoop] No room ID provided, using default room");
            }

            LOG_INFO("[GameLoop] Joining room: ", roomId);
            _replicator->sendJoinRoom(roomId);

            // Wait for S2C_ROOM_STATE from server with player list
            // No need for mock data anymore
        }
    } else if (event.getType() == UIEventType::CREATE_ROOM) {
        LOG_INFO("[GameLoop] Create room requested by UI");
        if (_replicator) {
            // Parse room data (format: "roomName|maxPlayers|isPrivate")
            const std::string &data = event.getData();
            size_t pos1 = data.find('|');
            size_t pos2 = data.find('|', pos1 + 1);

            if (pos1 != std::string::npos && pos2 != std::string::npos) {
                std::string roomName = data.substr(0, pos1);
                uint32_t maxPlayers = std::stoi(data.substr(pos1 + 1, pos2 - pos1 - 1));
                bool isPrivate = (data.substr(pos2 + 1) == "1");

                LOG_INFO("[GameLoop] Creating room: ", roomName, " (Max: ", maxPlayers,
                         ", Private: ", isPrivate, ")");
                _replicator->sendCreateRoom(roomName, maxPlayers, isPrivate);

                // Mark that we just created a room, so we know we're the host when RoomState comes back
                _justCreatedRoom = true;

                // Request room list after creation (with small delay)
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                // TODO: Request room list
            }
        }
    } else if (event.getType() == UIEventType::REQUEST_ROOM_LIST) {
        LOG_INFO("[GameLoop] Room list requested by UI");
        if (_replicator) {
            _replicator->sendRequestRoomList();
        }
    } else if (event.getType() == UIEventType::START_GAME_REQUEST) {
        LOG_INFO("[GameLoop] Host requesting game start");
        if (_replicator) {
            _replicator->sendStartGame();
        }
    } else if (event.getType() == UIEventType::LEAVE_ROOM) {
        LOG_INFO("[GameLoop] Player leaving room");
        _justCreatedRoom = false;  // Reset host flag when leaving
        if (_replicator) {
            _replicator->sendLeaveRoom();
        }
    } else if (event.getType() == UIEventType::QUIT_GAME) {
        stop();
    }
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

    // Load sprite sheets
    LOG_INFO("Loading sprite sheets...");
    if (!_rendering->LoadTexture("r-typesheet1.gif", "assets/sprites/r-typesheet1.gif")) {
        LOG_WARNING("Failed to load r-typesheet1.gif");
    } else {
        LOG_INFO("✓ Loaded r-typesheet1.gif (player ship)");
    }
    if (!_rendering->LoadTexture("r-typesheet2.gif", "assets/sprites/r-typesheet2.gif")) {
        LOG_WARNING("Failed to load r-typesheet2.gif");
    } else {
        LOG_INFO("✓ Loaded r-typesheet2.gif (enemies)");
    }

    // Apply stored entity ID if GameStart was received before run()
    if (_myEntityId.has_value()) {
        LOG_INFO("Applying stored local player entity ID: ", _myEntityId.value());
        _rendering->SetMyEntityId(_myEntityId.value());
    }

    _running = true;

    while (_running) {
        // Get player ID from replicator (once authenticated)
        if (_myPlayerId == 0 && _replicator) {
            _myPlayerId = _replicator->getMyPlayerId();
        }

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

void GameLoop::setReconciliationThreshold(float threshold) {
    if (_rendering) {
        _rendering->SetReconciliationThreshold(threshold);
        LOG_INFO("Reconciliation threshold set to: ", threshold, " pixels");
    }
}

float GameLoop::getReconciliationThreshold() const {
    if (_rendering) {
        return _rendering->GetReconciliationThreshold();
    }
    return 5.0f;  // Default fallback value
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

        // Update ping display and adaptive reconciliation
        if (_replicator != nullptr) {
            const uint32_t currentPing = _replicator->getLatency();
            _rendering->SetPing(currentPing);

            // ADAPTIVE RECONCILIATION THRESHOLD

            // Base threshold: 5.0px (was 3.0px) - Gives more breathing room for local prediction

            // Penalty: +0.25px per ms of ping (was 0.2px)

            // Max: 30.0px (was 20.0px) - Allow significant drift at high ping (100ms+)

            float adaptiveThreshold = 5.0f + (static_cast<float>(currentPing) * _playerSpeed * 0.0025f);

            if (adaptiveThreshold > 30.0f)
                adaptiveThreshold = 30.0f;

            _rendering->SetReconciliationThreshold(adaptiveThreshold);
        }

        // Update ping display timer (throttles updates to 1 second)
        _rendering->UpdatePingTimer(deltaTime);
    }

    // Note: Don't check WindowShouldClose here - it's checked in render()
    // The window state is managed properly in the rendering system
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

    // Rendering::Render() already clears the window.
    _rendering->Render();

    // Check if shutdown was requested during render (e.g. Quit button)
    if (!_rendering->IsWindowOpen()) {
        stop();
        shutdown();
    }
}

void GameLoop::processInput() {
    if (!_rendering || !_replicator) {
        return;
    }

    // Collect all currently pressed actions
    std::vector<RType::Messages::Shared::Action> actions;

    // Calculate movement delta (distance per frame at fixed 60Hz)
    float moveDelta = _playerSpeed * _fixedTimestep;  // e.g., 200 * 0.0167 = 3.33 pixels

    // Collect input directions first
    int dx = 0, dy = 0;

    // ZQSD movement (French keyboard layout)
    if (_rendering->IsKeyDown(KEY_W) || _rendering->IsKeyDown(KEY_Z)) {
        actions.push_back(RType::Messages::Shared::Action::MoveUp);
        dy = -1;
    }
    if (_rendering->IsKeyDown(KEY_S)) {
        actions.push_back(RType::Messages::Shared::Action::MoveDown);
        dy = 1;
    }
    if (_rendering->IsKeyDown(KEY_A) || _rendering->IsKeyDown(KEY_Q)) {
        actions.push_back(RType::Messages::Shared::Action::MoveLeft);
        dx = -1;
    }
    if (_rendering->IsKeyDown(KEY_D)) {
        actions.push_back(RType::Messages::Shared::Action::MoveRight);
        dx = 1;
    }
    if (_rendering->IsKeyDown(KEY_SPACE)) {
        actions.push_back(RType::Messages::Shared::Action::Shoot);
    }

    // Update movement state based on input
    _isMoving = (dx != 0 || dy != 0);

    // CLIENT-SIDE PREDICTION: Apply movement with diagonal normalization (MUST MATCH SERVER!)
    // Only predict if entity is initialized (prevents moving before spawn)
    if (_myEntityId.has_value() && _entityInitialized && _clientSidePredictionEnabled && _isMoving) {
        float moveX = static_cast<float>(dx);
        float moveY = static_cast<float>(dy);

        // Normalize diagonal movement to prevent going faster diagonally
        if (dx != 0 && dy != 0) {
            float length = std::sqrt(moveX * moveX + moveY * moveY);  // √2 ≈ 1.414
            moveX /= length;                                          // 1/√2 ≈ 0.707
            moveY /= length;                                          // 1/√2 ≈ 0.707
        }

        // Apply normalized movement
        _rendering->MoveEntityLocally(_myEntityId.value(), moveX * moveDelta, moveY * moveDelta);
    }

    // Don't send inputs if in spectator mode (avoid unnecessary network traffic)
    if (_replicator->isSpectator()) {
        return;
    }

    // Create current snapshot
    RType::Messages::C2S::PlayerInput::InputSnapshot currentSnapshot;
    currentSnapshot.sequenceId = _inputSequenceId++;
    currentSnapshot.actions = actions;

    // Add to history
    _inputHistory.push_front(currentSnapshot);
    if (_inputHistory.size() > INPUT_HISTORY_SIZE) {
        _inputHistory.pop_back();
    }

    // Create packet with full history (redundancy)
    // Convert deque to vector for the message constructor
    std::vector<RType::Messages::C2S::PlayerInput::InputSnapshot> historyVector(_inputHistory.begin(),
                                                                                _inputHistory.end());

    RType::Messages::C2S::PlayerInput inputPacket(historyVector);

    // Serialize and wrap in network message
    std::vector<uint8_t> payload = inputPacket.serialize();
    std::vector<uint8_t> packet =
        NetworkMessages::createMessage(NetworkMessages::MessageType::C2S_PLAYER_INPUT, payload);

    // Send to server (packet already contains type, so pass empty type)
    _replicator->sendPacket(static_cast<NetworkMessageType>(0), packet);

    // Log ALL inputs for debugging (not just non-empty)
    static uint32_t logCounter = 0;
    if (++logCounter % 60 == 0) {
        LOG_DEBUG("[INPUT] Sent seq=", currentSnapshot.sequenceId, " actions=", actions.size(), " (",
                  (dx != 0 || dy != 0 ? "MOVING" : "STOPPED"), ") + history=", _inputHistory.size());
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
    auto payload = NetworkMessages::getPayload(event.getData());

    switch (messageType) {
        case NetworkMessages::MessageType::S2C_GAME_START:
            handleGameStart(payload);
            break;
        case NetworkMessages::MessageType::S2C_GAME_STATE:
            handleGameState(payload);
            break;
        case NetworkMessages::MessageType::S2C_GAMERULE_UPDATE:
            handleGameruleUpdate(payload);
            break;
        case NetworkMessages::MessageType::S2C_ROOM_LIST:
            handleRoomList(payload);
            break;
        case NetworkMessages::MessageType::S2C_ROOM_STATE:
            handleRoomState(payload);
            break;
        case NetworkMessages::MessageType::S2C_ENTITY_DESTROYED:
            handleEntityDestroyed(payload);
            break;
        default:
            break;
    }
}

void GameLoop::handleGameStart(const std::vector<uint8_t> &payload) {
    LOG_INFO("GameStart message received");

    // Hide waiting room and start game
    if (_rendering) {
        _rendering->StartGame();
    }

    try {
        auto gameStart = RType::Messages::S2C::GameStart::deserialize(payload);
        LOG_INFO("GameStart received: yourEntityId=", gameStart.yourEntityId);

        for (const auto &entity : gameStart.initialState.entities) {
            if (entity.entityId == gameStart.yourEntityId) {
                _myEntityId = entity.entityId;
                _entityInitialized = true;
                LOG_INFO("✓ Stored local player entity ID: ", entity.entityId);

                // Set the entity ID in the rendering system
                if (_rendering) {
                    _rendering->SetMyEntityId(entity.entityId);
                    LOG_INFO("✓ SetMyEntityId called with ID: ", entity.entityId);
                }
            }

            if (_rendering) {
                _rendering->UpdateEntity(entity.entityId, entity.type, entity.position.x, entity.position.y,
                                         entity.health.value_or(-1), entity.currentAnimation, entity.spriteX,
                                         entity.spriteY, entity.spriteW, entity.spriteH);
            }
        }
        LOG_INFO("Loaded ", gameStart.initialState.entities.size(), " entities from GameStart");
    } catch (const std::exception &e) {
        LOG_ERROR("Failed to parse GamerulePacket: ", e.what());
    }
}

void GameLoop::handleRoomList(const std::vector<uint8_t> &payload) {
    try {
        auto roomList = RType::Messages::S2C::RoomList::deserialize(payload);

        LOG_INFO("✓ RoomList received with ", roomList.rooms.size(), " rooms");

        // Convert to RoomData format for Rendering
        std::vector<RoomData> rooms;
        for (const auto &room : roomList.rooms) {
            RoomData roomData;
            roomData.roomId = room.roomId;
            roomData.roomName = room.roomName;
            roomData.playerCount = room.playerCount;
            roomData.maxPlayers = room.maxPlayers;
            roomData.isPrivate = room.isPrivate;
            roomData.state = room.state;
            rooms.push_back(roomData);

            LOG_INFO("  - Room: ", room.roomName, " [", room.playerCount, "/", room.maxPlayers, "]");
        }

        // Update rendering with room list
        if (_rendering) {
            _rendering->UpdateRoomList(rooms);
        }

    } catch (const std::exception &e) {
        LOG_ERROR("Failed to parse RoomList: ", e.what());
    }
}

void GameLoop::handleRoomState(const std::vector<uint8_t> &payload) {
    try {
        auto roomState = RType::Messages::S2C::RoomState::deserialize(payload);

        LOG_INFO("✓ RoomState received: ", roomState.roomName, " with ", roomState.players.size(),
                 " players");

        // Convert to PlayerInfo format for Rendering
        std::vector<Game::PlayerInfo> players;
        bool isHost = false;
        bool isSpectator = false;  // Check if we are a spectator

        // Determine if we are the host by checking if our playerId matches a player with isHost=true
        for (const auto &playerData : roomState.players) {
            Game::PlayerInfo playerInfo(playerData.playerId, playerData.playerName, playerData.isHost,
                                        playerData.isSpectator);
            players.push_back(playerInfo);

            LOG_INFO("  - Player: '", playerData.playerName, "' (ID:", playerData.playerId,
                     ") | isHost=", playerData.isHost, " | isSpectator=", playerData.isSpectator);

            // Check if this is us
            if (playerData.playerId == _myPlayerId) {
                isHost = playerData.isHost;
                isSpectator = playerData.isSpectator;

                if (playerData.isHost) {
                    LOG_INFO("    -> MATCH! This is ME and I'm the HOST");
                } else if (playerData.isSpectator) {
                    LOG_INFO("    -> MATCH! This is ME and I'm a SPECTATOR");
                } else {
                    LOG_INFO("    -> This is ME (regular player)");
                }
            }
        }

        LOG_INFO("  Final isHost value: ", isHost, ", isSpectator: ", isSpectator);

        // Update waiting room with player list
        if (_rendering) {
            _rendering->UpdateWaitingRoom(players, roomState.roomName, isHost, isSpectator);
        }

    } catch (const std::exception &e) {
        LOG_ERROR("Failed to parse RoomState: ", e.what());
    }
}

void GameLoop::handleEntityDestroyed(const std::vector<uint8_t> &payload) {
    try {
        auto entityDestroyed = RType::Messages::S2C::EntityDestroyed::deserialize(payload);

        LOG_INFO("✓ EntityDestroyed received: entityId=", entityDestroyed.entityId,
                 " reason=", static_cast<int>(entityDestroyed.reason));

        // Remove the entity from rendering immediately
        if (_rendering) {
            _rendering->RemoveEntity(entityDestroyed.entityId);
        }

        // If this was our entity, handle game over scenario
        if (_myEntityId.has_value() && entityDestroyed.entityId == _myEntityId.value()) {
            LOG_WARNING("Our entity was destroyed!");
            _myEntityId = std::nullopt;
            _entityInitialized = false;
        }

    } catch (const std::exception &e) {
        LOG_ERROR("Failed to parse EntityDestroyed: ", e.what());
    }
}

void GameLoop::handleGameState(const std::vector<uint8_t> &payload) {
    try {
        auto gameState = RType::Messages::S2C::GameState::deserialize(payload);

        for (const auto &entity : gameState.entities) {
            if (entity.entityId == _myEntityId.value_or(0) && _clientSidePredictionEnabled) {
                processServerReconciliation(entity);
            } else {
                _rendering->UpdateEntity(entity.entityId, entity.type, entity.position.x, entity.position.y,
                                         entity.health.value_or(-1), entity.currentAnimation, entity.spriteX,
                                         entity.spriteY, entity.spriteW, entity.spriteH);
            }
        }

        static uint32_t logCounter = 0;
        if (++logCounter % 60 == 0) {
            LOG_DEBUG("GameState tick=", gameState.serverTick, " entities=", gameState.entities.size());
        }
    } catch (const std::exception &e) {
        LOG_ERROR("Failed to parse GameState: ", e.what());
    }
}

void GameLoop::processServerReconciliation(const RType::Messages::S2C::EntityState &entity) {
    // 1. Prune history: Remove inputs already processed by server
    while (!_inputHistory.empty() && _inputHistory.back().sequenceId <= entity.lastProcessedInput) {
        _inputHistory.pop_back();
    }

    // 2. Re-simulate: Start from Server Position
    float predictedX = entity.position.x;
    float predictedY = entity.position.y;

    simulateInputHistory(predictedX, predictedY);

    if (_rendering) {
        _rendering->UpdateEntity(entity.entityId, entity.type, predictedX, predictedY,
                                 entity.health.value_or(-1), entity.currentAnimation, entity.spriteX,
                                 entity.spriteY, entity.spriteW, entity.spriteH);
    }
}

void GameLoop::simulateInputHistory(float &x, float &y) {
    for (auto it = _inputHistory.rbegin(); it != _inputHistory.rend(); ++it) {
        const auto &snapshot = *it;
        int dx = 0, dy = 0;

        for (auto act : snapshot.actions) {
            if (act == RType::Messages::Shared::Action::MoveUp)
                dy = -1;
            else if (act == RType::Messages::Shared::Action::MoveDown)
                dy = 1;
            else if (act == RType::Messages::Shared::Action::MoveLeft)
                dx = -1;
            else if (act == RType::Messages::Shared::Action::MoveRight)
                dx = 1;
        }

        if (dx != 0 || dy != 0) {
            float moveX = static_cast<float>(dx);
            float moveY = static_cast<float>(dy);

            // Normalize diagonal
            if (dx != 0 && dy != 0) {
                float length = std::sqrt(moveX * moveX + moveY * moveY);
                moveX /= length;
                moveY /= length;
            }

            float frameDelta = _playerSpeed * (1.0f / 60.0f);
            x += moveX * frameDelta;
            y += moveY * frameDelta;
        }
    }
}

void GameLoop::handleGameruleUpdate(const std::vector<uint8_t> &payload) {
    try {
        auto gamerulePacket = RType::Messages::S2C::GamerulePacket::deserialize(payload);
        auto &clientRules = client::ClientGameRules::getInstance();
        clientRules.updateMultiple(gamerulePacket.getGamerules());

        LOG_INFO("✓ Gamerule update received: ", gamerulePacket.size(), " rules updated");

        if (gamerulePacket.hasGamerule(GameruleKeys::toString(GameruleKey::PLAYER_SPEED))) {
            _playerSpeed = gamerulePacket.getGamerule(GameruleKeys::toString(GameruleKey::PLAYER_SPEED));
            LOG_INFO("  - Player speed updated to: ", _playerSpeed);
        }
    } catch (const std::exception &e) {
        LOG_ERROR("Failed to parse GamerulePacket: ", e.what());
    }
}
