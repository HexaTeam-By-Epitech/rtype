/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** GameLoop.hpp
*/

#ifndef GAMELOOP_HPP
#define GAMELOOP_HPP

#include <raylib.h>
#include <chrono>
#include <deque>
#include <iostream>
#include <memory>
#include <optional>
#include "../common/Logger/Logger.hpp"
#include "Capnp/Messages/Messages.hpp"
#include "Capnp/NetworkMessages.hpp"
#include "Core/EventBus/EventBus.hpp"
#include "Events/NetworkEvent/NetworkEvent.hpp"
#include "Events/UIEvent.hpp"
#include "Input/InputBuffer.hpp"
#include "Network/Replicator.hpp"
#include "Rendering/Rendering.hpp"

enum class GameScene { LOBBY, IN_GAME, PAUSED, GAME_OVER };

/**
 * @class GameLoop
 * @brief Main game loop orchestrating all subsystems
 * 
 * GameLoop is the conductor of the R-Type client.
 * It coordinates and synchronizes all components:
 * - EventBus: Inter-component communication
 * - InputBuffer: Player input management
 * - Replicator: Network communication
 * - Rendering: Graphical display
 * - ECS (future): Game logic and prediction
 * 
 * Temporal architecture:
 * ```
 * Frame N:
 *   1. ProcessInput()      - Capture keyboard/mouse
 *   2. FixedUpdate() x N   - Physics simulation (60 FPS fixed)
 *   3. Update(deltaTime)   - Variable game logic
 *   4. Render()            - Display (VSync)
 * ```
 * 
 * Fixed timestep:
 * - Physics runs at constant 60 FPS (16.67ms per frame)
 * - Decoupled from display framerate
 * - Guarantees deterministic behavior
 * - Essential for client prediction and network synchronization
 */
class GameLoop {
   public:
    /**
     * @brief Constructor with shared EventBus and Replicator
     * 
     * Uses the provided EventBus and Replicator from Client.
     * Call initialize() to start other subsystems.
     * 
     * @param eventBus Shared EventBus instance
     * @param replicator Shared Replicator instance
     */
    GameLoop(EventBus &eventBus, Replicator &replicator);

    /**
     * @brief Destructor
     * 
     * Properly stops all subsystems and frees resources.
     */
    ~GameLoop();

    /**
     * @brief Initialize all game subsystems
     * 
     * Initializes in order:
     * 1. EventBus
     * 2. InputBuffer
     * 3. Replicator
     * 4. Rendering (window, textures)
     * 5. ECS (future)
     * 
     * @return true if all systems initialize correctly
     * @return false if a system fails (error logged)
     * 
     * @note On failure, automatically calls shutdown()
     * @note Must be called before run()
     */
    bool initialize();

    /**
     * @brief Start the main game loop
     * 
     * Launches the game loop that runs until:
     * - Window closed by user
     * - stop() called
     * - Critical error
     * 
     * Loop structure:
     * @code
     * while (running) {
     *     float deltaTime = calculateDeltaTime();
     *     accumulator += deltaTime;
     *     
     *     processInput();
     *     
     *     // Fixed timestep for physics
     *     while (accumulator >= fixedTimestep) {
     *         fixedUpdate(fixedTimestep);
     *         accumulator -= fixedTimestep;
     *         currentFrame++;
     *     }
     *     
     *     update(deltaTime);
     *     render();
     * }
     * @endcode
     * 
     * @note Blocking until game stops
     * @note Uses fixed timestep for simulation
     */
    void run();

    /**
     * @brief Stop and clean up all subsystems
     * 
     * Stops in reverse initialization order:
     * 1. Rendering
     * 2. Replicator (clean disconnection)
     * 3. InputBuffer
     * 4. EventBus
     * 
     * @note Called automatically by destructor
     * @note Idempotent (can be called multiple times)
     */
    void shutdown();

    /**
     * @brief Stop the game loop
     * 
     * Requests game loop to stop at next iteration.
     * Game will terminate cleanly after current frame.
     * 
     * @note Non-blocking
     * @note Loop terminates after render() of current frame
     */
    void stop();

