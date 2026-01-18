/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** Rendering.cpp
*/

#include "Rendering.hpp"
#include <thread>
#include "Events/UIEvent.hpp"
#include "UI/TextUtils.hpp"

Rendering::Rendering(EventBus &eventBus) : _eventBus(eventBus) {
    // Note: Window creation is deferred to Initialize()
    // This allows GameLoop to control initialization timing
    _entityRenderer = std::make_unique<EntityRenderer>(_graphics);
}

Rendering::~Rendering() {
    Shutdown();
}

bool Rendering::Initialize(uint32_t width, uint32_t height, const std::string &title) {
    if (_initialized) {
        LOG_WARNING("Rendering already initialized, skipping");
        return true;
    }

    _graphics.InitWindow(static_cast<int>(width), static_cast<int>(height), title.c_str());

    _width = width;
    _height = height;

    _quitRequested = false;

    _showPing = true;
    _showFps = true;

    _fps = 0;
    _fpsAccumulator = 0.0f;
    _fpsFrameCount = 0;

    InitializeMenus();
    ApplyInitialMenuSettings();

    // Start with server selection menu
    _entityRenderer.reset();

    _initialized = true;
    LOG_INFO("Rendering initialized: ", width, "x", height, " (", title, ")");
    return true;
}

void Rendering::InitializeMenus() {
    _uiFactory = std::make_unique<UI::RaylibUIFactory>(_graphics);

    // Initialize sound effect manager for UI sounds
    _soundEffectManager = std::make_unique<Audio::SoundEffectManager>(_graphics);
    if (!_soundEffectManager->Initialize()) {
        LOG_WARNING("[Rendering] Sound effect manager initialization failed - sounds disabled");
    }

    InitializeConfirmQuitMenu();
    InitializeSettingsMenu();
    InitializeMainMenu();
    InitializeLoginMenu();
    InitializeServerListMenu();
    InitializeAddServerMenu();
    InitializeRoomListMenu();
    InitializeCreateRoomMenu();
    InitializeWaitingRoomMenu();
    InitializeConnectionMenu();
    InitializeChatWidget();
    SubscribeToConnectionEvents();
}

void Rendering::ApplyInitialMenuSettings() {
    // Default: 60 FPS cap at startup
    _graphics.SetTargetFPS(60);

    // Keep menu button state consistent with renderer state (silent sync)
    if (_settingsMenu) {
        _settingsMenu->SetTargetFpsSilent(60);
        _settingsMenu->SetShowPingSilent(_showPing);
        _settingsMenu->SetShowFpsSilent(_showFps);
        _settingsMenu->RefreshVisuals();
    }
}

// ===== Menu Initialization Functions (SOLID: Single Responsibility) =====

void Rendering::InitializeConfirmQuitMenu() {
    _confirmQuitMenu = std::make_unique<Game::ConfirmQuitMenu>(*_uiFactory);
    _confirmQuitMenu->SetSoundEffectService(_soundEffectManager.get());

    _confirmQuitMenu->SetOnConfirm([this]() { _quitRequested = true; });

    _confirmQuitMenu->SetOnCancel([this]() {
        if (_confirmQuitMenu) {
            _confirmQuitMenu->Hide();
        }
        _confirmQuitOverlay = false;

        // Return to previous UI depending on scene
        if (_scene == Scene::MENU) {
            if (_mainMenu) {
                _mainMenu->Show();
            }
        }
    });

    _confirmQuitMenu->Initialize();
    _confirmQuitMenu->Hide();
}

