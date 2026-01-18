/*
** EPITECH PROJECT, 2025
** workspace
** File description:
** Raylib
*/

#include "RaylibGraphics.hpp"

namespace Graphics {
    // Constructor / Destructor
    RaylibGraphics::RaylibGraphics() {}

    RaylibGraphics::~RaylibGraphics() {
        UnloadColorblindShader();
        for (const Font &font : _fonts) {
            ::UnloadFont(font);
        }
        for (const auto &[name, texture] : _textures) {
            ::UnloadTexture(texture);
        }
    }

    // Window management
    void RaylibGraphics::InitWindow(int width, int height, const char *title) {
        if (!::IsWindowReady()) {
            // Create new window
            ::InitWindow(width, height, title);
            // Disable default ESC exit behavior to allow custom window/menu handling
            ::SetExitKey(0);
        } else {
            // Window already exists (from login), reconfigure it
            ::SetWindowTitle(title);
            ::SetWindowSize(width, height);
            // Ensure this is set even if reused
            ::SetExitKey(0);
        }
        _windowInitialized = true;
    }

    void RaylibGraphics::ClearWindow() {
        ::ClearBackground(_clearColor);
    }

    void RaylibGraphics::StartDrawing() {
        ::BeginDrawing();
    }

    void RaylibGraphics::DisplayWindow() {
        ::EndDrawing();
    }

    bool RaylibGraphics::IsWindowOpen() const {
        return _windowInitialized && !::WindowShouldClose();
    }

    int RaylibGraphics::GetWindowWidth() const {
        return ::GetScreenWidth();
    }

    int RaylibGraphics::GetWindowHeight() const {
        return ::GetScreenHeight();
    }

    void RaylibGraphics::CloseWindow() {
        if (_windowInitialized && !::WindowShouldClose()) {
            ::CloseWindow();
            _windowInitialized = false;
        }
    }

    void RaylibGraphics::SetWindowTitle(const char *title) {
        ::SetWindowTitle(title);
    }

    void RaylibGraphics::SetWindowSize(int width, int height) {
        ::SetWindowSize(width, height);
    }

    void RaylibGraphics::ToggleFullScreen() {
        ::ToggleFullscreen();
    }

    void RaylibGraphics::SetTargetFPS(int fps) {
        ::SetTargetFPS(fps);
    }

    void RaylibGraphics::SetClearColor(unsigned int color) {
        _clearColor.a = (color >> 24) & 0xFF;
        _clearColor.r = (color >> 16) & 0xFF;
        _clearColor.g = (color >> 8) & 0xFF;
        _clearColor.b = color & 0xFF;
    }

    void RaylibGraphics::TakeScreenshot(const char *filepath) {
        ::TakeScreenshot(filepath);
    }

    // Time / profiling
    float RaylibGraphics::GetTime() const {
        return ::GetTime();
    }

    float RaylibGraphics::GetDeltaTime() const {
        return ::GetFrameTime();
    }

    // Basic drawing primitives
    void RaylibGraphics::DrawRect(int x, int y, int width, int height, unsigned int color) {
        Color clr;
        clr.a = (color >> 24) & 0xFF;
        clr.r = (color >> 16) & 0xFF;
        clr.g = (color >> 8) & 0xFF;
        clr.b = color & 0xFF;
        ::DrawRectangleLines(x, y, width, height, clr);
    }

    void RaylibGraphics::DrawRectFilled(int x, int y, int width, int height, unsigned int color) {
        Color clr;
        clr.a = (color >> 24) & 0xFF;
        clr.r = (color >> 16) & 0xFF;
        clr.g = (color >> 8) & 0xFF;
        clr.b = color & 0xFF;
        ::DrawRectangle(x, y, width, height, clr);
    }

    void RaylibGraphics::DrawCircle(int x, int y, int radius, unsigned int color) {
        Color clr;
        clr.a = (color >> 24) & 0xFF;
        clr.r = (color >> 16) & 0xFF;
        clr.g = (color >> 8) & 0xFF;
        clr.b = color & 0xFF;
        ::DrawCircleLines(x, y, radius, clr);
    }

    void RaylibGraphics::DrawCircleFilled(int x, int y, int radius, unsigned int color) {
        Color clr;
        clr.a = (color >> 24) & 0xFF;
        clr.r = (color >> 16) & 0xFF;
        clr.g = (color >> 8) & 0xFF;
        clr.b = color & 0xFF;
        ::DrawCircle(x, y, radius, clr);
    }
    // Fonts / text
    int RaylibGraphics::LoadFont(const char *filepath, int size) {
        Font font = ::LoadFontEx(filepath, size, 0, 0);
        if (font.texture.id == 0)
            return -1;
        _fonts.push_back(font);
        return _fonts.size() - 1;
    }