    /**
     * @brief Set the reconciliation threshold for client-side prediction
     * @param threshold Distance in pixels before server correction is applied
     * 
     * Adjusts how tolerant the client-side prediction is before reconciling with
     * the server's authoritative position. This should be tuned based on observed
     * network conditions:
     * 
     * - Low latency (<50ms): 3.0f - 5.0f pixels (tight sync, minimal drift)
     * - Medium latency (50-150ms): 5.0f - 10.0f pixels (default, balanced)
     * - High latency (>150ms): 10.0f - 20.0f pixels (loose sync, smoother visuals)
     * 
     * Call this method when you detect network latency changes or based on
     * user preferences/settings.
     * 
     * @note Default is 5.0f pixels (suitable for medium latency)
     * @note Delegates to Rendering -> EntityRenderer
     */
    void setReconciliationThreshold(float threshold);

    /**
     * @brief Get the current reconciliation threshold
     * @return Current threshold in pixels
     * 
     * @note Delegates to Rendering -> EntityRenderer
     */
    float getReconciliationThreshold() const;

   private:
    /**
     * @brief Update game logic (variable timestep)
     * 
     * Called once per frame for non-critical logic.
     * 
     * @param deltaTime Time elapsed since last call (in seconds)
     */
    void update(float deltaTime);

    /**
     * @brief Update physics simulation (fixed timestep)
     * 
     * Called N times per frame to guarantee 60 FPS simulation.
     * 
     * @param fixedDeltaTime Fixed simulation time (1/60 = 0.0167s)
     */
    void fixedUpdate(float fixedDeltaTime);

    /**
     * @brief Perform rendering of current frame
     * 
     * Delegates to Rendering system.
     */
    void render();

    /**
     * @brief Process player inputs
     * 
     * Reads keyboard/mouse events and publishes them on EventBus.
     */
    void processInput();

    /**
     * @brief Calculate time elapsed since last frame
     * 
     * @return Delta time in seconds
     */
    float calculateDeltaTime();

    /**
     * @brief Handle incoming network messages
     * 
     * Processes GameStart and GameState messages to update entity rendering.
     * 
     * @param event Network event containing message data
     */
    void handleNetworkMessage(const NetworkEvent &event);
    void handleUIEvent(const UIEvent &event);

    // Network message handlers
    void handleGameStart(const std::vector<uint8_t> &payload);
    void handleGameState(const std::vector<uint8_t> &payload);
    void handleGameruleUpdate(const std::vector<uint8_t> &payload);

    // Helpers
    void processServerReconciliation(const RType::Messages::S2C::EntityState &entity);
    void simulateInputHistory(float &x, float &y);

    EventBus *_eventBus;  // Non-owning pointer (owned by Client)
    std::unique_ptr<InputBuffer> _inputBuffer;
    Replicator *_replicator;  // Non-owning pointer (owned by Client)
    std::unique_ptr<Rendering> _rendering;

    bool _running = false;
    bool _initialized = false;

    float _fixedTimestep = 1.0f / 60.0f;
    float _accumulator = 0.0f;
    uint32_t _currentFrame = 0;

    // Input tracking
    uint32_t _inputSequenceId = 0;

    // Input redundancy history
    static constexpr size_t INPUT_HISTORY_SIZE = 12;  // Send last 12 inputs (~200ms)
    std::deque<RType::Messages::C2S::PlayerInput::InputSnapshot> _inputHistory;

    // Player entity tracking
    std::optional<uint32_t> _myEntityId;       // Local player's entity ID (std::nullopt if not yet assigned)
    bool _entityInitialized = false;           // True after first server update received
    bool _isMoving = false;                    // True when player is actively moving
    float _playerSpeed = 100.0f;               // pixels per second (MUST MATCH SERVER!)
    bool _clientSidePredictionEnabled = true;  // Client-side prediction for smooth movement

    GameScene _currentScene = GameScene::LOBBY;
};

#endif