void Rendering::InitializeSettingsMenu() {
    _settingsMenu = std::make_unique<Game::SettingsMenu>(*_uiFactory, _graphics);
    _settingsMenu->SetSoundEffectService(_soundEffectManager.get());
    _settingsMenu->SetMode(Game::SettingsMenu::Mode::FULLSCREEN);

    _settingsMenu->SetOnShowPingChanged([this](bool enabled) { SetShowPing(enabled); });

    _settingsMenu->SetOnShowFpsChanged([this](bool enabled) { SetShowFps(enabled); });

    _settingsMenu->SetOnShowChatChanged([this](bool enabled) {
        if (_chatWidget) {
            _chatWidget->SetVisible(enabled);
        }
    });

    _settingsMenu->SetOnTargetFpsChanged(
        [this](uint32_t fps) { _graphics.SetTargetFPS(static_cast<int>(fps)); });

    _settingsMenu->SetOnBack([this]() {
        if (_settingsMenu) {
            _settingsMenu->Hide();
        }
        if (_scene == Scene::MENU) {
            if (_mainMenu) {
                _mainMenu->Show();
            }
        }
        _settingsOverlay = false;
    });

    _settingsMenu->SetOnMainMenu([this]() {
        // Return to main menu while in-game
        LOG_INFO("[Rendering] Back to main menu - leaving room");
        _scene = Scene::MENU;
        _settingsOverlay = false;

        // Notify server that player is leaving the room
        _eventBus.publish(UIEvent(UIEventType::LEAVE_ROOM));

        if (_settingsMenu) {
            _settingsMenu->Hide();
            if (_settingsMenu->GetMode() != Game::SettingsMenu::Mode::FULLSCREEN) {
                _settingsMenu->SetMode(Game::SettingsMenu::Mode::FULLSCREEN);
                _settingsMenu->Initialize();
            } else {
                _settingsMenu->RefreshVisuals();
            }
        }

        if (_entityRenderer) {
            _entityRenderer->clearAllEntities();
        }
        _entityRenderer.reset();

        if (_mainMenu) {
            _mainMenu->Show();
        }
    });

    _settingsMenu->Initialize();
    _settingsMenu->Hide();
}

void Rendering::InitializeMainMenu() {
    _mainMenu = std::make_unique<Game::MainMenu>(*_uiFactory);
    _mainMenu->SetSoundEffectService(_soundEffectManager.get());

    _mainMenu->SetOnPlay([this]() {
        if (_mainMenu)
            _mainMenu->Hide();

        // Show room selection menu
        if (_roomListMenu)
            _roomListMenu->Show();

        // Request room list from server
        _eventBus.publish(UIEvent(UIEventType::REQUEST_ROOM_LIST));
    });

    _mainMenu->SetOnQuit([this]() { _quitRequested = true; });

    _mainMenu->SetOnSettings([this]() {
        if (_mainMenu) {
            _mainMenu->Hide();
        }
        if (_settingsMenu) {
            _settingsMenu->SetMode(Game::SettingsMenu::Mode::FULLSCREEN);
            _settingsMenu->Show();
        }
        _settingsOverlay = false;
    });

    _mainMenu->SetOnProfile([this]() {
        if (_mainMenu)
            _mainMenu->Hide();
        if (_loginMenu)
            _loginMenu->Show();
        _loginOverlay = true;
    });

    _mainMenu->SetOnSelectServer([this]() {
        if (_mainMenu)
            _mainMenu->Hide();
        if (_serverListMenu)
            _serverListMenu->Show();
    });

    // Pass screen dimensions for responsive layout (profile button)
    _mainMenu->SetScreenSize(static_cast<float>(_width), static_cast<float>(_height));

    _mainMenu->Initialize();
    _mainMenu->Hide();  // Start hidden, show after server selection
}

void Rendering::InitializeLoginMenu() {
    _loginMenu = std::make_unique<Game::LoginMenu>(*_uiFactory, _graphics);
    _loginMenu->SetSoundEffectService(_soundEffectManager.get());

    _loginMenu->SetOnBack([this]() {
        if (_loginMenu)
            _loginMenu->Hide();
        if (_mainMenu)
            _mainMenu->Show();
        _loginOverlay = false;
    });

    _loginMenu->Initialize();
    _loginMenu->Hide();
}

void Rendering::InitializeServerListMenu() {
    _serverListMenu = std::make_unique<Game::ServerListMenu>(*_uiFactory, _graphics);
    _serverListMenu->SetSoundEffectService(_soundEffectManager.get());

    _serverListMenu->SetOnBack([this]() {
        // Back from server selection = quit game
        _quitRequested = true;
    });

    _serverListMenu->SetOnServerSelected([this](const std::string &ip, uint16_t port) {
        // Get server name for display
        _connectingServerName = "Unknown";
        for (const auto &server : _serverListMenu->GetServers()) {
            if (server.ip == ip && server.port == port) {
                _connectingServerName = server.name;
                break;
            }
        }

        // Store the selected server info
        _selectedServerIp = ip;
        _selectedServerPort = port;
        LOG_INFO("[Rendering] Connecting to server: ", ip, ":", port, "...");

        // Set connecting state in ServerListMenu
        if (_serverListMenu)
            _serverListMenu->SetConnecting(true, _connectingServerName);

        // Set connecting state
        _isConnecting = true;

        // Publish Server Connect event to trigger connection (async)
        std::string serverInfo = ip + ":" + std::to_string(port);
        _eventBus.publish(UIEvent(UIEventType::SERVER_CONNECT, serverInfo));

        // Don't block here - let the CONNECTION_FAILED or connection success handle UI updates
    });

    _serverListMenu->SetOnAddServer([this]() {
        if (_serverListMenu)
            _serverListMenu->Hide();
        if (_addServerMenu)
            _addServerMenu->Show();
    });

    _serverListMenu->Initialize();
    _serverListMenu->Show();  // Show at startup
}