    void RaylibGraphics::UnloadFont(int fontHandle) {
        if (fontHandle >= 0 && static_cast<size_t>(fontHandle) < _fonts.size()) {
            ::UnloadFont(_fonts[fontHandle]);
        }
    }

    void RaylibGraphics::DrawText(int fontHandle, const char *text, int x, int y, int fontSize,
                                  unsigned int color) {
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

    int RaylibGraphics::GetFontHeight(int fontHandle, int fontSize) {
        if (fontHandle >= 0 && static_cast<size_t>(fontHandle) < _fonts.size()) {
            return _fonts[fontHandle].baseSize;
        }
        return fontSize;
    }

    // Textures / sprites / images
    int RaylibGraphics::LoadTexture(const char *textureName, const char *filepath) {
        Texture2D texture = ::LoadTexture(filepath);
        if (texture.id == 0)
            return -1;
        _textures[textureName] = texture;
        return 0;
    }

    int RaylibGraphics::CreateTextureFromMemory(const char *textureName, const void *pixels, int width,
                                                int height, int format) {
        Image img;
        img.data = const_cast<void *>(pixels);
        img.width = width;
        img.height = height;
        img.format = format;
        img.mipmaps = 1;
        Texture2D texture = ::LoadTextureFromImage(img);
        if (texture.id == 0)
            return -1;
        _textures[textureName] = texture;
        return 0;
    }

    void RaylibGraphics::UpdateTexture(const char *textureName, const void *pixels) {
        auto iter = _textures.find(textureName);
        if (iter != _textures.end()) {
            ::UpdateTexture(iter->second, pixels);
        }
    }

    void RaylibGraphics::UnloadTexture(const char *textureName) {
        auto iter = _textures.find(textureName);
        if (iter != _textures.end()) {
            ::UnloadTexture(iter->second);
            _textures.erase(iter);
        }
    }

    void RaylibGraphics::DrawTexture(const char *textureName, int xPos, int yPos, unsigned int tint) {
        auto iter = _textures.find(textureName);
        if (iter != _textures.end()) {
            Color clr;
            clr.a = (tint >> 24) & 0xFF;
            clr.r = (tint >> 16) & 0xFF;
            clr.g = (tint >> 8) & 0xFF;
            clr.b = tint & 0xFF;
            ::DrawTexture(iter->second, xPos, yPos, clr);
        }
    }

    void RaylibGraphics::DrawTextureEx(const char *textureName, int srcX, int srcY, int srcW, int srcH,
                                       float destX, float destY, float rotation, float scale,
                                       unsigned int tint) {
        auto iter = _textures.find(textureName);
        if (iter != _textures.end()) {
            Color clr;
            clr.a = (tint >> 24) & 0xFF;
            clr.r = (tint >> 16) & 0xFF;
            clr.g = (tint >> 8) & 0xFF;
            clr.b = tint & 0xFF;

            Rectangle source = {(float)srcX, (float)srcY, (float)srcW, (float)srcH};
            Rectangle dest = {destX, destY, (float)srcW * scale, (float)srcH * scale};
            Vector2 origin = {0, 0};

            ::DrawTexturePro(iter->second, source, dest, origin, rotation, clr);
        } else {
            // DEBUG: Texture not found - fallback to colored rectangle
            TraceLog(LOG_WARNING, "DrawTextureEx: Texture '%s' not found! Drawing fallback rectangle",
                     textureName);
            ::DrawRectangle((int)destX, (int)destY, (int)(srcW * scale), (int)(srcH * scale), BLUE);
        }
    }

    // Input helpers
    bool RaylibGraphics::IsKeyPressed(int key) const {
        return ::IsKeyPressed(key);
    }

    bool RaylibGraphics::IsKeyDown(int key) const {
        return ::IsKeyDown(key);
    }

    bool RaylibGraphics::IsKeyReleased(int key) const {
        return ::IsKeyReleased(key);
    }

    bool RaylibGraphics::IsMouseButtonPressed(int button) const {
        return ::IsMouseButtonPressed(button);
    }

    bool RaylibGraphics::IsMouseButtonDown(int button) const {
        return ::IsMouseButtonDown(button);
    }

    void RaylibGraphics::GetMousePosition(float &x, float &y) const {
        Vector2 pos = ::GetMousePosition();
        x = pos.x;
        y = pos.y;
    }

    bool RaylibGraphics::WindowShouldClose() const {
        if (!_windowInitialized) {
            return false;
        }
        return ::WindowShouldClose();
    }

    int RaylibGraphics::GetMouseX() const {
        return ::GetMouseX();
    }

    int RaylibGraphics::GetMouseY() const {
        return ::GetMouseY();
    }

    int RaylibGraphics::GetCharPressed() const {
        return ::GetCharPressed();
    }

    int RaylibGraphics::GetScreenWidth() const {
        return ::GetScreenWidth();
    }

    int RaylibGraphics::GetScreenHeight() const {
        return ::GetScreenHeight();
    }

    void RaylibGraphics::DrawRectangle(int x, int y, int width, int height, unsigned int color) {
        Color clr;
        clr.a = (color >> 24) & 0xFF;
        clr.r = (color >> 16) & 0xFF;
        clr.g = (color >> 8) & 0xFF;
        clr.b = color & 0xFF;
        ::DrawRectangle(x, y, width, height, clr);
    }

    void RaylibGraphics::DrawRectangleLines(int x, int y, int width, int height, unsigned int color) {
        Color clr;
        clr.a = (color >> 24) & 0xFF;
        clr.r = (color >> 16) & 0xFF;
        clr.g = (color >> 8) & 0xFF;
        clr.b = color & 0xFF;
        ::DrawRectangleLines(x, y, width, height, clr);
    }

    void RaylibGraphics::DrawText(const char *text, int x, int y, int fontSize, unsigned int color) {
        Color clr;
        clr.a = (color >> 24) & 0xFF;
        clr.r = (color >> 16) & 0xFF;
        clr.g = (color >> 8) & 0xFF;
        clr.b = color & 0xFF;
        ::DrawText(text, x, y, fontSize, clr);
    }

    // ========== Colorblind Filter Implementation ==========

    void RaylibGraphics::LoadColorblindShader() {
        if (_colorblindShaderLoaded) {
            return;
        }

        // Load shader from files
        _colorblindShader = ::LoadShader("assets/shaders/colorblind.vs", "assets/shaders/colorblind.fs");

        if (_colorblindShader.id == 0) {
            // Shader failed to load, fall back to no filtering
            return;
        }

        // Get uniform location for filter type
        _filterTypeLoc = ::GetShaderLocation(_colorblindShader, "filterType");

        // Create render texture for post-processing
        int width = ::GetScreenWidth();
        int height = ::GetScreenHeight();
        _colorblindRenderTexture = ::LoadRenderTexture(width, height);

        _colorblindShaderLoaded = true;
    }

    void RaylibGraphics::UnloadColorblindShader() {
        if (!_colorblindShaderLoaded) {
            return;
        }

        ::UnloadShader(_colorblindShader);
        ::UnloadRenderTexture(_colorblindRenderTexture);
        _colorblindShaderLoaded = false;
    }

    void RaylibGraphics::SetColorblindFilter(ColorblindFilterType filter) {
        _colorblindFilter = filter;

        // Load shader if needed and filter is not NONE
        if (filter != ColorblindFilterType::NONE && !_colorblindShaderLoaded) {
            LoadColorblindShader();
        }
    }

    ColorblindFilterType RaylibGraphics::GetColorblindFilter() const {
        return _colorblindFilter;
    }

    void RaylibGraphics::BeginColorblindCapture() {
        if (_colorblindFilter == ColorblindFilterType::NONE || !_colorblindShaderLoaded) {
            return;
        }

        // Check if render texture needs resizing
        int width = ::GetScreenWidth();
        int height = ::GetScreenHeight();
        if (_colorblindRenderTexture.texture.width != width ||
            _colorblindRenderTexture.texture.height != height) {
            ::UnloadRenderTexture(_colorblindRenderTexture);
            _colorblindRenderTexture = ::LoadRenderTexture(width, height);
        }

        // Begin rendering to texture
        ::BeginTextureMode(_colorblindRenderTexture);
        ::ClearBackground(_clearColor);
    }

    void RaylibGraphics::EndColorblindCapture() {
        if (_colorblindFilter == ColorblindFilterType::NONE || !_colorblindShaderLoaded) {
            return;
        }

        // End rendering to texture
        ::EndTextureMode();

        // Set filter type uniform
        int filterValue = static_cast<int>(_colorblindFilter);
        ::SetShaderValue(_colorblindShader, _filterTypeLoc, &filterValue, SHADER_UNIFORM_INT);

        // Draw the render texture with shader applied
        ::BeginShaderMode(_colorblindShader);

        // Draw flipped because render textures are flipped in OpenGL
        Rectangle sourceRec = {0.0f, 0.0f, static_cast<float>(_colorblindRenderTexture.texture.width),
                               static_cast<float>(-_colorblindRenderTexture.texture.height)};
        Rectangle destRec = {0.0f, 0.0f, static_cast<float>(::GetScreenWidth()),
                             static_cast<float>(::GetScreenHeight())};
        Vector2 origin = {0.0f, 0.0f};

        ::DrawTexturePro(_colorblindRenderTexture.texture, sourceRec, destRec, origin, 0.0f, WHITE);

        ::EndShaderMode();
    }
}  // namespace Graphics
