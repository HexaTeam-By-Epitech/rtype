/*
** EPITECH PROJECT, 2025
** workspace
** File description:
** Raylib
*/

#include "Raylib.hpp"

namespace Graphics {
    // Constructor / Destructor
    Raylib::Raylib() {}

    Raylib::~Raylib() {}

    // Window management
    void Raylib::initWindow(int width, int height, const char *title) {}

    void Raylib::clearWindow() {}

    void Raylib::displayWindow() {}

    bool Raylib::isWindowOpen() const {}

    void Raylib::closeWindow() {}

    void Raylib::setWindowTitle(const char *title) {}

    void Raylib::setWindowSize(int width, int height) {}

    void Raylib::setFullscreen(bool enabled) {}

    void Raylib::setTargetFPS(int fps) {}

    void Raylib::setClearColor(unsigned int color) {}

    void Raylib::takeScreenshot(const char *filepath) {}

    // Time / profiling
    float Raylib::getTime() const {}

    float Raylib::getDeltaTime() const {}

    // Basic drawing primitives
    void Raylib::drawRect(int x, int y, int width, int height, unsigned int color) {}

    void Raylib::drawRectFilled(int x, int y, int width, int height, unsigned int color) {}

    void Raylib::drawCircle(int x, int y, int radius, unsigned int color) {}

    void Raylib::drawCircleFilled(int x, int y, int radius, unsigned int color) {}

    // Fonts / text
    int Raylib::loadFont(const char *filepath, int size) {}

    void Raylib::unloadFont(int fontHandle) {}

    void Raylib::drawText(int fontHandle, const char *text, int x, int y, int fontSize, unsigned int color) {}

    int Raylib::getFontHeight(int fontHandle, int fontSize) {}

    // Textures / sprites / images
    int Raylib::loadTexture(const char *filepath) {}

    int Raylib::createTextureFromMemory(const void *pixels, int width, int height, int format) {}

    void Raylib::updateTexture(int textureHandle, const void *pixels) {}

    void Raylib::unloadTexture(int textureHandle) {}

    void Raylib::drawTexture(int textureHandle, int x, int y, unsigned int tint) {}

    void Raylib::drawTextureEx(int textureHandle, int srcX, int srcY, int srcW, int srcH, float destX,
                               float destY, float rotation, float scale, unsigned int tint) {}

    // Input helpers
    bool Raylib::isKeyPressed(int key) const {}

    bool Raylib::isKeyDown(int key) const {}

    bool Raylib::isKeyReleased(int key) const {}
}  // namespace Graphics