void Rendering::InitializeAddServerMenu() {
    _addServerMenu = std::make_unique<Game::AddServerMenu>(*_uiFactory, _graphics);
    _addServerMenu->SetSoundEffectService(_soundEffectManager.get());

    _addServerMenu->SetOnCancel([this]() {
        if (_addServerMenu)
            _addServerMenu->Hide();
        if (_serverListMenu)
            _serverListMenu->Show();
    });

    _addServerMenu->SetOnAdd([this](const std::string &name, const std::string &ip, const std::string &port) {
        LOG_INFO("[Rendering] Adding server: ", name, " - ", ip, ":", port);

        // Add server to list
        if (_serverListMenu) {
            try {
                uint16_t portNum = static_cast<uint16_t>(std::stoi(port));
                _serverListMenu->AddServer(name, ip, portNum);
            } catch (...) {
                LOG_ERROR("[Rendering] Failed to parse port: ", port);
            }
        }

        // Hide add server menu and show server list
        if (_addServerMenu)
            _addServerMenu->Hide();
        if (_serverListMenu)
            _serverListMenu->Show();
    });

    _addServerMenu->Initialize();
    _addServerMenu->Hide();
}

void Rendering::InitializeConnectionMenu() {
    _connectionMenu = std::make_unique<Game::ConnectionMenu>(*_uiFactory, _graphics);
    _connectionMenu->SetSoundEffectService(_soundEffectManager.get());

    _connectionMenu->SetOnBack([this]() {
        if (_connectionMenu)
            _connectionMenu->Hide();
        if (_mainMenu)
            _mainMenu->Show();
    });

    _connectionMenu->SetOnJoin(
        [this](const std::string &nickname, const std::string &ip, const std::string &port) {
            (void)nickname;
            (void)ip;
            (void)port;

            // Publish Join Game event
            _eventBus.publish(UIEvent(UIEventType::JOIN_GAME));

            if (_connectionMenu)
                _connectionMenu->Hide();
            StartGame();
        });

    _connectionMenu->Initialize();
    _connectionMenu->Hide();
}

void Rendering::InitializeRoomListMenu() {
    _roomListMenu = std::make_unique<Game::RoomListMenu>(*_uiFactory, _graphics);
    _roomListMenu->SetSoundEffectService(_soundEffectManager.get());

    _roomListMenu->SetOnRoomSelected([this](const std::string &roomId) {
        LOG_INFO("[Rendering] Room selected: ", roomId);
        _selectedRoomId = roomId;

        // Hide room list and show waiting room
        if (_roomListMenu)
            _roomListMenu->Hide();
        if (_waitingRoomMenu)
            _waitingRoomMenu->Show();

        // Send JOIN_GAME event with room ID (but don't start game yet)
        _eventBus.publish(UIEvent(UIEventType::JOIN_GAME, _selectedRoomId));
    });

    _roomListMenu->SetOnCreateRoom([this]() {
        if (_roomListMenu)
            _roomListMenu->Hide();
        if (_createRoomMenu)
            _createRoomMenu->Show();
    });

    _roomListMenu->SetOnBack([this]() {
        if (_roomListMenu)
            _roomListMenu->Hide();
        if (_mainMenu)
            _mainMenu->Show();
    });

    _roomListMenu->Initialize();
    _roomListMenu->Hide();
}

