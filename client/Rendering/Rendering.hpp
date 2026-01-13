/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** Rendering.hpp
*/

#ifndef RENDERING_HPP
#define RENDERING_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include "../common/Logger/Logger.hpp"
#include "Capnp/Messages/Shared/SharedTypes.hpp"
#include "Core/EventBus/EventBus.hpp"
#include "EntityRenderer.hpp"
#include "Events/UIEvent.hpp"
#include "Graphics/RaylibGraphics/RaylibGraphics.hpp"

// UI library
#include "Menu/AddServerMenu.hpp"
#include "Menu/ConfirmQuitMenu.hpp"
#include "Menu/ConnectionMenu.hpp"
#include "Menu/CreateRoomMenu.hpp"
#include "Menu/LoginMenu.hpp"
#include "Menu/MainMenu.hpp"
#include "Menu/RoomListMenu.hpp"
#include "Menu/ServerListMenu.hpp"
#include "Menu/SettingsMenu.hpp"
#include "Menu/WaitingRoomMenu.hpp"
#include "UI/ChatWidget.hpp"
#include "UI/IButton.hpp"
#include "UI/IMenu.hpp"
#include "UI/Raylib/RaylibUIFactory.hpp"

/**
 * @class Rendering
 * @brief Graphical rendering system using Raylib
 * 
 * Rendering handles the display of all visual game elements:
 * - Game window
 * - Sprites (ships, enemies, projectiles)
 * - Text (UI, score, FPS)
 * - Visual effects
 * 
 * Architecture:
 * - Uses Raylib for 2D rendering
 * - Caches textures for optimization
 * - Handles double-buffering automatically
 * - Integrates with EventBus for visual events
 * 
 */
class Rendering {
   public:
    /**
     * @brief Constructor with EventBus reference
     * 
     * @param eventBus Event bus for inter-component communication
     * 
     * @note Does not create the window, call initialize() after
     */
    explicit Rendering(EventBus &eventBus);

    /**
     * @brief Destructor
     * 
     * Frees Raylib wrapper resources (textures, window, etc.)
     */
    ~Rendering();

    /**
     * @brief Initialize the rendering system and create window
     * 
     * Creates a window through Raylib wrapper with specified parameters.
     * Loads basic resources (fonts, shaders).
     * 
     * @param width Window width in pixels
     * @param height Window height in pixels
     * @param title Window title
     * @return true if initialization succeeds, false otherwise
     * 
     * @note Must be called before any other rendering call
     * @note Recommended: 1920x1080 for R-Type
     * @note Depends on Raylib wrapper implementation (not yet available)
     */
    bool Initialize(uint32_t width, uint32_t height, const std::string &title);

    /**
     * @brief Stop the rendering system and destroy window
     * 
     * Frees all allocated graphical resources.
     * Closes the window properly.
     */
    void Shutdown();

    /**
    * @brief Clears the window before rendering its content.
    *
    * @note Must be called at the beginning of each frame before drawing.
    */
    void ClearWindow();

    /**
     * @brief Perform rendering of current frame
     * 
     * Clears buffer, draws all queued elements, and displays.
     * Must be called once per frame.
     * 
     * @note Uses Raylib wrapper's rendering pipeline
     * @note Synchronized with VSync if enabled
     * @note Implementation pending Raylib wrapper availability
     */
    void Render();

    /**
     * @brief Check if window is open
     * 
     * @return true if window exists and hasn't been closed
     * 
     * @note Returns false if user closes the window
     */
    [[nodiscard]] bool IsWindowOpen() const;

    /**
     * @brief Switch immediately to the game scene
     * 
     * Forces the rendering system to enter the game mode:
     * - Hides all menus
     * - Enables entity rendering
     * - Sets scene to IN_GAME
     * 
     * Useful when the game state is already active (e.g. after login flow).
     */
    void StartGame();

