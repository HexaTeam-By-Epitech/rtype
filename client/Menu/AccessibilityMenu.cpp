/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** AccessibilityMenu - Accessibility features menu implementation
*/

#include "Menu/AccessibilityMenu.hpp"
#include <algorithm>
#include <cstring>
#include "../common/Logger/Logger.hpp"

namespace Game {
    AccessibilityMenu::AccessibilityMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics)
        : BaseMenu(uiFactory), _graphics(graphics) {}

    void AccessibilityMenu::Initialize() {
        if (!_menu) {
            return;
        }

        _menu->Clear();

        const float buttonWidth = 400.0f;
        const float buttonHeight = 50.0f;
        const float spacing = 18.0f;

        const bool showMainMenuButton = (_mode == Mode::OVERLAY);
        const int buttonCount = showMainMenuButton ? 4 : 3;

        // Center buttons vertically as a stack
        const float totalHeight = (buttonHeight * static_cast<float>(buttonCount)) +
                                  (spacing * static_cast<float>(buttonCount - 1));

        auto offsetForIndex = [&](int idx) {
            const float start = -(totalHeight / 2.0f) + (buttonHeight / 2.0f);
            return start + (buttonHeight + spacing) * static_cast<float>(idx);
        };

        // Colorblind Filter button (cycles through filter types)
        _menu->AddButton(CreateCenteredButton(
            "COLORBLIND: NONE", offsetForIndex(0), buttonWidth, buttonHeight, 0xFF424242, 0xFF616161,
            [this]() { SetColorblindFilter(NextColorblindFilter(_colorblindFilter)); }));

        // Key Bindings configuration button
        _menu->AddButton(CreateCenteredButton("CONFIGURE KEY BINDINGS", offsetForIndex(1), buttonWidth,
                                              buttonHeight, 0xFF5E35B1, 0xFF7E57C2,
                                              [this]() { OpenKeyBindingsConfig(); }));

        // Back button
        _menu->AddButton(CreateCenteredButton("BACK", offsetForIndex(2), buttonWidth, buttonHeight,
                                              0xFF1976D2, 0xFF1E88E5, [this]() {
                                                  if (_onBack) {
                                                      _onBack();
                                                  } else {
                                                      Hide();
                                                  }
                                              }));

        // Main menu (only in overlay mode)
        if (showMainMenuButton) {
            _menu->AddButton(CreateCenteredButton("MAIN MENU", offsetForIndex(3), buttonWidth, buttonHeight,
                                                  0xFF5D4037, 0xFF6D4C41, [this]() {
                                                      if (_onMainMenu) {
                                                          _onMainMenu();
                                                      }
                                                  }));
        }

        RefreshVisuals();
    }

    void AccessibilityMenu::Update() {
        BaseMenu::Update();
    }

    void AccessibilityMenu::Render() {
        if (!_menu || !_menu->IsVisible()) {
            return;
        }

        // Draw title
        int screenWidth = _graphics.GetScreenWidth();
        const char *title = "ACCESSIBILITY SETTINGS";
        int titleFontSize = 24;
        int titleWidth = static_cast<int>(strlen(title) * titleFontSize * 0.6f);
        int titleX = (screenWidth - titleWidth) / 2;
        int titleY = 80;
        _graphics.DrawText(title, titleX, titleY, titleFontSize, 0xFF4CAF50);

        // Draw dim overlay if in overlay mode
        if (_mode == Mode::OVERLAY) {
            // Background dim is handled by the caller
        }

        BaseMenu::Render();
    }

    // --- Display Mode ---
    void AccessibilityMenu::SetMode(Mode mode) {
        if (_mode != mode) {
            _mode = mode;
            Initialize();  // Rebuild menu to show/hide Main Menu button
        }
    }

    AccessibilityMenu::Mode AccessibilityMenu::GetMode() const {
        return _mode;
    }

    void AccessibilityMenu::SetOverlayDimColor(unsigned int color) {
        _overlayDimColor = color;
    }

    unsigned int AccessibilityMenu::GetOverlayDimColor() const {
        return _overlayDimColor;
    }

    bool AccessibilityMenu::ShouldDimBackground() const {
        return (_mode == Mode::OVERLAY) && IsVisible();
    }

    // --- Colorblind Filters ---
    void AccessibilityMenu::SetColorblindFilter(ColorblindFilter filter) {
        if (_colorblindFilter != filter) {
            _colorblindFilter = filter;
            UpdateColorblindFilterVisuals();
            LOG_INFO("[AccessibilityMenu] Colorblind filter changed: ", ColorblindFilterToString(filter));
            if (_onColorblindFilterChanged) {
                _onColorblindFilterChanged(filter);
            }
        }
    }

    AccessibilityMenu::ColorblindFilter AccessibilityMenu::GetColorblindFilter() const {
        return _colorblindFilter;
    }

    void AccessibilityMenu::SetOnColorblindFilterChanged(std::function<void(ColorblindFilter)> callback) {
        _onColorblindFilterChanged = std::move(callback);
    }

    void AccessibilityMenu::SetColorblindFilterSilent(ColorblindFilter filter) {
        _colorblindFilter = filter;
        UpdateColorblindFilterVisuals();
    }

    // --- Game Speed ---
    void AccessibilityMenu::SetGameSpeed(float speed) {
        float clampedSpeed = ClampGameSpeed(speed);
        if (_gameSpeed != clampedSpeed) {
            _gameSpeed = clampedSpeed;
            LOG_INFO("[AccessibilityMenu] Game speed set to: ", static_cast<int>(_gameSpeed * 100), "%");
            if (_onGameSpeedChanged) {
                _onGameSpeedChanged(_gameSpeed);
            }
        }
    }

    float AccessibilityMenu::GetGameSpeed() const {
        return _gameSpeed;
    }

    void AccessibilityMenu::SetOnGameSpeedChanged(std::function<void(float)> callback) {
        _onGameSpeedChanged = std::move(callback);
    }

    void AccessibilityMenu::SetGameSpeedSilent(float speed) {
        _gameSpeed = ClampGameSpeed(speed);
    }

    // --- Key Bindings ---
    void AccessibilityMenu::OpenKeyBindingsConfig() {
        LOG_INFO("[AccessibilityMenu] Opening key bindings configuration");
        if (_onConfigureKeyBindings) {
            _onConfigureKeyBindings();
        } else {
            LOG_INFO("[AccessibilityMenu] No key bindings configuration callback set");
            // TODO: Implement key bindings configuration dialog
        }
    }

    void AccessibilityMenu::SetOnConfigureKeyBindings(std::function<void()> callback) {
        _onConfigureKeyBindings = std::move(callback);
    }

    // --- Callbacks ---
    void AccessibilityMenu::SetOnBack(std::function<void()> callback) {
        _onBack = std::move(callback);
    }

    void AccessibilityMenu::SetOnMainMenu(std::function<void()> callback) {
        _onMainMenu = std::move(callback);
    }

    // --- Visual Updates ---
    void AccessibilityMenu::RefreshVisuals() {
        UpdateColorblindFilterVisuals();
    }

    void AccessibilityMenu::UpdateColorblindFilterVisuals() {
        if (!_menu) {
            return;
        }

        const char *filterStr = ColorblindFilterToString(_colorblindFilter);
        char buttonText[64];
        snprintf(buttonText, sizeof(buttonText), "COLORBLIND: %s", filterStr);

        if (_menu->GetButtonCount() > COLORBLIND_FILTER_INDEX) {
            auto button = _menu->GetButton(COLORBLIND_FILTER_INDEX);
            button->SetText(buttonText);
        }
    }

    // --- Helper Methods ---
    AccessibilityMenu::ColorblindFilter AccessibilityMenu::NextColorblindFilter(
        ColorblindFilter current) const {
        switch (current) {
            case ColorblindFilter::NONE:
                return ColorblindFilter::PROTANOPIA;
            case ColorblindFilter::PROTANOPIA:
                return ColorblindFilter::DEUTERANOPIA;
            case ColorblindFilter::DEUTERANOPIA:
                return ColorblindFilter::TRITANOPIA;
            case ColorblindFilter::TRITANOPIA:
                return ColorblindFilter::MONOCHROMACY;
            case ColorblindFilter::MONOCHROMACY:
                return ColorblindFilter::NONE;
            default:
                return ColorblindFilter::NONE;
        }
    }

    const char *AccessibilityMenu::ColorblindFilterToString(ColorblindFilter filter) const {
        switch (filter) {
            case ColorblindFilter::NONE:
                return "NONE";
            case ColorblindFilter::PROTANOPIA:
                return "PROTANOPIA";
            case ColorblindFilter::DEUTERANOPIA:
                return "DEUTERANOPIA";
            case ColorblindFilter::TRITANOPIA:
                return "TRITANOPIA";
            case ColorblindFilter::MONOCHROMACY:
                return "MONOCHROMACY";
            default:
                return "UNKNOWN";
        }
    }

    float AccessibilityMenu::ClampGameSpeed(float speed) const {
        return std::max(0.25f, std::min(1.0f, speed));
    }
}  // namespace Game