void Rendering::InitializeCreateRoomMenu() {
    _createRoomMenu = std::make_unique<Game::CreateRoomMenu>(*_uiFactory, _graphics);
    _createRoomMenu->SetSoundEffectService(_soundEffectManager.get());

    _createRoomMenu->SetOnCreate([this](const std::string &roomName, uint32_t maxPlayers, bool isPrivate) {
        LOG_INFO("[Rendering] Creating room: ", roomName, " (Max: ", maxPlayers, ", Private: ", isPrivate,
                 ")");

        // Publish CREATE_ROOM event (format: "roomName|maxPlayers|isPrivate")
        std::string roomData = roomName + "|" + std::to_string(maxPlayers) + "|" + (isPrivate ? "1" : "0");
        _eventBus.publish(UIEvent(UIEventType::CREATE_ROOM, roomData));

        if (_createRoomMenu)
            _createRoomMenu->Hide();

        // Show WaitingRoom instead of RoomListMenu (creator becomes host automatically)
        if (_waitingRoomMenu)
            _waitingRoomMenu->Show();
    });

    _createRoomMenu->SetOnCancel([this]() {
        if (_createRoomMenu)
            _createRoomMenu->Hide();
        if (_roomListMenu)
            _roomListMenu->Show();
    });

    _createRoomMenu->Initialize();
    _createRoomMenu->Hide();
}

void Rendering::InitializeWaitingRoomMenu() {
    _waitingRoomMenu = std::make_unique<Game::WaitingRoomMenu>(*_uiFactory, _graphics);
    _waitingRoomMenu->SetSoundEffectService(_soundEffectManager.get());

    _waitingRoomMenu->SetOnStartGame([this]() {
        LOG_INFO("[Rendering] Start Game button clicked");
        // Publish START_GAME_REQUEST event
        _eventBus.publish(UIEvent(UIEventType::START_GAME_REQUEST));
    });

    _waitingRoomMenu->SetOnBack([this]() {
        LOG_INFO("[Rendering] Back to room list - leaving room");
        if (_waitingRoomMenu)
            _waitingRoomMenu->Hide();

        // Notify server that player is leaving the room
        _eventBus.publish(UIEvent(UIEventType::LEAVE_ROOM));

        if (_roomListMenu)
            _roomListMenu->Show();
    });

    _waitingRoomMenu->Initialize();
    _waitingRoomMenu->Hide();
}

void Rendering::SubscribeToConnectionEvents() {
    _eventBus.subscribe<UIEvent>([this](const UIEvent &event) {
        if (event.getType() == UIEventType::AUTH_SUCCESS) {
            // Update player name after authentication
            const std::string &username = event.getData();
            SetPlayerName(username);

            // Hide both login and server list menus, show main menu
            if (_loginMenu)
                _loginMenu->Hide();
            if (_serverListMenu)
                _serverListMenu->Hide();
            if (_mainMenu)
                _mainMenu->Show();

            LOG_INFO("[Rendering] Authentication successful, returning to main menu");
        } else if (event.getType() == UIEventType::CONNECTION_SUCCESS) {
            LOG_INFO("[Rendering] Connection successful!");

            // Clear connecting state
            _isConnecting = false;
            if (_serverListMenu)
                _serverListMenu->SetConnecting(false);

            // Request room list from server
            _eventBus.publish(UIEvent(UIEventType::REQUEST_ROOM_LIST));

            // Hide server list and show main menu (room selection happens on PLAY click)
            if (_serverListMenu)
                _serverListMenu->Hide();
            if (_mainMenu)
                _mainMenu->Show();

        } else if (event.getType() == UIEventType::CONNECTION_FAILED) {
            LOG_ERROR("[Rendering] Connection failed: ", event.getData());

            // Clear connecting state
            _isConnecting = false;
            if (_serverListMenu)
                _serverListMenu->SetConnecting(false);

            // Set error message in server list (it's already visible)
            if (_serverListMenu) {
                _serverListMenu->SetConnectionError("Connection failed: Server unreachable");
            }

            // Make sure we're back to menu scene
            _scene = Scene::MENU;
        } else if (event.getType() == UIEventType::ROOM_LIST_RECEIVED) {
            LOG_INFO("[Rendering] Room list received");
            // Room list will be updated by GameLoop parsing the network message
        } else if (event.getType() == UIEventType::REGISTER_SUCCESS) {
            LOG_INFO("[Rendering] Registration successful: ", event.getData());
            if (_loginMenu) {
                _loginMenu->SetSuccessMessage("Registration successful! You can now login.");
            }
        } else if (event.getType() == UIEventType::REGISTER_FAILED) {
            LOG_ERROR("[Rendering] Registration failed: ", event.getData());
            if (_loginMenu) {
                _loginMenu->SetErrorMessage(event.getData());
            }
        } else if (event.getType() == UIEventType::LOGIN_FAILED) {
            LOG_ERROR("[Rendering] Login failed: ", event.getData());
            if (_loginMenu) {
                _loginMenu->SetErrorMessage(event.getData());
            }
        }
    });
}

