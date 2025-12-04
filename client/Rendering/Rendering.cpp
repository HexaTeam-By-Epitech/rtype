/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** Rendering.cpp
*/

#include "Rendering.hpp"

Rendering::Rendering(EventBus &eventBus) : _eventBus(eventBus) {}

Rendering::~Rendering() {
    shutdown();
}

bool Rendering::initialize(uint32_t width, uint32_t height, const std::string &title) {
    (void)width;
    (void)height;
    (void)title;

    return true;
}

void Rendering::shutdown() {
    _initialized = false;
}

void Rendering::render() {}

bool Rendering::isWindowOpen() const {
    return _initialized;
}

bool Rendering::loadTexture(const std::string &id, const std::string &path) {
    (void)id;
    (void)path;

    return true;
}

void Rendering::drawSprite(const std::string &textureId, float x, float y, float rotation, float scale) {
    (void)textureId;
    (void)x;
    (void)y;
    (void)rotation;
    (void)scale;
}

void Rendering::drawText(const std::string &text, float x, float y, uint32_t size) {
    (void)text;
    (void)x;
    (void)y;
    (void)size;
}

uint32_t Rendering::getWidth() const {
    return _width;
}

uint32_t Rendering::getHeight() const {
    return _height;
}