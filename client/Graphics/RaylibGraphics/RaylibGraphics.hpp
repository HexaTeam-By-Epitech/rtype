/*
** EPITECH PROJECT, 2025
** workspace
** File description:
** Raylib
*/

#pragma once

#include <raylib.h>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>
#include "IGraphics.hpp"

namespace Graphics {
    /**
     * @brief Raylib implementation of the IGraphics interface
     */
    class RaylibGraphics : public IGraphics {
       public:
        // Constructor / Destructor
        /**
         * @brief Construct a new Raylib graphics object
         */
        RaylibGraphics();
        /**
         * @brief Destroy the Raylib graphics object and cleanup resources
         */
        ~RaylibGraphics() override;

        // Window management
        void InitWindow(int width, int height, const char *title) override;
        void ClearWindow() override;
        void StartDrawing() override;
        void DisplayWindow() override;
        bool IsWindowOpen() const override;
        void CloseWindow() override;
        [[nodiscard]] int GetWindowWidth() const override;
        [[nodiscard]] int GetWindowHeight() const override;
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
        int LoadTexture(const char *name, const char *filepath) override;
        int CreateTextureFromMemory(char const *textureName, const void *pixels, int width, int height,
                                    int format) override;
        void UpdateTexture(const char *textureName, const void *pixels) override;
        void UnloadTexture(const char *textureName) override;
        void DrawTexture(const char *textureName, int x, int y, unsigned int tint) override;
        void DrawTextureEx(const char *textureName, int srcX, int srcY, int srcW, int srcH, float destX,
                           float destY, float rotation, float scale, unsigned int tint) override;

        // Input helpers
        bool IsKeyPressed(int key) const override;
        bool IsKeyDown(int key) const override;
        bool IsKeyReleased(int key) const override;
        bool IsMouseButtonPressed(int button) const override;
        bool IsMouseButtonDown(int button) const override;
        void GetMousePosition(float &x, float &y) const override;
        bool WindowShouldClose() const override;
        int GetMouseX() const override;
        int GetMouseY() const override;
        int GetCharPressed() const override;
        int GetScreenWidth() const override;
        int GetScreenHeight() const override;
        void DrawRectangle(int x, int y, int width, int height, unsigned int color) override;
        void DrawRectangleLines(int x, int y, int width, int height, unsigned int color) override;
        void DrawText(const char *text, int x, int y, int fontSize, unsigned int color) override;

        // Colorblind filter
        void SetColorblindFilter(ColorblindFilterType filter) override;
        [[nodiscard]] ColorblindFilterType GetColorblindFilter() const override;
        void BeginColorblindCapture() override;
        void EndColorblindCapture() override;

       private:
        void LoadColorblindShader();
        void UnloadColorblindShader();

        std::vector<Font> _fonts;
        std::unordered_map<std::string, Texture2D> _textures;
        Color _clearColor{255, 255, 255, 255};
        bool _windowInitialized = false;

        // Colorblind filter resources
        Shader _colorblindShader{};
        RenderTexture2D _colorblindRenderTexture{};
        ColorblindFilterType _colorblindFilter{ColorblindFilterType::NONE};
        bool _colorblindShaderLoaded{false};
        int _filterTypeLoc{-1};
    };
}  // namespace Graphics
