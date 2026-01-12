/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** Rendering.cpp
*/

#include "Rendering.hpp"
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

    // Start in menu: disable entity renderer until Play
    _entityRenderer.reset();

    _initialized = true;
    LOG_INFO("Rendering initialized: ", width, "x", height, " (", title, ")");
    return true;
}

void Rendering::InitializeMenus() {
    _uiFactory = std::make_unique<UI::RaylibUIFactory>(_graphics);

    // ===== Confirm quit dialog =====
    _confirmQuitMenu = std::make_unique<Game::ConfirmQuitMenu>(*_uiFactory);
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

    // ===== Settings menu =====
    _settingsMenu = std::make_unique<Game::SettingsMenu>(*_uiFactory, _graphics);
    _settingsMenu->SetMode(Game::SettingsMenu::Mode::FULLSCREEN);

    _settingsMenu->SetOnShowPingChanged([this](bool enabled) { SetShowPing(enabled); });
    _settingsMenu->SetOnShowFpsChanged([this](bool enabled) { SetShowFps(enabled); });
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
        _scene = Scene::MENU;
        _settingsOverlay = false;
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

    // ===== Main menu =====
    _mainMenu = std::make_unique<Game::MainMenu>(*_uiFactory);
    _mainMenu->SetOnPlay([this]() {
        if (_mainMenu)
            _mainMenu->Hide();
        if (_connectionMenu) {
            _connectionMenu->Show();
        }
    });

    _mainMenu->SetOnQuit([this]() {
        _quitRequested = true;
        // Or show confirm quit menu
        // if (_mainMenu) _mainMenu->Hide();
        // if (_confirmQuitMenu) _confirmQuitMenu->Show();
        // _confirmQuitOverlay = true;
    });

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

    // Pass screen dimensions for responsive layout (profile button)
    _mainMenu->SetScreenSize(static_cast<float>(_width), static_cast<float>(_height));

    _mainMenu->Initialize();
    _mainMenu->Show();

    // ===== Login Menu =====
    _loginMenu = std::make_unique<Game::LoginMenu>(*_uiFactory, _graphics);
    _loginMenu->SetOnBack([this]() {
        if (_loginMenu)
            _loginMenu->Hide();
        if (_mainMenu)
            _mainMenu->Show();
        _loginOverlay = false;
    });
    _loginMenu->Initialize();
    _loginMenu->Hide();

    // ===== Connection menu =====
    _connectionMenu = std::make_unique<Game::ConnectionMenu>(*_uiFactory, _graphics);
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

void Rendering::StartGame() {
    if (!_initialized)
        return;

    LOG_INFO("Rendering: Force switching to Game Scene");
    _scene = Scene::IN_GAME;

    // Hide all menus
    if (_mainMenu)
        _mainMenu->Hide();
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
        Shutdown();
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
    if (_confirmQuitMenu && _confirmQuitMenu->IsVisible()) {
        _confirmQuitMenu->Update();
        return;
    }

    if (_scene == Scene::MENU) {
        if (_mainMenu && _mainMenu->IsVisible()) {
            _mainMenu->Update();
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
            if (_loginMenu->IsLoginSubmitted() || _loginMenu->IsRegisterSubmitted() ||
                _loginMenu->IsGuestSubmitted()) {
                std::string username;
                if (_loginMenu->IsGuestSubmitted()) {
                    username = "Guest";
                } else {
                    username = _loginMenu->GetUsername();
                }

                // Update MainMenu profile button
                if (_mainMenu) {
                    _mainMenu->SetProfileName(username);
                }

                LOG_INFO("[Rendering] User logged in as: " + username);

                // Close menu
                _loginMenu->Hide();
                if (_mainMenu)
                    _mainMenu->Show();
                _loginOverlay = false;
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
                             int health) {
    if (_entityRenderer) {
        _entityRenderer->updateEntity(id, type, x, y, health);
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

void Rendering::SetPing(uint32_t pingMs) {
    _currentPing = pingMs;
}
