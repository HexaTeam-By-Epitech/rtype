/*
** EPITECH PROJECT, 2025
** workspace
** File description:
** Raylib
*/

#pragma once

#include <raylib.h>
#include <vector>
#include "../Interfaces/IGraphics.hpp"

namespace Graphics {
    class Raylib : public IGraphics {
       public:
        // Constructor / Destructor
        Raylib();
        ~Raylib() override;

        // Window management
        void InitWindow(int width, int height, const char *title) override;
        void ClearWindow() override;
        void DisplayWindow() override;
        bool IsWindowOpen() const override;
        void CloseWindow() override;
        void SetWindowTitle(const char *title) override;
        void SetWindowSize(int width, int height) override;
        void ToggleFullScreen() override;
        void SetTargetFPS(int fps) override;
        void SetClearColor(unsigned int color) override;
        void TakeScreenshot(const char *filepath) override;

        // Time / profiling
        float GetTime() const override;
        float GetDeltaTime() const override;

        // Basic drawing primitives
        void DrawRect(int x, int y, int width, int height, unsigned int color) override;
        void DrawRectFilled(int x, int y, int width, int height, unsigned int color) override;
        void DrawCircle(int x, int y, int radius, unsigned int color) override;
        void DrawCircleFilled(int x, int y, int radius, unsigned int color) override;

        // Fonts / text
        int LoadFont(const char *filepath, int size) override;
        void UnloadFont(int fontHandle) override;
        void DrawText(int fontHandle, const char *text, int x, int y, int fontSize,
                      unsigned int color) override;
        int GetFontHeight(int fontHandle, int fontSize) override;

        // Textures / sprites / images
        int LoadTexture(const char *filepath) override;
        int CreateTextureFromMemory(const void *pixels, int width, int height, int format) override;
        void UpdateTexture(int textureHandle, const void *pixels) override;
        void UnloadTexture(int textureHandle) override;
        void DrawTexture(int textureHandle, int x, int y, unsigned int tint) override;
        void DrawTextureEx(int textureHandle, int srcX, int srcY, int srcW, int srcH, float destX,
                           float destY, float rotation, float scale, unsigned int tint) override;

        // Input helpers
        bool IsKeyPressed(int key) const override;
        bool IsKeyDown(int key) const override;
        bool IsKeyReleased(int key) const override;
        bool IsMouseButtonPressed(int button) const override;
        bool IsMouseButtonDown(int button) const override;
        void GetMousePosition(float &x, float &y) const override;

       private:
        std::vector<Font> _fonts;
        std::vector<Texture2D> _textures;
    };
}  // namespace Graphics