// ===== End of Menu Initialization Functions =====

void Rendering::Shutdown() {
    if (!_initialized) {
        return;
    }
    _graphics.CloseWindow();
    _initialized = false;
}

void Rendering::ClearWindow() {
    if (!_initialized) {
        return;
    }

    // Kept for legacy callers; main loop uses Render() which clears.
    _graphics.ClearWindow();
}

void Rendering::SetShowPing(bool enabled) {
    _showPing = enabled;
}

bool Rendering::GetShowPing() const {
    return _showPing;
}

void Rendering::SetShowFps(bool enabled) {
    _showFps = enabled;
}

bool Rendering::GetShowFps() const {
    return _showFps;
}

void Rendering::SetPlayerName(const std::string &name) {
    if (_mainMenu) {
        _mainMenu->SetProfileName(name);
    }
    LOG_INFO("[Rendering] Player name updated to: ", name);
}

void Rendering::StartGame() {
    if (!_initialized)
        return;

    LOG_INFO("Rendering: Force switching to Game Scene");
    _scene = Scene::IN_GAME;

    // Hide all menus
    if (_mainMenu)
        _mainMenu->Hide();
    if (_serverListMenu)
        _serverListMenu->Hide();
    if (_addServerMenu)
        _addServerMenu->Hide();
    if (_roomListMenu)
        _roomListMenu->Hide();
    if (_createRoomMenu)
        _createRoomMenu->Hide();
    if (_waitingRoomMenu)
        _waitingRoomMenu->Hide();
    if (_connectionMenu)
        _connectionMenu->Hide();
    if (_settingsMenu)
        _settingsMenu->Hide();

    _settingsOverlay = false;

    // Enable entity renderer
    if (!_entityRenderer) {
        _entityRenderer = std::make_unique<EntityRenderer>(_graphics);
    }
}

void Rendering::Render() {
    if (!_initialized) {
        return;
    }

    UpdateFpsCounter();

    if (_quitRequested) {
        _initialized = false;
        return;
    }

    HandleEscapeKeyInput();
    UpdateUI();

    _graphics.StartDrawing();
    _graphics.ClearWindow();

    RenderGameScene();
    RenderUI();
    RenderHUD();

    _graphics.DisplayWindow();

    // Check if user wants to close the window and stop the game loop
    if (_graphics.WindowShouldClose()) {
        // Trigger game loop shutdown
        // This will be detected by the GameLoop and initiate clean shutdown
    }
}

void Rendering::UpdateFpsCounter() {
    const float dt = _graphics.GetDeltaTime();
    _fpsAccumulator += dt;
    _fpsFrameCount += 1;
    if (_fpsAccumulator >= 1.0f) {
        _fps = _fpsFrameCount;
        _fpsFrameCount = 0;
        _fpsAccumulator = 0.0f;
    }
}

void Rendering::HandleEscapeKeyInput() {
    // ESC toggles settings overlay only in-game
    if (_scene != Scene::IN_GAME || !_graphics.IsKeyPressed(KEY_ESCAPE) || !_settingsMenu) {
        return;
    }

    if (_settingsMenu->IsVisible() && _settingsOverlay) {
        _settingsMenu->Hide();
        _settingsOverlay = false;
    } else {
        // Only rebuild menu if the mode actually changes (button count differs)
        if (_settingsMenu->GetMode() != Game::SettingsMenu::Mode::OVERLAY) {
            _settingsMenu->SetMode(Game::SettingsMenu::Mode::OVERLAY);
            _settingsMenu->Initialize();
        } else {
            _settingsMenu->RefreshVisuals();
        }
        _settingsMenu->Show();
        _settingsOverlay = true;
    }
}