    /**
     * @brief Load a texture from file
     * 
     * Loads a texture into memory and associates it with an identifier.
     * Texture is cached for reuse.
     * 
     * @param id Unique identifier for this texture (e.g., "player_ship")
     * @param path Path to image file (PNG, JPG, BMP)
     * @return true if loading succeeds, false otherwise
     * 
     * @note Textures are shared (same texture = same memory)
     * @note Supported formats depend on Raylib wrapper implementation
     * @note Wrapper implementation pending
     */
    bool LoadTexture(const std::string &textureName, const std::string &path);

    /**
     * @brief Draw a sprite on screen
     * 
     * Adds a sprite to the render queue for current frame.
     * 
     * @param textureId Texture identifier (loaded with loadTexture)
     * @param x X position on screen (0 = left)
     * @param y Y position on screen (0 = top)
     * @param rotation Rotation in degrees (0-360)
     * @param scale Scale (1.0 = original size, 2.0 = double)
     * 
     * @note Sprite is centered on (x, y)
     * @note Call order determines render order (Z-order)
     * @note Uses Raylib wrapper's sprite rendering (pending implementation)
     */
    void DrawSprite(const std::string &textureId, float xPosition, float yPosition, float rotation = 0.0,
                    float scale = 1.0);

    /**
     * @brief Draw text on screen
     * 
     * Displays text with default font.
     * 
     * @param text Text to display (UTF-8 supported)
     * @param x X position (origin at top-left of text)
     * @param y Y position (origin at top-left of text)
     * @param size Font size in pixels (default: 24)
     * 
     * @note Uses default font loaded at initialization
     * @note Rendered on top of sprites (UI)
     * @note Font rendering via Raylib wrapper (not yet implemented)
     */
    void DrawText(const std::string &text, float xPosition, float yPosition, uint32_t size = 24);

    /**
     * @brief Get window width
     * 
     * @return Width in pixels
     */
    [[nodiscard]] uint32_t GetWidth() const;

    /**
     * @brief Get window height
     * 
     * @return Height in pixels
     */
    [[nodiscard]] uint32_t GetHeight() const;

    [[nodiscard]] bool WindowShouldClose() const;

    // ═══════════════════════════════════════════════════════════
    // Entity Rendering API (delegated to EntityRenderer)
    // ═══════════════════════════════════════════════════════════

    /**
     * @brief Update or create an entity for rendering with animation
     * @param id Entity unique identifier
     * @param type Entity type (Player, Enemy, Bullet)
     * @param x World position X
     * @param y World position Y
     * @param health Current health (-1 if not applicable)
     * @param currentAnimation Current animation clip name
     * @param srcX Sprite source X on spritesheet
     * @param srcY Sprite source Y on spritesheet  
     * @param srcW Sprite width
     * @param srcH Sprite height
     * 
     * Delegates to EntityRenderer. Call when receiving GameState updates.
     */
    void UpdateEntity(uint32_t id, RType::Messages::Shared::EntityType type, float x, float y, int health,
                      const std::string &currentAnimation, int srcX, int srcY, int srcW, int srcH);

    /**
     * @brief Remove an entity from rendering
     * @param id Entity unique identifier
     * 
     * Delegates to EntityRenderer. Call when entity is destroyed.
     */
    void RemoveEntity(uint32_t id);

    /**
     * @brief Set the local player's entity ID
     * @param id Entity ID representing the local player
     * 
     * Allows visual differentiation of the player's own entity.
     */
    void SetMyEntityId(uint32_t id);

    /**
     * @brief Clear all entities from the rendering cache
     * 
     * Useful for scene transitions or disconnection.
     */
    void ClearAllEntities();

    /**
     * @brief Check if a key is currently being held down
     * @param key Raylib key code
     * @return true if key is down
     */
    bool IsKeyDown(int key) const;

    /**
     * @brief Update interpolation for all entities
     * @param deltaTime Time elapsed since last frame (in seconds)
     * 
     * Should be called every frame before Render() to smoothly interpolate
     * entity positions between network updates.
     * 
     * Delegates to EntityRenderer.
     */
    void UpdateInterpolation(float deltaTime);

