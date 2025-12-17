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

    _graphics.DisplayWindow();
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