void Rendering::UpdateUI() {
    // Update chat visibility based on scene
    UpdateChatVisibility();

    // Update chat widget
    if (_chatWidget && _chatWidget->IsVisible()) {
        _chatWidget->Update();
    }

    if (_confirmQuitMenu && _confirmQuitMenu->IsVisible()) {
        _confirmQuitMenu->Update();
        return;
    }

    if (_scene == Scene::MENU) {
        if (_mainMenu && _mainMenu->IsVisible()) {
            _mainMenu->Update();
        }
        if (_serverListMenu && _serverListMenu->IsVisible()) {
            _serverListMenu->Update();
        }
        if (_addServerMenu && _addServerMenu->IsVisible()) {
            _addServerMenu->Update();
        }
        if (_roomListMenu && _roomListMenu->IsVisible()) {
            _roomListMenu->Update();
        }
        if (_createRoomMenu && _createRoomMenu->IsVisible()) {
            _createRoomMenu->Update();
        }
        if (_waitingRoomMenu && _waitingRoomMenu->IsVisible()) {
            _waitingRoomMenu->Update();
        }
        if (_connectionMenu && _connectionMenu->IsVisible()) {
            _connectionMenu->Update();
        }
        if (_settingsMenu && _settingsMenu->IsVisible()) {
            _settingsMenu->Update();
        }
        if (_loginMenu && _loginMenu->IsVisible()) {
            _loginMenu->Update();

            // Check for submission
            if (_loginMenu->IsRegisterSubmitted()) {
                // Register: Send RegisterAccount event
                std::string username = _loginMenu->GetUsername();
                std::string password = _loginMenu->GetPassword();

                if (!username.empty() && !password.empty()) {
                    LOG_INFO("[Rendering] Sending register request for user: " + username);
                    // Publish event with "username:password" format
                    std::string credentials = username + ":" + password;
                    _eventBus.publish(UIEvent(UIEventType::REGISTER_ACCOUNT, credentials));

                    // Reset the menu state so it doesn't keep sending
                    _loginMenu->Reset();
                } else {
                    _loginMenu->SetErrorMessage("Please enter username and password");
                }

            } else if (_loginMenu->IsLoginSubmitted()) {
                // Login: Send LoginAccount event
                std::string username = _loginMenu->GetUsername();
                std::string password = _loginMenu->GetPassword();

                if (!username.empty() && !password.empty()) {
                    LOG_INFO("[Rendering] Sending login request for user: " + username);
                    // Publish event with "username:password" format
                    std::string credentials = username + ":" + password;
                    _eventBus.publish(UIEvent(UIEventType::LOGIN_ACCOUNT, credentials));

                    // Reset the menu state so it doesn't keep sending
                    _loginMenu->Reset();
                } else {
                    _loginMenu->SetErrorMessage("Please enter username and password");
                }

            } else if (_loginMenu->IsGuestSubmitted()) {
                // Guest login: send guest credentials to re-authenticate as guest
                LOG_INFO("[Rendering] Guest login selected - sending guest credentials");

                // Send guest credentials
                std::string credentials = "guest:guest";
                _eventBus.publish(UIEvent(UIEventType::LOGIN_ACCOUNT, credentials));

                // Reset the menu state
                _loginMenu->Reset();
            }
        }
    } else {
        // In-game: only overlay settings gets updates
        if (_settingsMenu && _settingsMenu->IsVisible() && _settingsOverlay) {
            _settingsMenu->Update();
        }
    }
}

void Rendering::RenderGameScene() {
    if (_scene == Scene::IN_GAME && _entityRenderer) {
        _entityRenderer->render();
    }
}

void Rendering::RenderUI() {
    if (_confirmQuitMenu && _confirmQuitMenu->IsVisible()) {
        if (_confirmQuitOverlay) {
            _graphics.DrawRectFilled(0, 0, static_cast<int>(_width), static_cast<int>(_height), 0x88000000);
        }
        _confirmQuitMenu->Render();
        return;
    }

    if (_scene == Scene::MENU) {
        if (_mainMenu && _mainMenu->IsVisible()) {
            _mainMenu->Render();
        }
        if (_serverListMenu && _serverListMenu->IsVisible()) {
            _serverListMenu->Render();
        }
        if (_addServerMenu && _addServerMenu->IsVisible()) {
            _addServerMenu->Render();
        }
        if (_roomListMenu && _roomListMenu->IsVisible()) {
            _roomListMenu->Render();
        }
        if (_createRoomMenu && _createRoomMenu->IsVisible()) {
            _createRoomMenu->Render();
        }
        if (_waitingRoomMenu && _waitingRoomMenu->IsVisible()) {
            _waitingRoomMenu->Render();
        }
        if (_connectionMenu && _connectionMenu->IsVisible()) {
            _connectionMenu->Render();
        }
        if (_settingsMenu && _settingsMenu->IsVisible()) {
            _settingsMenu->Render();
        }
        if (_loginMenu && _loginMenu->IsVisible()) {
            _loginMenu->Render();
        }
    } else {
        if (_settingsMenu && _settingsMenu->IsVisible() && _settingsOverlay) {
            if (_settingsMenu->ShouldDimBackground()) {
                _graphics.DrawRectFilled(0, 0, static_cast<int>(_width), static_cast<int>(_height),
                                         _settingsMenu->GetOverlayDimColor());
            }
            _settingsMenu->Render();
        }
    }

    // Render chat widget (on top of everything)
    if (_chatWidget && _chatWidget->IsVisible()) {
        _chatWidget->Render();
    }
}

