/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** Rendering.cpp
*/

#include "Rendering.hpp"

Rendering::Rendering(EventBus &eventBus) : _eventBus(eventBus) {}

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

    // Initialize EntityRenderer subsystem
    _entityRenderer = std::make_unique<EntityRenderer>(_graphics);
    LOG_DEBUG("EntityRenderer subsystem initialized");

    _initialized = true;
    LOG_INFO("Rendering initialized: ", width, "x", height, " (", title, ")");
    return true;
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

    _graphics.ClearWindow();
}

void Rendering::Render() {
    if (!_initialized) {
        return;
    }

    _graphics.StartDrawing();

    // Render entities (if EntityRenderer is initialized)
    if (_entityRenderer) {
        _entityRenderer->render();
    }

    // Render ping display in top-right corner (always visible when initialized)
    // Determine color based on ping quality
    uint32_t color;
    std::string pingText;

    if (_displayedPing == 0) {
        // No ping data yet or disconnected
        color = 0xB3B3B3AA;  // Gray
    } else if (_displayedPing <= 50) {
        color = 0x9DFF73AA;  // Green (excellent)
    } else if (_displayedPing <= 100) {
        color = 0xFDFF75AA;  // Yellow (good)
    } else if (_displayedPing <= 150) {
        color = 0xFFB057AA;  // Orange (fair)
    } else {
        color = 0xF05454AA;  // Red (poor)
    }
    pingText = "Ping: " + std::to_string(_displayedPing) + "ms";

    // Position in top-right corner (with padding)
    const int textWidth = static_cast<int>(pingText.length() * 10);  // Approximate width
    const int xPos = static_cast<int>(_width) - textWidth - 20;
    constexpr int yPos = 10;

    // Draw with colored text
    _graphics.DrawText(-1, pingText.c_str(), xPos, yPos, 20, color);

    _graphics.DisplayWindow();

    // Check if user wants to close the window and stop the game loop
    if (_graphics.WindowShouldClose()) {
        // Trigger game loop shutdown
        // This will be detected by the GameLoop and initiate clean shutdown
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
