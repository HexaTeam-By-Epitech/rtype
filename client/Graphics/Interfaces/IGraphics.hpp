/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** IGraphics
*/

#pragma once

class IGraphics {
   public:
    // Destructor
    virtual ~IGraphics() = default;

    // Window management
    virtual void InitWindow(int width, int height, const char *title) = 0;
    virtual void ClearWindow() = 0;
    virtual void DisplayWindow() = 0;
    virtual bool IsWindowOpen() const = 0;
    virtual void CloseWindow() = 0;

    // Window / display helpers
    virtual void SetWindowTitle(const char *title) = 0;
    virtual void SetWindowSize(int width, int height) = 0;
    virtual void ToggleFullScreen() = 0;
    virtual void SetTargetFPS(int fps) = 0;
    virtual void SetClearColor(unsigned int color) = 0;  // 0xAARRGGBB
    virtual void TakeScreenshot(const char *filepath) = 0;

    // Time / profiling
    virtual float GetTime() const = 0;
    virtual float GetDeltaTime() const = 0;

    // Basic drawing primitives
    virtual void DrawRect(int x, int y, int width, int height, unsigned int color) = 0;
    virtual void DrawRectFilled(int x, int y, int width, int height, unsigned int color) = 0;
    virtual void DrawCircle(int x, int y, int radius, unsigned int color) = 0;
    virtual void DrawCircleFilled(int x, int y, int radius, unsigned int color) = 0;

    // Text / fonts
    virtual int LoadFont(const char *filepath, int size) = 0;  // returns font handle
    virtual void UnloadFont(int fontHandle) = 0;
    virtual void DrawText(int fontHandle, const char *text, int x, int y, int fontSize,
                          unsigned int color) = 0;
    virtual int GetFontHeight(int fontHandle, int fontSize) = 0;

    // Textures / sprites / images
    virtual int LoadTexture(const char *filepath) = 0;  // returns texture handle
    virtual int CreateTextureFromMemory(const void *pixels, int width, int height, int format) = 0;
    virtual void UpdateTexture(int textureHandle, const void *pixels) = 0;
    virtual void UnloadTexture(int textureHandle) = 0;
    virtual void DrawTexture(int textureHandle, int x, int y, unsigned int tint) = 0;
    virtual void DrawTextureEx(int textureHandle, int srcX, int srcY, int srcW, int srcH, float destX,
                               float destY, float rotation, float scale, unsigned int tint) = 0;

    // Input helpers (often forwarded from windowing layer)
    virtual bool IsKeyPressed(int key) const = 0;
    virtual bool IsKeyDown(int key) const = 0;
    virtual bool IsKeyReleased(int key) const = 0;
    virtual bool IsMouseButtonPressed(int button) const = 0;
    virtual bool IsMouseButtonDown(int button) const = 0;
    virtual void GetMousePosition(float &x, float &y) const = 0;
};