    /**
     * @brief Update ping display timer (called every frame)
     * @param deltaTime Time elapsed since last frame (in seconds)
     * 
     * Updates the displayed ping value once per second to avoid flickering
     * and optimize performance. Should be called from GameLoop's update().
     */
    void UpdatePingTimer(float deltaTime);

    /**
     * @brief Move an entity locally (client-side prediction)
     * @param entityId Entity ID to move
     * @param deltaX Movement in X direction (pixels)
     * @param deltaY Movement in Y direction (pixels)
     * 
     * Used for local player prediction: moves the entity immediately
     * without waiting for server confirmation.
     * 
     * Provides instant (0ms) input response for the local player.
     */
    void MoveEntityLocally(uint32_t entityId, float deltaX, float deltaY);

    /**
     * @brief Update room list from server data
     * @param rooms Vector of room information
     */
    void UpdateRoomList(const std::vector<RoomData> &rooms);

    /**
     * @brief Update waiting room with player list
     * @param players Vector of players in the room
     * @param roomName Name of the room
     * @param isHost Whether local player is the host
     */
    void UpdateWaitingRoom(const std::vector<Game::PlayerInfo> &players, const std::string &roomName,
                           bool isHost);

    /**
     * @brief Add a chat message to the chat widget
     * @param playerId ID of the player who sent the message
     * @param playerName Name of the player
     * @param message The message text
     * @param timestamp Message timestamp
     */
    void AddChatMessage(uint32_t playerId, const std::string &playerName, const std::string &message,
                        uint64_t timestamp);

    /**
     * @brief Set callback for when a chat message is sent
     * @param callback Function to call when user sends a message
     */
    void SetOnChatMessageSent(std::function<void(const std::string &)> callback);

    /**
     * @brief Update chat widget visibility based on current scene
     */
    void UpdateChatVisibility();

    /**
     * @brief Enable or disable client-side prediction for local player
     * @param enabled true to enable prediction (instant movement), false for interpolation
     * 
     * Delegates to EntityRenderer. Should be called when toggling prediction mode.
     */
    void SetClientSidePredictionEnabled(bool enabled);

    /**
     * @brief Set the reconciliation threshold for client-side prediction
     * @param threshold Distance in pixels before server correction is applied
     * 
     * Controls when the client prediction is corrected by the server's authoritative position.
     * Smaller values = more frequent corrections (tighter sync, more visual jitter)
     * Larger values = fewer corrections (looser sync, smoother visuals)
     * 
     * Recommended ranges based on network latency:
     * - Low latency (<50ms): 3.0f - 5.0f pixels
     * - Medium latency (50-150ms): 5.0f - 10.0f pixels (default)
     * - High latency (>150ms): 10.0f - 20.0f pixels
     * 
     * Delegates to EntityRenderer.
     * 
     * @note Default is 5.0f pixels
     */
    void SetReconciliationThreshold(float threshold);

    /**
     * @brief Get the current reconciliation threshold
     * @return Current threshold in pixels
     * 
     * Delegates to EntityRenderer.
     */
    float GetReconciliationThreshold() const;

    /**
     * @brief Set the current ping value for display
     * @param pingMs Ping in milliseconds
     * 
     * Updates the ping value displayed in the top-right corner.
     * Color changes based on quality:
     * - Green: 0-50ms (excellent)
     * - Yellow: 51-100ms (good)
     * - Orange: 101-150ms (fair)
     * - Red: 151+ms (poor)
     */
    void SetPing(uint32_t pingMs);

    /**
     * @brief Enable/disable ping display.
     */
    void SetShowPing(bool enabled);

    /**
     * @brief Get ping display state.
     */
    [[nodiscard]] bool GetShowPing() const;

    /**
     * @brief Enable/disable FPS display.
     */
    void SetShowFps(bool enabled);

    /**
     * @brief Get FPS display state.
     */
    [[nodiscard]] bool GetShowFps() const;

