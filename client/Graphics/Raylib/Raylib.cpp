/*
** EPITECH PROJECT, 2025
** workspace
** File description:
** Raylib
*/

#include "Raylib.hpp"
#include <limits>

namespace Graphics {
    // Constructor / Destructor
    Raylib::Raylib() {}

    Raylib::~Raylib() {
        for (auto &font : _fonts) {
            ::UnloadFont(font);
        }
        for (auto &texture : _textures) {
            ::UnloadTexture(texture);
        }
        ::CloseWindow();
    }

    // Window management
    void Raylib::InitWindow(int width, int height, const char *title) {
        ::InitWindow(width, height, title);
    }

    void Raylib::ClearWindow() {
        ::ClearBackground(_clearColor);
    }

    void Raylib::StartDrawing() {
        ::BeginDrawing();
    }

    void Raylib::DisplayWindow() {
        ::EndDrawing();
    }

    bool Raylib::IsWindowOpen() const {
        return !::WindowShouldClose();
    }

    void Raylib::CloseWindow() {
        ::CloseWindow();
    }

    void Raylib::SetWindowTitle(const char *title) {
        ::SetWindowTitle(title);
    }

    void Raylib::SetWindowSize(int width, int height) {
        ::SetWindowSize(width, height);
    }

    void Raylib::ToggleFullScreen() {
        ::ToggleFullscreen();
    }

    void Raylib::SetTargetFPS(int fps) {
        ::SetTargetFPS(fps);
    }

    void Raylib::SetClearColor(unsigned int color) {
        _clearColor.a = (color >> 24) & 0xFF;
        _clearColor.r = (color >> 16) & 0xFF;
        _clearColor.g = (color >> 8) & 0xFF;
        _clearColor.b = color & 0xFF;
    }

    void Raylib::TakeScreenshot(const char *filepath) {
        ::TakeScreenshot(filepath);
    }

    // Time / profiling
    float Raylib::GetTime() const {
        return ::GetTime();
    }

    float Raylib::GetDeltaTime() const {
        return ::GetFrameTime();
    }

    // Basic drawing primitives
    void Raylib::DrawRect(int x, int y, int width, int height, unsigned int color) {
        Color clr;
        clr.a = (color >> 24) & 0xFF;
        clr.r = (color >> 16) & 0xFF;
        clr.g = (color >> 8) & 0xFF;
        clr.b = color & 0xFF;
        ::DrawRectangleLines(x, y, width, height, clr);
    }

    void Raylib::DrawRectFilled(int x, int y, int width, int height, unsigned int color) {
        Color clr;
        clr.a = (color >> 24) & 0xFF;
        clr.r = (color >> 16) & 0xFF;
        clr.g = (color >> 8) & 0xFF;
        clr.b = color & 0xFF;
        ::DrawRectangle(x, y, width, height, clr);
    }

    void Raylib::DrawCircle(int x, int y, int radius, unsigned int color) {
        Color clr;
        clr.a = (color >> 24) & 0xFF;
        clr.r = (color >> 16) & 0xFF;
        clr.g = (color >> 8) & 0xFF;
        clr.b = color & 0xFF;
        ::DrawCircleLines(x, y, radius, clr);
    }

    void Raylib::DrawCircleFilled(int x, int y, int radius, unsigned int color) {
        Color clr;
        clr.a = (color >> 24) & 0xFF;
        clr.r = (color >> 16) & 0xFF;
        clr.g = (color >> 8) & 0xFF;
        clr.b = color & 0xFF;
        ::DrawCircle(x, y, radius, clr);
    }
    // Fonts / text
    int Raylib::LoadFont(const char *filepath, int size) {
        Font font = ::LoadFontEx(filepath, size, 0, 0);
        if (font.texture.id == 0)
            return -1;
        _fonts.push_back(font);
        return _fonts.size() - 1;
    }

    void Raylib::UnloadFont(int fontHandle) {
        if (fontHandle >= 0 && static_cast<size_t>(fontHandle) < _fonts.size()) {
            ::UnloadFont(_fonts[fontHandle]);
        }
    }

