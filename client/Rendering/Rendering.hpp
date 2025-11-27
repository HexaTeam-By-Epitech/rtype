/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** Rendering.hpp
*/

#ifndef RENDERING_HPP
#define RENDERING_HPP

#include <string>
#include <cstdint>
#include "../Core/EventBus/EventBus.hpp"

class Rendering {
public:
    Rendering(EventBus& eventBus);
    ~Rendering();

    bool initialize(uint32_t width, uint32_t height, const std::string& title);
    void shutdown();
    void render();
    bool isWindowOpen() const;
    bool loadTexture(const std::string& id, const std::string& path);
    void drawSprite(const std::string& textureId, float x, float y, 
                    float rotation = 0.0f, float scale = 1.0f);
    void drawText(const std::string& text, float x, float y, uint32_t size = 24);
    uint32_t getWidth() const;
    uint32_t getHeight() const;

private:
    EventBus& _eventBus;
    bool _initialized = false;
    uint32_t _width = 0;
    uint32_t _height = 0;
};

#endif