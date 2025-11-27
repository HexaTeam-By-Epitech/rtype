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
    virtual void initWindow(int width, int height, const char *title) = 0;
    virtual void clearWindow() = 0;
    virtual void displayWindow() = 0;
    virtual bool isWindowOpen() const = 0;
    virtual void closeWindow() = 0;

    // Window / display helpers
    virtual void setWindowTitle(const char *title) = 0;
    virtual void setWindowSize(int width, int height) = 0;
    virtual void setFullscreen(bool enabled) = 0;
    virtual void setTargetFPS(int fps) = 0;
    virtual void setClearColor(unsigned int color) = 0;  // 0xAARRGGBB
    virtual void takeScreenshot(const char *filepath) = 0;

    // Time / profiling
    virtual float getTime() const = 0;
    virtual float getDeltaTime() const = 0;

    // Basic drawing primitives
    virtual void drawRect(int x, int y, int width, int height, unsigned int color) = 0;
    virtual void drawRectFilled(int x, int y, int width, int height, unsigned int color) = 0;
    virtual void drawCircle(int x, int y, int radius, unsigned int color) = 0;
    virtual void drawCircleFilled(int x, int y, int radius, unsigned int color) = 0;

    // Text / fonts
    virtual int loadFont(const char *filepath, int size) = 0;  // returns font handle
    virtual void unloadFont(int fontHandle) = 0;
    virtual void drawText(int fontHandle, const char *text, int x, int y, int fontSize,
                          unsigned int color) = 0;
    virtual int getFontHeight(int fontHandle, int fontSize) = 0;

    // Textures / sprites / images
    virtual int loadTexture(const char *filepath) = 0;  // returns texture handle
    virtual int createTextureFromMemory(const void *pixels, int width, int height, int format) = 0;
    virtual void updateTexture(int textureHandle, const void *pixels) = 0;
    virtual void unloadTexture(int textureHandle) = 0;
    virtual void drawTexture(int textureHandle, int x, int y, unsigned int tint) = 0;
    virtual void drawTextureEx(int textureHandle, int srcX, int srcY, int srcW, int srcH, float destX,
                               float destY, float rotation, float scale, unsigned int tint) = 0;

    // Input helpers (often forwarded from windowing layer)
    virtual bool isKeyPressed(int key) const = 0;
    virtual bool isKeyDown(int key) const = 0;
    virtual bool isKeyReleased(int key) const = 0;
    virtual bool isMouseButtonPressed(int button) const = 0;
    virtual bool isMouseButtonDown(int button) const = 0;
    virtual void getMousePosition(float &x, float &y) const = 0;
};
