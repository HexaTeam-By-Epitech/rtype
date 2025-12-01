/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** IGraphics
*/

#pragma once

namespace Graphics {
    /**
 * @brief Abstract interface for graphics rendering operations
 * 
 * This interface provides a platform-agnostic API for window management,
 * rendering primitives, texture handling, font rendering, and input processing.
 * Implementations should provide concrete backends (e.g., Raylib, SDL, SFML).
 * 
 * @note All color values follow the format 0xAARRGGBB (Alpha, Red, Green, Blue)
 */
    class IGraphics {
       public:
        /**
     * @brief Virtual destructor
     */
        virtual ~IGraphics() = default;

        // ========== Window Management ==========

        /**
     * @brief Initialize the graphics window
     * @param width Width of the window in pixels
     * @param height Height of the window in pixels
     * @param title Title displayed on the window
     */
        virtual void InitWindow(int width, int height, const char *title) = 0;

        /**
     * @brief Clear the window with the current clear color
     */
        virtual void ClearWindow() = 0;

        /**
     * @brief Begin drawing frame (setup canvas for drawing operations)
     */
        virtual void StartDrawing() = 0;

        /**
     * @brief Display the current frame to the window (end drawing and swap buffers)
     */
        virtual void DisplayWindow() = 0;

        /**
     * @brief Check if the window is still open
     * @return true if the window should remain open, false if close was requested
     */
        virtual bool IsWindowOpen() const = 0;

        /**
     * @brief Close the graphics window and cleanup resources
     */
        virtual void CloseWindow() = 0;

        // ========== Window Configuration ==========

        /**
     * @brief Change the window title
     * @param title New title for the window
     */
        virtual void SetWindowTitle(const char *title) = 0;

        /**
     * @brief Resize the window
     * @param width New width in pixels
     * @param height New height in pixels
     */
        virtual void SetWindowSize(int width, int height) = 0;

        /**
     * @brief Toggle between fullscreen and windowed mode
     */
        virtual void ToggleFullScreen() = 0;

        /**
     * @brief Set the target frames per second for rendering
     * @param fps Target frame rate (e.g., 60 for 60 FPS)
     */
        virtual void SetTargetFPS(int fps) = 0;

        /**
     * @brief Set the background clear color
     * @param color Color in 0xAARRGGBB format (Alpha, Red, Green, Blue)
     */
        virtual void SetClearColor(unsigned int color) = 0;

        /**
     * @brief Capture the current screen and save it to a file
     * @param filepath Path where the screenshot will be saved
     */
        virtual void TakeScreenshot(const char *filepath) = 0;

        // ========== Time and Profiling ==========

        /**
     * @brief Get elapsed time since initialization
     * @return Time in seconds since graphics system was initialized
     */
        virtual float GetTime() const = 0;

        /**
     * @brief Get the time elapsed for the last frame
     * @return Delta time in seconds (time between current and previous frame)
     */
        virtual float GetDeltaTime() const = 0;

        // ========== Drawing Primitives ==========

        /**
     * @brief Draw a rectangle outline
     * @param x X coordinate of the top-left corner
     * @param y Y coordinate of the top-left corner
     * @param width Width of the rectangle
     * @param height Height of the rectangle
     * @param color Color in 0xAARRGGBB format
     */
        virtual void DrawRect(int x, int y, int width, int height, unsigned int color) = 0;

        /**
     * @brief Draw a filled rectangle
     * @param x X coordinate of the top-left corner
     * @param y Y coordinate of the top-left corner
     * @param width Width of the rectangle
     * @param height Height of the rectangle
     * @param color Color in 0xAARRGGBB format
     */
        virtual void DrawRectFilled(int x, int y, int width, int height, unsigned int color) = 0;

        /**
     * @brief Draw a circle outline
     * @param x X coordinate of the circle center
     * @param y Y coordinate of the circle center
     * @param radius Radius of the circle in pixels
     * @param color Color in 0xAARRGGBB format
     */
        virtual void DrawCircle(int x, int y, int radius, unsigned int color) = 0;

        /**
     * @brief Draw a filled circle
     * @param x X coordinate of the circle center
     * @param y Y coordinate of the circle center
     * @param radius Radius of the circle in pixels
     * @param color Color in 0xAARRGGBB format
     */
        virtual void DrawCircleFilled(int x, int y, int radius, unsigned int color) = 0;

        // ========== Font and Text Rendering ==========

        /**
     * @brief Load a font from file
     * @param filepath Path to the font file (e.g., .ttf, .otf)
     * @param size Font size to load
     * @return Font handle (integer ID) for use in drawing operations
     */
        virtual int LoadFont(const char *filepath, int size) = 0;

