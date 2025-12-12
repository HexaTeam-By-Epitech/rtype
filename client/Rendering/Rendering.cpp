/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** Rendering.cpp
*/

#include "Rendering.hpp"

Rendering::Rendering(EventBus &eventBus) : _eventBus(eventBus) {
    _graphics = Graphics::RaylibGraphics();
}

Rendering::~Rendering() {
    Shutdown();
    _graphics.CloseWindow();
    _graphics.~RaylibGraphics();
}

bool Rendering::Initialize(uint32_t width, uint32_t height, const std::string &title) {
    _initialized = true;
    _eventBus = {};
    _graphics.InitWindow(static_cast<int>(width), static_cast<int>(height), title.c_str());
    _width = width;
    _height = height;

    return true;
}

void Rendering::Shutdown() {
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
}

bool Rendering::IsWindowOpen() const {
    return _initialized;
}

bool Rendering::LoadTexture(const std::string &textureName, const std::string &path) {
    _graphics.LoadTexture(textureName.c_str(), path.c_str());
    return true;
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
