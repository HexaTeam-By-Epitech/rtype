/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** Rendering.cpp
*/

#include "Rendering.hpp"
#include "Graphics/RaylibGraphics/RaylibGraphics.hpp"

Rendering::Rendering(EventBus &eventBus)
    : _eventBus(eventBus), _graphicsInstance(new Graphics::RaylibGraphics()) {}

Rendering::~Rendering() {
    shutdown();
}

bool Rendering::initialize(uint32_t width, uint32_t height, const std::string &title) {
    _width = width;
    _height = height;
    _title = title;

    if (!_graphicsInstance) {
        _graphicsInstance = new Graphics::RaylibGraphics();
    }

    _graphicsInstance->InitWindow(static_cast<int>(_width), static_cast<int>(_height), _title.c_str());
    _graphicsInstance->SetTargetFPS(60);
    _initialized = true;

    return true;
}

void Rendering::shutdown() {
    if (!_graphicsInstance)
        return;

    // Close window and cleanup backend resources
    _graphicsInstance->CloseWindow();

    delete _graphicsInstance;
    _graphicsInstance = nullptr;
    _initialized = false;
    _loadedTextures.clear();
}

void Rendering::render() {
    if (!_initialized || !_graphicsInstance)
        return;

    _graphicsInstance->ClearWindow();

    // drawing logic goes here but :( no idea
    // wrote a queue void *, not sure about a "queue".
    // maybe a vector would be better

    // I'm mitigated delegating the rendering to here rather than the game loop

    for (RenderCommand rc : _toRender) {
        if (rc.type == RenderType::SPRITE) {
            auto it = _loadedTextures.find(rc.id);
            if (it != _loadedTextures.end()) {
                int handle = it->second;
                _graphicsInstance->DrawTextureEx(handle, 0, 0, 0, 0, rc.settings.x, rc.settings.y,
                                                 rc.settings.rotation, rc.settings.scale, 0xFFFFFFFFu);
            }
        } else if (rc.type == RenderType::TEXT) {
            _graphicsInstance->DrawText(-1, rc.id.c_str(), static_cast<int>(rc.settings.x),
                                        static_cast<int>(rc.settings.y), static_cast<int>(rc.settings.size),
                                        0xFFFFFFFFu);
        }
    }

    _graphicsInstance->DisplayWindow();
}

bool Rendering::isWindowOpen() const {
    if (!_graphicsInstance)
        return false;
    return _graphicsInstance->IsWindowOpen();
}

bool Rendering::loadTexture(const std::string &id, const std::string &path) {
    if (!_graphicsInstance)
        return false;

    int handle = _graphicsInstance->LoadTexture(path.c_str());
    if (handle == -1)
        return false;

    _loadedTextures[id] = handle;
    return true;
}

void Rendering::drawSprite(const std::string &textureId, float x, float y, float rotation, float scale) {
    if (!_graphicsInstance)
        return;

    auto it = _loadedTextures.find(textureId);
    if (it == _loadedTextures.end())
        return;

    int handle = it->second;

    _graphicsInstance->DrawTextureEx(handle, 0, 0, 0, 0, x, y, rotation, scale, 0xFFFFFFFFu);
    _toRender.push_back({RenderType::SPRITE, textureId, {x, y, rotation, scale, 0}});
}

void Rendering::drawText(const std::string &text, float x, float y, uint32_t size) {
    if (!_graphicsInstance)
        return;

    _graphicsInstance->DrawText(-1, text.c_str(), static_cast<int>(x), static_cast<int>(y),
                                static_cast<int>(size), 0xFFFFFFFFu);
    _toRender.push_back({RenderType::TEXT, text, {x, y, 0.0f, 1.0f, size}});
}

uint32_t Rendering::getWidth() const {
    return _width;
}

uint32_t Rendering::getHeight() const {
    return _height;
}