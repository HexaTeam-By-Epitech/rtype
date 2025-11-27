/*
** EPITECH PROJECT, 2025
** workspace
** File description:
** Raylib
*/

#pragma once

#include <raylib.h>
#include "IGraphics.hpp"

namespace Graphics {
    class Raylib : public IGraphics {
       public:
        // Constructor / Destructor
        Raylib();
        ~Raylib() override;

        // Window management
        void initWindow(int width, int height, const char *title) override;
        void clearWindow() override;
        void displayWindow() override;
        bool isWindowOpen() const override;
        void closeWindow() override;
        void setWindowTitle(const char *title) override;
        void setWindowSize(int width, int height) override;
        void setFullscreen(bool enabled) override;
        void setTargetFPS(int fps) override;
        void setClearColor(unsigned int color) override;
        void takeScreenshot(const char *filepath) override;

        // Time / profiling
        float getTime() const override;
        float getDeltaTime() const override;

        // Basic drawing primitives
        void drawRect(int x, int y, int width, int height, unsigned int color) override;
        void drawRectFilled(int x, int y, int width, int height, unsigned int color) override;
        void drawCircle(int x, int y, int radius, unsigned int color) override;
        void drawCircleFilled(int x, int y, int radius, unsigned int color) override;

        // Fonts / text
        int loadFont(const char *filepath, int size) override;
        void unloadFont(int fontHandle) override;
        void drawText(int fontHandle, const char *text, int x, int y, int fontSize,
                      unsigned int color) override;
        int getFontHeight(int fontHandle, int fontSize) override;

        // Textures / sprites / images
        int loadTexture(const char *filepath) override;
        int createTextureFromMemory(const void *pixels, int width, int height, int format) override;
        void updateTexture(int textureHandle, const void *pixels) override;
        void unloadTexture(int textureHandle) override;
        void drawTexture(int textureHandle, int x, int y, unsigned int tint) override;
        void drawTextureEx(int textureHandle, int srcX, int srcY, int srcW, int srcH, float destX,
                           float destY, float rotation, float scale, unsigned int tint) override;

        // Input helpers
        bool isKeyPressed(int key) const override;
        bool isKeyDown(int key) const override;
        bool isKeyReleased(int key) const override;
    };
}  // namespace Graphics