void Rendering::RenderHUD() {
    const int fontSize = 20;
    const int margin = 10;
    const int pad = 6;

    // Ping (top-right)
    int hudRightX = _graphics.GetWindowWidth() - margin;
    int hudY = margin;

    if (_showPing) {
        const bool hasMeasurement = (_displayedPing != 0);
        const std::string pingText =
            hasMeasurement ? ("PING: " + std::to_string(_displayedPing) + " ms") : std::string("PING: --");

        const int textWidth = UI::TextUtils::EstimateTextWidth(pingText, fontSize);
        const int x = hudRightX - textWidth;
        const int y = hudY;

        _graphics.DrawRectFilled(x - pad, y - pad, textWidth + pad * 2, fontSize + pad * 2, 0x88000000);
        _graphics.DrawText(-1, pingText.c_str(), x, y, fontSize, 0xFFFFFFFF);

        hudY += fontSize + pad * 2 + 6;
    }

    // FPS (below ping, top-right)
    if (_showFps) {
        const std::string fpsText = "FPS: " + std::to_string(_fps);
        const int textWidth = UI::TextUtils::EstimateTextWidth(fpsText, fontSize);
        const int x = hudRightX - textWidth;
        const int y = hudY;

        _graphics.DrawRectFilled(x - pad, y - pad, textWidth + pad * 2, fontSize + pad * 2, 0x88000000);
        _graphics.DrawText(-1, fpsText.c_str(), x, y, fontSize, 0xFFFFFFFF);
    }
}

bool Rendering::IsWindowOpen() const {
    if (!_initialized) {
        return false;
    }
    return _initialized && _graphics.IsWindowOpen();
}

bool Rendering::LoadTexture(const std::string &textureName, const std::string &path) {
    int result = _graphics.LoadTexture(textureName.c_str(), path.c_str());
    return result != -1;
}

void Rendering::DrawSprite(const std::string &textureId, float xPosition, float yPosition, float rotation,
                           float scale) {
    _graphics.DrawTextureEx(textureId.c_str(), 0, 0, 0, 0, xPosition, yPosition, rotation, scale, 0xFFFFFFFF);
}

void Rendering::DrawText(const std::string &text, float xPosition, float yPosition, uint32_t size) {
    _graphics.DrawText(-1, text.c_str(), static_cast<int>(xPosition), static_cast<int>(yPosition),
                       static_cast<int>(size), 0xFFFFFFFF);
}

uint32_t Rendering::GetWidth() const {
    return _width;
}

uint32_t Rendering::GetHeight() const {
    return _height;
}

bool Rendering::WindowShouldClose() const {
    if (!_initialized) {
        return false;
    }
    return _graphics.WindowShouldClose();
}

// ═══════════════════════════════════════════════════════════
// Entity Rendering API (delegation to EntityRenderer)
// ═══════════════════════════════════════════════════════════

void Rendering::UpdateEntity(uint32_t id, RType::Messages::Shared::EntityType type, float x, float y,
                             int health, const std::string &currentAnimation, int srcX, int srcY, int srcW,
                             int srcH) {
    if (_entityRenderer) {
        _entityRenderer->updateEntity(id, type, x, y, health, currentAnimation, srcX, srcY, srcW, srcH);
    }
}

void Rendering::RemoveEntity(uint32_t id) {
    if (_entityRenderer) {
        _entityRenderer->removeEntity(id);
    }
}

void Rendering::SetMyEntityId(uint32_t id) {
    if (_entityRenderer) {
        _entityRenderer->setMyEntityId(id);
    }
}

void Rendering::ClearAllEntities() {
    if (_entityRenderer) {
        _entityRenderer->clearAllEntities();
    }
}

bool Rendering::IsKeyDown(int key) const {
    return _graphics.IsKeyDown(key);
}

