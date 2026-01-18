/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** SettingsMenu - Settings menu (business logic)
*/

#include "Menu/SettingsMenu.hpp"
#include <cstring>
#include "../common/Logger/Logger.hpp"

namespace Game {
    SettingsMenu::SettingsMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics)
        : BaseMenu(uiFactory), _graphics(graphics) {
        _volumeSlider = _uiFactory.CreateSlider();
    }

    void SettingsMenu::Initialize() {
        if (!_menu) {
            return;
        }

        _menu->Clear();

        const float buttonWidth = 360.0f;
        const float buttonHeight = 50.0f;
        const float spacing = 18.0f;

        const bool showMainMenuButton = (_mode == Mode::OVERLAY);
        const int buttonCount = showMainMenuButton ? 6 : 5;

        // Center buttons vertically as a stack
        const float totalHeight = (buttonHeight * static_cast<float>(buttonCount)) +
                                  (spacing * static_cast<float>(buttonCount - 1));

        auto offsetForIndex = [&](int idx) {
            // idx in [0..buttonCount-1], centered stack
            const float start = -(totalHeight / 2.0f) + (buttonHeight / 2.0f);
            return start + (buttonHeight + spacing) * static_cast<float>(idx);
        };

        // Initialize volume slider (after offsetForIndex is defined)
        const float sliderWidth = 300.0f;
        const float sliderHeight = 20.0f;
        const float sliderTopMargin = 120.0f;  // Distance from top of screen

        if (_volumeSlider) {
            _volumeSlider->SetSize(sliderWidth, sliderHeight);
            _volumeSlider->SetMinValue(0.0f);
            _volumeSlider->SetMaxValue(100.0f);
            _volumeSlider->SetValue(_volume);
            _volumeSlider->SetTrackColor(0xFF505050);
            _volumeSlider->SetFilledColor(0xFF4CAF50);
            _volumeSlider->SetHandleColor(0xFFFFFFFF);
            _volumeSlider->SetHandleHoverColor(0xFFE0E0E0);
            _volumeSlider->SetAlign(UI::Align::CENTER_HORIZONTAL);
            _volumeSlider->ApplyAlignment();

            // Position slider at fixed position from top of screen
            float sliderX, sliderY;
            _volumeSlider->GetPosition(sliderX, sliderY);
            _volumeSlider->SetPosition(sliderX, sliderTopMargin);

            _volumeSlider->SetOnValueChanged([this](float value) {
                _volume = value;
                LOG_INFO("[SettingsMenu] Volume changed: ", static_cast<int>(value), "%");
                if (_onVolumeChanged) {
                    _onVolumeChanged(value);
                }
            });
        }

        // Toggle Ping button
        _menu->AddButton(CreateCenteredButton("PING: ON", offsetForIndex(0), buttonWidth, buttonHeight,
                                              0xFF424242, 0xFF616161, [this]() { SetShowPing(!_showPing); }));

        // Toggle FPS display button
        _menu->AddButton(CreateCenteredButton("FPS: ON", offsetForIndex(1), buttonWidth, buttonHeight,
                                              0xFF424242, 0xFF616161, [this]() { SetShowFps(!_showFps); }));

        // Toggle Chat button
        _menu->AddButton(CreateCenteredButton("CHAT: ON", offsetForIndex(2), buttonWidth, buttonHeight,
                                              0xFF424242, 0xFF616161, [this]() { SetShowChat(!_showChat); }));

        // Toggle Auto-Matchmaking button
        _menu->AddButton(CreateCenteredButton("AUTO-MM: OFF", offsetForIndex(3), buttonWidth, buttonHeight,
                                              0xFF424242, 0xFF616161,
                                              [this]() { SetAutoMatchmaking(!_autoMatchmaking); }));

        // Target FPS selection button (cycles)
        _menu->AddButton(CreateCenteredButton("TARGET FPS: 60", offsetForIndex(4), buttonWidth, buttonHeight,
                                              0xFF424242, 0xFF616161,
                                              [this]() { SetTargetFps(NextTargetFps(_targetFps)); }));

        // Accessibility button
        _menu->AddButton(CreateCenteredButton("ACCESSIBILITY", offsetForIndex(4), buttonWidth, buttonHeight,
                                              0xFF6A1B9A, 0xFF8E24AA, [this]() {
                                                  if (_onAccessibility) {
                                                      _onAccessibility();
                                                  }
                                              }));

        // Back (closes settings)
        _menu->AddButton(CreateCenteredButton("BACK", offsetForIndex(5), buttonWidth, buttonHeight,
                                              0xFF1976D2, 0xFF1E88E5, [this]() {
                                                  if (_onBack) {
                                                      _onBack();
                                                  } else {
                                                      Hide();
                                                  }
                                              }));

        // Main menu (only in overlay)
        if (showMainMenuButton) {
            _menu->AddButton(CreateCenteredButton("MAIN MENU", offsetForIndex(6), buttonWidth, buttonHeight,
                                                  0xFF5D4037, 0xFF6D4C41, [this]() {
                                                      if (_onMainMenu) {
                                                          _onMainMenu();
                                                      }
                                                  }));
        }

        RefreshVisuals();
    }

    void SettingsMenu::Update() {
        BaseMenu::Update();
        if (_volumeSlider && _menu && _menu->IsVisible()) {
            _volumeSlider->Update();
        }
    }

    void SettingsMenu::Render() {
        if (!_menu || !_menu->IsVisible()) {
            return;
        }

        // Draw volume settings section
        if (_volumeSlider) {
            float sliderX, sliderY;
            _volumeSlider->GetPosition(sliderX, sliderY);

            int screenWidth = _graphics.GetScreenWidth();

            // Draw "VOLUME SETTINGS" title at the very top
            const char *title = "VOLUME SETTINGS";
            int titleFontSize = 24;
            int titleWidth = static_cast<int>(strlen(title) * titleFontSize * 0.6f);
            int titleX = (screenWidth - titleWidth) / 2;
            int titleY = static_cast<int>(sliderY) - 65;  // Above the "VOLUME:" label

            _graphics.DrawText(title, titleX, titleY, titleFontSize, 0xFF4CAF50);  // Green color

            // Draw "VOLUME:" label above slider
            const char *label = "VOLUME:";
            int fontSize = 18;
            int labelWidth = static_cast<int>(strlen(label) * fontSize * 0.6f);
            int labelX = (screenWidth - labelWidth) / 2;
            int labelY = static_cast<int>(sliderY) - 30;  // Closer to slider

            _graphics.DrawText(label, labelX, labelY, fontSize, 0xFFFFFFFF);

            // Draw volume percentage below slider
            int volumePercent = static_cast<int>(_volume);
            std::string valueText = std::to_string(volumePercent) + "%";
            int valueWidth = static_cast<int>(valueText.length() * fontSize * 0.6f);
            int valueX = (screenWidth - valueWidth) / 2;
            int valueY = static_cast<int>(sliderY) + 30;

            _graphics.DrawText(valueText.c_str(), valueX, valueY, fontSize, 0xFFFFFFFF);

            // Render slider
            _volumeSlider->Render();
        }

        BaseMenu::Render();
    }

    void SettingsMenu::SetMode(Mode mode) {
        _mode = mode;
    }

    SettingsMenu::Mode SettingsMenu::GetMode() const {
        return _mode;
    }

    void SettingsMenu::SetShowPing(bool enabled) {
        _showPing = enabled;
        UpdateToggleVisuals();
        if (_onShowPingChanged) {
            _onShowPingChanged(_showPing);
        }
        LOG_INFO("[SettingsMenu] showPing=", _showPing ? "true" : "false");
    }

    bool SettingsMenu::GetShowPing() const {
        return _showPing;
    }

    void SettingsMenu::SetOnShowPingChanged(std::function<void(bool)> cb) {
        _onShowPingChanged = std::move(cb);
    }

    void SettingsMenu::SetOnBack(std::function<void()> cb) {
        _onBack = std::move(cb);
    }

    void SettingsMenu::SetOnMainMenu(std::function<void()> cb) {
        _onMainMenu = std::move(cb);
    }

    void SettingsMenu::SetOnAccessibility(std::function<void()> cb) {
        _onAccessibility = std::move(cb);
    }

    void SettingsMenu::SetShowFps(bool enabled) {
        _showFps = enabled;
        UpdateFpsToggleVisuals();
        if (_onShowFpsChanged) {
            _onShowFpsChanged(_showFps);
        }
        LOG_INFO("[SettingsMenu] showFps=", _showFps ? "true" : "false");
    }

    bool SettingsMenu::GetShowFps() const {
        return _showFps;
    }

    void SettingsMenu::SetOnShowFpsChanged(std::function<void(bool)> cb) {
        _onShowFpsChanged = std::move(cb);
    }

    uint32_t SettingsMenu::NextTargetFps(uint32_t current) const {
        switch (current) {
            case 30:
                return 60;
            case 60:
                return 120;
            case 120:
                return 144;
            case 144:
                return 240;
            default:
                return 30;
        }
    }

    uint32_t SettingsMenu::ValidateTargetFps(uint32_t targetFps) const {
        switch (targetFps) {
            case 30:
            case 60:
            case 120:
            case 144:
            case 240:
                return targetFps;
            default:
                return 60;
        }
    }

    void SettingsMenu::RefreshVisuals() {
        UpdateToggleVisuals();
        UpdateFpsToggleVisuals();
        UpdateChatToggleVisuals();
        UpdateAutoMatchmakingVisuals();
        UpdateTargetFpsVisuals();
    }

    void SettingsMenu::SetShowPingSilent(bool enabled) {
        _showPing = enabled;
        UpdateToggleVisuals();
    }

    void SettingsMenu::SetShowFpsSilent(bool enabled) {
        _showFps = enabled;
        UpdateFpsToggleVisuals();
    }

    void SettingsMenu::SetTargetFpsSilent(uint32_t targetFps) {
        _targetFps = ValidateTargetFps(targetFps);
        UpdateTargetFpsVisuals();
    }

    void SettingsMenu::SetTargetFps(uint32_t targetFps) {
        _targetFps = ValidateTargetFps(targetFps);

        UpdateTargetFpsVisuals();
        if (_onTargetFpsChanged) {
            _onTargetFpsChanged(_targetFps);
        }
        LOG_INFO("[SettingsMenu] targetFps=", _targetFps);
    }

    uint32_t SettingsMenu::GetTargetFps() const {
        return _targetFps;
    }

    void SettingsMenu::SetOnTargetFpsChanged(std::function<void(uint32_t)> callback) {
        _onTargetFpsChanged = std::move(callback);
    }

    void SettingsMenu::UpdateToggleVisuals() {
        if (!_menu) {
            return;
        }
        auto toggleBtn = _menu->GetButton(TOGGLE_PING_INDEX);
        if (!toggleBtn) {
            return;
        }

        if (_showPing) {
            toggleBtn->SetText("PING: ON");
            toggleBtn->SetBackgroundColor(0xFF2E7D32);
            toggleBtn->SetHoverColor(0xFF388E3C);
        } else {
            toggleBtn->SetText("PING: OFF");
            toggleBtn->SetBackgroundColor(0xFFB71C1C);
            toggleBtn->SetHoverColor(0xFFD32F2F);
        }
        toggleBtn->SetTextColor(0xFFFFFFFF);
    }

    void SettingsMenu::UpdateFpsToggleVisuals() {
        if (!_menu) {
            return;
        }
        auto toggleBtn = _menu->GetButton(TOGGLE_FPS_INDEX);
        if (!toggleBtn) {
            return;
        }

        if (_showFps) {
            toggleBtn->SetText("FPS: ON");
            toggleBtn->SetBackgroundColor(0xFF2E7D32);
            toggleBtn->SetHoverColor(0xFF388E3C);
        } else {
            toggleBtn->SetText("FPS: OFF");
            toggleBtn->SetBackgroundColor(0xFFB71C1C);
            toggleBtn->SetHoverColor(0xFFD32F2F);
        }
        toggleBtn->SetTextColor(0xFFFFFFFF);
    }

    void SettingsMenu::UpdateTargetFpsVisuals() {
        if (!_menu) {
            return;
        }
        auto targetBtn = _menu->GetButton(TARGET_FPS_INDEX);
        if (!targetBtn) {
            return;
        }

        targetBtn->SetText("TARGET FPS: " + std::to_string(_targetFps));
        targetBtn->SetBackgroundColor(0xFF424242);
        targetBtn->SetHoverColor(0xFF616161);
        targetBtn->SetTextColor(0xFFFFFFFF);
    }

    void SettingsMenu::SetOverlayDimColor(unsigned int color) {
        _overlayDimColor = color;
    }

    unsigned int SettingsMenu::GetOverlayDimColor() const {
        return _overlayDimColor;
    }

    bool SettingsMenu::ShouldDimBackground() const {
        return _mode == Mode::OVERLAY && IsVisible();
    }

    void SettingsMenu::SetVolume(float volume) {
        _volume = std::max(0.0f, std::min(100.0f, volume));
        if (_volumeSlider) {
            _volumeSlider->SetValue(_volume);
        }
        LOG_INFO("[SettingsMenu] Volume set to: ", static_cast<int>(_volume), "%");
        if (_onVolumeChanged) {
            _onVolumeChanged(_volume);
        }
    }

    float SettingsMenu::GetVolume() const {
        return _volume;
    }

    void SettingsMenu::SetOnVolumeChanged(std::function<void(float)> callback) {
        _onVolumeChanged = std::move(callback);
    }

    void SettingsMenu::SetVolumeSilent(float volume) {
        _volume = std::max(0.0f, std::min(100.0f, volume));
        if (_volumeSlider) {
            _volumeSlider->SetValue(_volume);
        }
    }

    void SettingsMenu::SetShowChat(bool enabled) {
        _showChat = enabled;
        UpdateChatToggleVisuals();
        if (_onShowChatChanged) {
            _onShowChatChanged(_showChat);
        }
        LOG_INFO("[SettingsMenu] showChat=", _showChat ? "true" : "false");
    }

    bool SettingsMenu::GetShowChat() const {
        return _showChat;
    }

    void SettingsMenu::SetOnShowChatChanged(std::function<void(bool)> cb) {
        _onShowChatChanged = std::move(cb);
    }

    void SettingsMenu::SetShowChatSilent(bool enabled) {
        _showChat = enabled;
        UpdateChatToggleVisuals();
    }

    void SettingsMenu::UpdateChatToggleVisuals() {
        if (!_menu) {
            return;
        }
        auto toggleBtn = _menu->GetButton(TOGGLE_CHAT_INDEX);
        if (!toggleBtn) {
            return;
        }

        if (_showChat) {
            toggleBtn->SetText("CHAT: ON");
            toggleBtn->SetBackgroundColor(0xFF2E7D32);
            toggleBtn->SetHoverColor(0xFF388E3C);
        } else {
            toggleBtn->SetText("CHAT: OFF");
            toggleBtn->SetBackgroundColor(0xFFB71C1C);
            toggleBtn->SetHoverColor(0xFFD32F2F);
        }
        toggleBtn->SetTextColor(0xFFFFFFFF);
    }

    void SettingsMenu::SetAutoMatchmaking(bool enabled) {
        _autoMatchmaking = enabled;
        UpdateAutoMatchmakingVisuals();
        if (_onAutoMatchmakingChanged) {
            _onAutoMatchmakingChanged(_autoMatchmaking);
        }
        LOG_INFO("[SettingsMenu] Auto-matchmaking=", _autoMatchmaking ? "true" : "false",
                 " (notifying server)");
    }

    void SettingsMenu::ApplyAutoMatchmakingPreference(bool enabled) {
        // Apply preference silently without triggering callback
        // This is used when loading preference from server after login
        _autoMatchmaking = enabled;
        UpdateAutoMatchmakingVisuals();
        LOG_INFO("[SettingsMenu] Auto-matchmaking preference applied from server: ",
                 _autoMatchmaking ? "ON" : "OFF", " (no server notification)");
    }

    bool SettingsMenu::GetAutoMatchmaking() const {
        return _autoMatchmaking;
    }

    void SettingsMenu::SetOnAutoMatchmakingChanged(std::function<void(bool)> cb) {
        _onAutoMatchmakingChanged = std::move(cb);
    }

    void SettingsMenu::UpdateAutoMatchmakingVisuals() {
        if (!_menu) {
            return;
        }
        auto toggleBtn = _menu->GetButton(AUTO_MATCHMAKING_INDEX);
        if (!toggleBtn) {
            return;
        }

        if (_autoMatchmaking) {
            toggleBtn->SetText("AUTO-MM: ON");
            toggleBtn->SetBackgroundColor(0xFF2E7D32);
            toggleBtn->SetHoverColor(0xFF388E3C);
        } else {
            toggleBtn->SetText("AUTO-MM: OFF");
            toggleBtn->SetBackgroundColor(0xFFB71C1C);
            toggleBtn->SetHoverColor(0xFFD32F2F);
        }
        toggleBtn->SetTextColor(0xFFFFFFFF);
    }
}  // namespace Game