        /**
     * @brief Unload a previously loaded font
     * @param fontHandle Font handle returned by LoadFont()
     */
        virtual void UnloadFont(int fontHandle) = 0;

        /**
     * @brief Draw text using a loaded font
     * @param fontHandle Font handle (or -1 for default font)
     * @param text Text string to render
     * @param x X coordinate for text position
     * @param y Y coordinate for text position
     * @param fontSize Size of the rendered text
     * @param color Text color in 0xAARRGGBB format
     */
        virtual void DrawText(int fontHandle, const char *text, int x, int y, int fontSize,
                              unsigned int color) = 0;

        /**
     * @brief Get the height of a font at a given size
     * @param fontHandle Font handle returned by LoadFont()
     * @param fontSize Size to query
     * @return Height of the font in pixels
     */
        virtual int GetFontHeight(int fontHandle, int fontSize) = 0;

        // ========== Texture and Image Management ==========

        /**
     * @brief Load a texture from an image file
     * @param filepath Path to the image file (e.g., .png, .jpg)
     * @return Texture handle (integer ID) for use in drawing operations
     */
        virtual int LoadTexture(const char *filepath) = 0;

        /**
     * @brief Create a texture from raw pixel data in memory
     * @param pixels Pointer to raw pixel data
     * @param width Width of the texture in pixels
     * @param height Height of the texture in pixels
     * @param format Pixel format (implementation-specific)
     * @return Texture handle (integer ID) for use in drawing operations
     */
        virtual int CreateTextureFromMemory(const void *pixels, int width, int height, int format) = 0;

        /**
     * @brief Update an existing texture with new pixel data
     * @param textureHandle Texture handle returned by LoadTexture() or CreateTextureFromMemory()
     * @param pixels Pointer to new pixel data
     */
        virtual void UpdateTexture(int textureHandle, const void *pixels) = 0;

        /**
     * @brief Unload a previously loaded texture
     * @param textureHandle Texture handle to unload
     */
        virtual void UnloadTexture(int textureHandle) = 0;

        /**
     * @brief Draw a texture at the specified position
     * @param textureHandle Texture handle to draw
     * @param x X coordinate for drawing position
     * @param y Y coordinate for drawing position
     * @param tint Tint color in 0xAARRGGBB format (0xFFFFFFFF for no tint)
     */
        virtual void DrawTexture(int textureHandle, int x, int y, unsigned int tint) = 0;

        /**
     * @brief Draw a texture with advanced parameters (rotation, scale, source rectangle)
     * @param textureHandle Texture handle to draw
     * @param srcX X coordinate of source rectangle in texture
     * @param srcY Y coordinate of source rectangle in texture
     * @param srcW Width of source rectangle
     * @param srcH Height of source rectangle
     * @param destX X coordinate of destination position
     * @param destY Y coordinate of destination position
     * @param rotation Rotation angle in degrees
     * @param scale Scale factor (1.0 = original size)
     * @param tint Tint color in 0xAARRGGBB format
     */
        virtual void DrawTextureEx(int textureHandle, int srcX, int srcY, int srcW, int srcH, float destX,
                                   float destY, float rotation, float scale, unsigned int tint) = 0;

        // ========== Input Handling ==========

        /**
     * @brief Check if a key was pressed (triggered once when key goes down)
     * @param key Key code to check (implementation-specific key constants)
     * @return true if the key was just pressed, false otherwise
     */
        virtual bool IsKeyPressed(int key) const = 0;

        /**
     * @brief Check if a key is currently being held down
     * @param key Key code to check (implementation-specific key constants)
     * @return true if the key is currently down, false otherwise
     */
        virtual bool IsKeyDown(int key) const = 0;

        /**
     * @brief Check if a key was released (triggered once when key goes up)
     * @param key Key code to check (implementation-specific key constants)
     * @return true if the key was just released, false otherwise
     */
        virtual bool IsKeyReleased(int key) const = 0;

        /**
     * @brief Check if a mouse button was pressed (triggered once when button goes down)
     * @param button Mouse button code (implementation-specific button constants)
     * @return true if the button was just pressed, false otherwise
     */
        virtual bool IsMouseButtonPressed(int button) const = 0;

        /**
     * @brief Check if a mouse button is currently being held down
     * @param button Mouse button code (implementation-specific button constants)
     * @return true if the button is currently down, false otherwise
     */
        virtual bool IsMouseButtonDown(int button) const = 0;

        /**
     * @brief Get the current mouse cursor position
     * @param x Reference to store the X coordinate
     * @param y Reference to store the Y coordinate
     */
        virtual void GetMousePosition(float &x, float &y) const = 0;
    };
}  // namespace Graphics