void Rendering::UpdateInterpolation(float deltaTime) {
    if (_entityRenderer) {
        _entityRenderer->updateInterpolation(deltaTime);
    }
}

void Rendering::UpdatePingTimer(float deltaTime) {
    _pingUpdateTimer += deltaTime;

    // Update displayed ping once per second
    if (_pingUpdateTimer >= PING_UPDATE_INTERVAL) {
        _displayedPing = _currentPing;
        _pingUpdateTimer = 0.0f;
    }
}

void Rendering::MoveEntityLocally(uint32_t entityId, float deltaX, float deltaY) {
    if (_entityRenderer) {
        _entityRenderer->moveEntityLocally(entityId, deltaX, deltaY);
    }
}

void Rendering::SetClientSidePredictionEnabled(bool enabled) {
    if (_entityRenderer) {
        _entityRenderer->setClientSidePredictionEnabled(enabled);
    }
}

void Rendering::SetReconciliationThreshold(float threshold) {
    if (_entityRenderer) {
        _entityRenderer->setReconciliationThreshold(threshold);
    }
}

float Rendering::GetReconciliationThreshold() const {
    if (_entityRenderer) {
        return _entityRenderer->getReconciliationThreshold();
    }
    return 5.0f;  // Default value
}

void Rendering::SetLocalPlayerMoving(bool moving) {
    if (_entityRenderer) {
        _entityRenderer->setLocalPlayerMoving(moving);
    }
}

void Rendering::SetPing(uint32_t pingMs) {
    _currentPing = pingMs;
}

void Rendering::UpdateRoomList(const std::vector<RoomData> &rooms) {
    if (!_roomListMenu) {
        return;
    }

    std::vector<Game::RoomListMenu::RoomInfo> roomInfos;
    for (const auto &room : rooms) {
        roomInfos.emplace_back(room.roomId, room.roomName, room.playerCount, room.maxPlayers, room.isPrivate,
                               room.state);
    }

    _roomListMenu->UpdateRoomList(roomInfos);
    LOG_INFO("[Rendering] Room list updated with ", rooms.size(), " rooms");
}

void Rendering::UpdateWaitingRoom(const std::vector<Game::PlayerInfo> &players, const std::string &roomName,
                                  bool isHost, bool isSpectator) {
    if (!_waitingRoomMenu) {
        return;
    }

    _waitingRoomMenu->UpdatePlayerList(players);
    _waitingRoomMenu->SetRoomInfo(roomName, static_cast<uint32_t>(players.size()),
                                  4);  // TODO: get max from server
    _waitingRoomMenu->SetIsHost(isHost);
    _waitingRoomMenu->SetIsSpectator(isSpectator);

    LOG_INFO("[Rendering] Waiting room updated with ", players.size(), " players, isHost=", isHost,
             ", isSpectator=", isSpectator);
}

void Rendering::InitializeChatWidget() {
    if (!_uiFactory) {
        LOG_ERROR("[Rendering] Cannot initialize chat widget: UIFactory not initialized");
        return;
    }

    _chatWidget = std::make_unique<Game::ChatWidget>(*_uiFactory, _graphics);
    _chatWidget->Initialize();

    float x = _width - 300.0f;
    float y = _height - 240.0f;

    _chatWidget->SetPosition(x, y);
    _chatWidget->SetVisible(false);  // Will be set by UpdateChatVisibility

    LOG_INFO("[Rendering] Chat widget initialized");
}

void Rendering::AddChatMessage(uint32_t playerId, const std::string &playerName, const std::string &message,
                               uint64_t timestamp) {
    if (_chatWidget) {
        _chatWidget->AddMessage(playerId, playerName, message, timestamp);
    }
}

void Rendering::SetOnChatMessageSent(std::function<void(const std::string &)> callback) {
    if (_chatWidget) {
        _chatWidget->SetOnMessageSent(callback);
    }
}

void Rendering::UpdateChatVisibility() {
    if (!_chatWidget) {
        return;
    }

    // Show chat in waiting room or in-game, but only if enabled in settings
    bool shouldBeVisible = (_scene == Scene::IN_GAME) || (_waitingRoomMenu && _waitingRoomMenu->IsVisible());

    // Check if chat is enabled in settings
    if (_settingsMenu && !_settingsMenu->GetShowChat()) {
        shouldBeVisible = false;
    }

    _chatWidget->SetVisible(shouldBeVisible);
}