   private:
    enum class Scene { MENU, IN_GAME };

    Scene _scene = Scene::MENU;

    EventBus &_eventBus;
    bool _initialized = false;
    bool _quitRequested = false;
    uint32_t _width = 0;
    uint32_t _height = 0;
    Graphics::RaylibGraphics _graphics;

    // ===== Menu UI (business) =====
    std::unique_ptr<UI::RaylibUIFactory> _uiFactory;
    std::unique_ptr<Game::MainMenu> _mainMenu;
    std::unique_ptr<Game::ServerListMenu> _serverListMenu;
    std::unique_ptr<Game::AddServerMenu> _addServerMenu;
    std::unique_ptr<Game::RoomListMenu> _roomListMenu;
    std::unique_ptr<Game::CreateRoomMenu> _createRoomMenu;
    std::unique_ptr<Game::WaitingRoomMenu> _waitingRoomMenu;
    std::unique_ptr<Game::ConnectionMenu> _connectionMenu;
    std::unique_ptr<Game::SettingsMenu> _settingsMenu;
    std::unique_ptr<Game::ConfirmQuitMenu> _confirmQuitMenu;
    std::unique_ptr<Game::LoginMenu> _loginMenu;

    bool _settingsOverlay = false;
    bool _confirmQuitOverlay = false;
    bool _loginOverlay = false;

    // Selected server for connection
    std::string _selectedServerIp = "127.0.0.1";
    uint16_t _selectedServerPort = 4242;
    bool _isConnecting = false;
    std::string _connectingServerName;

    // Selected room for joining
    std::string _selectedRoomId;

    // Entity rendering subsystem
    std::unique_ptr<EntityRenderer> _entityRenderer;

    // Chat widget
    std::unique_ptr<Game::ChatWidget> _chatWidget;

    // Network stats display (updated once per second for optimization)
    uint32_t _currentPing = 0;
    uint32_t _displayedPing = 0;                         // The ping value currently displayed
    float _pingUpdateTimer = 0.0f;                       // Timer for ping update throttling
    static constexpr float PING_UPDATE_INTERVAL = 1.0f;  // Update every 1 second

    bool _showPing = true;
    bool _showFps = true;

    // ===== HUD stats =====
    uint32_t _fps = 0;
    float _fpsAccumulator = 0.0f;
    uint32_t _fpsFrameCount = 0;

    /**
     * @brief Initialize UI factory, menus and all related callbacks.
     *
     * Keeps Rendering::Initialize() small and focused.
     */
    void InitializeMenus();

    /**
     * @brief Apply runtime settings affecting rendering (target FPS, HUD visibility...).
     */
    void ApplyInitialMenuSettings();

    // ===== Menu initialization helpers (SOLID: Single Responsibility) =====
    void InitializeConfirmQuitMenu();
    void InitializeSettingsMenu();
    void InitializeMainMenu();
    void InitializeLoginMenu();
    void InitializeServerListMenu();
    void InitializeAddServerMenu();
    void InitializeRoomListMenu();
    void InitializeCreateRoomMenu();
    void InitializeWaitingRoomMenu();
    void InitializeConnectionMenu();
    void InitializeChatWidget();
    void SubscribeToConnectionEvents();

    // ===== Helper methods for Render() to reduce cognitive complexity =====

    /**
     * @brief Update FPS counter based on delta time.
     */
    void UpdateFpsCounter();

    /**
     * @brief Handle ESC key input to toggle settings overlay in-game.
     */
    void HandleEscapeKeyInput();

    /**
     * @brief Update all UI elements based on current scene.
     */
    void UpdateUI();

    /**
     * @brief Render the game scene (entities).
     */
    void RenderGameScene();

    /**
     * @brief Render all UI menus based on current scene and overlay state.
     */
    void RenderUI();

    /**
     * @brief Render HUD elements (ping, FPS).
     */
    void RenderHUD();
};
#endif