    void Raylib::DrawText(int fontHandle, const char *text, int x, int y, int fontSize, unsigned int color) {
        Color clr;
        clr.a = (color >> 24) & 0xFF;
        clr.r = (color >> 16) & 0xFF;
        clr.g = (color >> 8) & 0xFF;
        clr.b = color & 0xFF;

        if (fontHandle >= 0 && static_cast<size_t>(fontHandle) < _fonts.size()) {
            ::DrawTextEx(_fonts[fontHandle], text, {(float)x, (float)y}, fontSize, 1.0f, clr);
            return;
        } else
            ::DrawText(text, x, y, fontSize, clr);
    }

    int Raylib::GetFontHeight(int fontHandle, int fontSize) {
        if (fontHandle >= 0 && static_cast<size_t>(fontHandle) < _fonts.size()) {
            return _fonts[fontHandle].baseSize;
        }
        return fontSize;
    }

    // Textures / sprites / images
    int Raylib::LoadTexture(const char *filepath) {
        Texture2D texture = ::LoadTexture(filepath);
        if (texture.id == 0)
            return -1;
        _textures.push_back(texture);
        return _textures.size() - 1;
    }

    int Raylib::CreateTextureFromMemory(const void *pixels, int width, int height, int format) {
        Image img;
        img.data = const_cast<void *>(pixels);
        img.width = width;
        img.height = height;
        img.format = format;
        img.mipmaps = 1;
        Texture2D texture = ::LoadTextureFromImage(img);
        _textures.push_back(texture);
        return _textures.size() - 1;
    }

    void Raylib::UpdateTexture(int textureHandle, const void *pixels) {
        if (textureHandle >= 0 && static_cast<size_t>(textureHandle) < _textures.size()) {
            ::UpdateTexture(_textures[textureHandle], pixels);
        }
    }

    void Raylib::UnloadTexture(int textureHandle) {
        if (textureHandle >= 0 && static_cast<size_t>(textureHandle) < _textures.size()) {
            ::UnloadTexture(_textures[textureHandle]);
        }
    }

    void Raylib::DrawTexture(int textureHandle, int x, int y, unsigned int tint) {
        if (textureHandle >= 0 && static_cast<size_t>(textureHandle) < _textures.size()) {
            Color clr;
            clr.a = (tint >> 24) & 0xFF;
            clr.r = (tint >> 16) & 0xFF;
            clr.g = (tint >> 8) & 0xFF;
            clr.b = tint & 0xFF;
            ::DrawTexture(_textures[textureHandle], x, y, clr);
        }
    }

    void Raylib::DrawTextureEx(int textureHandle, int srcX, int srcY, int srcW, int srcH, float destX,
                               float destY, float rotation, float scale, unsigned int tint) {
        if (textureHandle >= 0 && static_cast<size_t>(textureHandle) < _textures.size()) {
            Color clr;
            clr.a = (tint >> 24) & 0xFF;
            clr.r = (tint >> 16) & 0xFF;
            clr.g = (tint >> 8) & 0xFF;
            clr.b = tint & 0xFF;

            Rectangle source = {(float)srcX, (float)srcY, (float)srcW, (float)srcH};
            Rectangle dest = {destX, destY, srcW * scale, srcH * scale};
            Vector2 origin = {0, 0};

            ::DrawTexturePro(_textures[textureHandle], source, dest, origin, rotation, clr);
        }
    }

    // Input helpers
    bool Raylib::IsKeyPressed(int key) const {
        return ::IsKeyPressed(key);
    }

    bool Raylib::IsKeyDown(int key) const {
        return ::IsKeyDown(key);
    }

    bool Raylib::IsKeyReleased(int key) const {
        return ::IsKeyReleased(key);
    }

    bool Raylib::IsMouseButtonPressed(int button) const {
        return ::IsMouseButtonPressed(button);
    }

    bool Raylib::IsMouseButtonDown(int button) const {
        return ::IsMouseButtonDown(button);
    }

    void Raylib::GetMousePosition(float &x, float &y) const {
        Vector2 pos = ::GetMousePosition();
        x = pos.x;
        y = pos.y;
    }
}  // namespace Graphics
