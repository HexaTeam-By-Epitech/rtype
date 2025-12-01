/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** Rendering.hpp
*/

#ifndef RENDERING_HPP
#define RENDERING_HPP

#include <cstdint>
#include <string>
#include "../Core/EventBus/EventBus.hpp"

/**
 * @class Rendering
 * @brief Graphical rendering system using Raylib
 * 
 * Rendering handles the display of all visual game elements:
 * - Game window
 * - Sprites (ships, enemies, projectiles)
 * - Text (UI, score, FPS)
 * - Visual effects
 * 
 * Architecture:
 * - Uses Raylib for 2D rendering
 * - Caches textures for optimization
 * - Handles double-buffering automatically
 * - Integrates with EventBus for visual events
 * 
 */
class Rendering {
   public:
    /**
     * @brief Constructor with EventBus reference
     * 
     * @param eventBus Event bus for inter-component communication
     * 
     * @note Does not create the window, call initialize() after
     */
    Rendering(EventBus &eventBus);

    /**
     * @brief Destructor
     * 
     * Frees Raylib wrapper resources (textures, window, etc.)
     */
    ~Rendering();

    /**
     * @brief Initialize the rendering system and create window
     * 
     * Creates a window through Raylib wrapper with specified parameters.
     * Loads basic resources (fonts, shaders).
     * 
     * @param width Window width in pixels
     * @param height Window height in pixels
     * @param title Window title
     * @return true if initialization succeeds, false otherwise
     * 
     * @note Must be called before any other rendering call
     * @note Recommended: 1920x1080 for R-Type
     * @note Depends on Raylib wrapper implementation (not yet available)
     */
    bool initialize(uint32_t width, uint32_t height, const std::string &title);

    /**
     * @brief Stop the rendering system and destroy window
     * 
     * Frees all allocated graphical resources.
     * Closes the window properly.
     */
    void shutdown();

    /**
     * @brief Perform rendering of current frame
     * 
     * Clears buffer, draws all queued elements, and displays.
     * Must be called once per frame.
     * 
     * @note Uses Raylib wrapper's rendering pipeline
     * @note Synchronized with VSync if enabled
     * @note Implementation pending Raylib wrapper availability
     */
    void render();

    /**
     * @brief Check if window is open
     * 
     * @return true if window exists and hasn't been closed
     * 
     * @note Returns false if user closes the window
     */
    bool isWindowOpen() const;

    /**
     * @brief Load a texture from file
     * 
     * Loads a texture into memory and associates it with an identifier.
     * Texture is cached for reuse.
     * 
     * @param id Unique identifier for this texture (e.g., "player_ship")
     * @param path Path to image file (PNG, JPG, BMP)
     * @return true if loading succeeds, false otherwise
     * 
     * @note Textures are shared (same texture = same memory)
     * @note Supported formats depend on Raylib wrapper implementation
     * @note Wrapper implementation pending
     */
    bool loadTexture(const std::string &id, const std::string &path);

    /**
     * @brief Draw a sprite on screen
     * 
     * Adds a sprite to the render queue for current frame.
     * 
     * @param textureId Texture identifier (loaded with loadTexture)
     * @param x X position on screen (0 = left)
     * @param y Y position on screen (0 = top)
     * @param rotation Rotation in degrees (0-360)
     * @param scale Scale (1.0 = original size, 2.0 = double)
     * 
     * @note Sprite is centered on (x, y)
     * @note Call order determines render order (Z-order)
     * @note Uses Raylib wrapper's sprite rendering (pending implementation)
     */
    void drawSprite(const std::string &textureId, float x, float y, float rotation = 0.0f,
                    float scale = 1.0f);

    /**
     * @brief Draw text on screen
     * 
     * Displays text with default font.
     * 
     * @param text Text to display (UTF-8 supported)
     * @param x X position (origin at top-left of text)
     * @param y Y position (origin at top-left of text)
     * @param size Font size in pixels (default: 24)
     * 
     * @note Uses default font loaded at initialization
     * @note Rendered on top of sprites (UI)
     * @note Font rendering via Raylib wrapper (not yet implemented)
     */
    void drawText(const std::string &text, float x, float y, uint32_t size = 24);

    /**
     * @brief Get window width
     * 
     * @return Width in pixels
     */
    uint32_t getWidth() const;

    /**
     * @brief Get window height
     * 
     * @return Height in pixels
     */
    uint32_t getHeight() const;

   private:
    EventBus &_eventBus;
    bool _initialized = false;
    uint32_t _width = 0;
    uint32_t _height = 0;
};

#endif
