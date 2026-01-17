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
        : BaseMenu(uiFactory), _graphics(graphics) {
        _gameSpeedSlider = _uiFactory.CreateSlider();
    }

    void AccessibilityMenu::Initialize() {
        if (!_menu) {
            return;
        }

        _menu->Clear();

        const float buttonWidth = 400.0f;
        const float buttonHeight = 50.0f;
        const float spacing = 18.0f;

        const bool showMainMenuButton = (_mode == Mode::OVERLAY);
        const int buttonCount = showMainMenuButton ? 5 : 4;

        // Center buttons vertically as a stack
        const float totalHeight = (buttonHeight * static_cast<float>(buttonCount)) +
                                  (spacing * static_cast<float>(buttonCount - 1));

        auto offsetForIndex = [&](int idx) {
            const float start = -(totalHeight / 2.0f) + (buttonHeight / 2.0f);
            return start + (buttonHeight + spacing) * static_cast<float>(idx);
        };

        // Initialize game speed slider
        const float sliderWidth = 300.0f;
        const float sliderHeight = 20.0f;
        const float sliderTopMargin = 120.0f;

        if (_gameSpeedSlider) {
            _gameSpeedSlider->SetSize(sliderWidth, sliderHeight);
            _gameSpeedSlider->SetMinValue(0.25f);  // 25% speed (very slow)
            _gameSpeedSlider->SetMaxValue(1.0f);   // 100% speed (normal)
            _gameSpeedSlider->SetValue(_gameSpeed);
            _gameSpeedSlider->SetTrackColor(0xFF505050);
            _gameSpeedSlider->SetFilledColor(0xFFFF9800);  // Orange for speed
            _gameSpeedSlider->SetHandleColor(0xFFFFFFFF);
            _gameSpeedSlider->SetHandleHoverColor(0xFFE0E0E0);
            _gameSpeedSlider->SetAlign(UI::Align::CENTER_HORIZONTAL);
            _gameSpeedSlider->ApplyAlignment();

            float sliderX, sliderY;
            _gameSpeedSlider->GetPosition(sliderX, sliderY);
            _gameSpeedSlider->SetPosition(sliderX, sliderTopMargin);

            _gameSpeedSlider->SetOnValueChanged([this](float value) {
                _gameSpeed = ClampGameSpeed(value);
                LOG_INFO("[AccessibilityMenu] Game speed changed: ", static_cast<int>(_gameSpeed * 100), "%");
                if (_onGameSpeedChanged) {
                    _onGameSpeedChanged(_gameSpeed);
                }
            });
        }

        // Colorblind Filter button (cycles through filter types)
        _menu->AddButton(CreateCenteredButton(
            "COLORBLIND: NONE", offsetForIndex(0), buttonWidth, buttonHeight, 0xFF424242, 0xFF616161,
            [this]() { SetColorblindFilter(NextColorblindFilter(_colorblindFilter)); }));

        // Visual Sound Indicators toggle
        _menu->AddButton(CreateCenteredButton(
            "VISUAL SOUNDS: OFF", offsetForIndex(1), buttonWidth, buttonHeight, 0xFF424242, 0xFF616161,
            [this]() { SetVisualSoundIndicators(!_visualSoundIndicators); }));

        // Key Bindings configuration button
        _menu->AddButton(CreateCenteredButton("CONFIGURE KEY BINDINGS", offsetForIndex(2), buttonWidth,
                                              buttonHeight, 0xFF5E35B1, 0xFF7E57C2,
                                              [this]() { OpenKeyBindingsConfig(); }));

        // Back button
        _menu->AddButton(CreateCenteredButton("BACK", offsetForIndex(3), buttonWidth, buttonHeight,
                                              0xFF1976D2, 0xFF1E88E5, [this]() {
                                                  if (_onBack) {
                                                      _onBack();
                                                  } else {
                                                      Hide();
                                                  }
                                              }));

        // Main menu (only in overlay mode)
        if (showMainMenuButton) {
            _menu->AddButton(CreateCenteredButton("MAIN MENU", offsetForIndex(4), buttonWidth, buttonHeight,
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
        if (_gameSpeedSlider && _menu && _menu->IsVisible()) {
            _gameSpeedSlider->Update();
        }
    }

    void AccessibilityMenu::Render() {
        if (!_menu || !_menu->IsVisible()) {
            return;
        }

        // Draw game speed settings section
        if (_gameSpeedSlider) {
            float sliderX, sliderY;
            _gameSpeedSlider->GetPosition(sliderX, sliderY);

            int screenWidth = _graphics.GetScreenWidth();

            // Draw "ACCESSIBILITY SETTINGS" title at the very top
            const char *title = "ACCESSIBILITY SETTINGS";
            int titleFontSize = 24;
            int titleWidth = static_cast<int>(strlen(title) * titleFontSize * 0.6f);
            int titleX = (screenWidth - titleWidth) / 2;
            int titleY = static_cast<int>(sliderY) - 65;

            _graphics.DrawText(title, titleX, titleY, titleFontSize, 0xFF4CAF50);

            // Draw "GAME SPEED:" label above slider
            const char *label = "GAME SPEED:";
            int fontSize = 18;
            int labelWidth = static_cast<int>(strlen(label) * fontSize * 0.6f);
            int labelX = (screenWidth - labelWidth) / 2;
            int labelY = static_cast<int>(sliderY) - 30;

            _graphics.DrawText(label, labelX, labelY, fontSize, 0xFFFFFFFF);

            // Draw game speed percentage below slider
            char speedText[32];
            snprintf(speedText, sizeof(speedText), "%d%%", static_cast<int>(_gameSpeed * 100));
            int speedTextWidth = static_cast<int>(strlen(speedText) * fontSize * 0.6f);
            int speedTextX = (screenWidth - speedTextWidth) / 2;
            int speedTextY = static_cast<int>(sliderY) + 35;

            _graphics.DrawText(speedText, speedTextX, speedTextY, fontSize, 0xFFFF9800);

            // Draw slider
            _gameSpeedSlider->Render();
        }

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

    // --- Visual Sound Indicators ---
    void AccessibilityMenu::SetVisualSoundIndicators(bool enabled) {
        if (_visualSoundIndicators != enabled) {
            _visualSoundIndicators = enabled;
            UpdateVisualSoundIndicatorsVisuals();
            LOG_INFO("[AccessibilityMenu] Visual sound indicators: ", enabled ? "ON" : "OFF");
            if (_onVisualSoundIndicatorsChanged) {
                _onVisualSoundIndicatorsChanged(enabled);
            }
        }
    }

    bool AccessibilityMenu::GetVisualSoundIndicators() const {
        return _visualSoundIndicators;
    }

    void AccessibilityMenu::SetOnVisualSoundIndicatorsChanged(std::function<void(bool)> callback) {
        _onVisualSoundIndicatorsChanged = std::move(callback);
    }

    void AccessibilityMenu::SetVisualSoundIndicatorsSilent(bool enabled) {
        _visualSoundIndicators = enabled;
        UpdateVisualSoundIndicatorsVisuals();
    }

    // --- Game Speed ---
    void AccessibilityMenu::SetGameSpeed(float speed) {
        float clampedSpeed = ClampGameSpeed(speed);
        if (_gameSpeed != clampedSpeed) {
            _gameSpeed = clampedSpeed;
            if (_gameSpeedSlider) {
                _gameSpeedSlider->SetValue(_gameSpeed);
            }
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
        if (_gameSpeedSlider) {
            _gameSpeedSlider->SetValue(_gameSpeed);
        }
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
        UpdateVisualSoundIndicatorsVisuals();
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

    void AccessibilityMenu::UpdateVisualSoundIndicatorsVisuals() {
        if (!_menu) {
            return;
        }

        const char *status = _visualSoundIndicators ? "ON" : "OFF";
        char buttonText[64];
        snprintf(buttonText, sizeof(buttonText), "VISUAL SOUNDS: %s", status);

        if (_menu->GetButtonCount() > VISUAL_SOUND_INDEX) {
            auto button = _menu->GetButton(VISUAL_SOUND_INDEX);
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
