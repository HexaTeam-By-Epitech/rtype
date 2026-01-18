/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** KeyBindingsMenu - Menu for remapping key bindings implementation
*/

#include "Menu/KeyBindingsMenu.hpp"
#include <raylib.h>
#include "../common/Logger/Logger.hpp"

namespace Game {

    KeyBindingsMenu::KeyBindingsMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics)
        : BaseMenu(uiFactory), _graphics(graphics) {}

    void KeyBindingsMenu::Initialize() {
        if (!_menu) {
            return;
        }

        _menu->Clear();
        _bindingButtons.clear();

        // Title (using a disabled button as label)
        std::shared_ptr<UI::IButton> titleLabel = _uiFactory.CreateButton();
        titleLabel->SetSize(400, 40);
        titleLabel->SetAlign(UI::Align::CENTER_HORIZONTAL);
        titleLabel->ApplyAlignment();
        float labelX = 0.0f;
        float labelY = 0.0f;
        titleLabel->GetPosition(labelX, labelY);
        titleLabel->SetPosition(labelX, 50);
        titleLabel->SetBackgroundColor(0x00000000);  // Transparent
        titleLabel->SetHoverColor(0x00000000);
        titleLabel->SetText("Key Bindings");
        titleLabel->SetTextSize(28);
        titleLabel->SetTextColor(0xFFFFFFFF);
        titleLabel->SetFont(-1);
        _menu->AddButton(titleLabel);

        // Instructions label
        std::shared_ptr<UI::IButton> instructionsLabel = _uiFactory.CreateButton();
        instructionsLabel->SetSize(500, 30);
        instructionsLabel->SetAlign(UI::Align::CENTER_HORIZONTAL);
        instructionsLabel->ApplyAlignment();
        instructionsLabel->GetPosition(labelX, labelY);
        instructionsLabel->SetPosition(labelX, 95);
        instructionsLabel->SetBackgroundColor(0x00000000);
        instructionsLabel->SetHoverColor(0x00000000);
        instructionsLabel->SetText("Click a key to remap. Press ESC to cancel.");
        instructionsLabel->SetTextSize(14);
        instructionsLabel->SetTextColor(0xFFAAAAAA);
        instructionsLabel->SetFont(-1);
        _menu->AddButton(instructionsLabel);

        // Create binding rows for each action
        float startY = 140;
        float rowHeight = 45;
        int rowIndex = 0;

        // Movement actions
        CreateBindingRow(Input::GameAction::MOVE_UP, startY + rowHeight * static_cast<float>(rowIndex++));
        CreateBindingRow(Input::GameAction::MOVE_DOWN, startY + rowHeight * static_cast<float>(rowIndex++));
        CreateBindingRow(Input::GameAction::MOVE_LEFT, startY + rowHeight * static_cast<float>(rowIndex++));
        CreateBindingRow(Input::GameAction::MOVE_RIGHT, startY + rowHeight * static_cast<float>(rowIndex++));

        // Combat
        CreateBindingRow(Input::GameAction::SHOOT, startY + rowHeight * static_cast<float>(rowIndex++));

        // UI/System
        CreateBindingRow(Input::GameAction::PAUSE_MENU, startY + rowHeight * static_cast<float>(rowIndex++));
        CreateBindingRow(Input::GameAction::CHAT_OPEN, startY + rowHeight * static_cast<float>(rowIndex++));

        // Bottom buttons
        float bottomY = startY + rowHeight * static_cast<float>(rowIndex) + 30;

        // Reset to Defaults button
        _resetButton =
            CreateCenteredButton("Reset to Defaults", bottomY - (_graphics.GetWindowHeight() / 2.0f), 180, 40,
                                 0xFF505050, 0xFF707070, [this]() {
                                     Input::KeyBindings::getInstance().ResetToDefaults();
                                     RefreshAllBindings();
                                     if (_onBindingsChanged) {
                                         _onBindingsChanged();
                                     }
                                 });
        _resetButton->SetPosition(_graphics.GetWindowWidth() / 2.0f - 200, bottomY);
        _menu->AddButton(_resetButton);

        // Back button
        _backButton = CreateCenteredButton("Back", bottomY - (_graphics.GetWindowHeight() / 2.0f), 120, 40,
                                           0xFF404040, 0xFF606060, [this]() {
                                               if (_onBack) {
                                                   _onBack();
                                               }
                                           });
        _backButton->SetPosition(_graphics.GetWindowWidth() / 2.0f + 80, bottomY);
        _menu->AddButton(_backButton);
    }

    void KeyBindingsMenu::CreateBindingRow(Input::GameAction action, float yOffset) {
        auto &bindings = Input::KeyBindings::getInstance();
        float centerX = static_cast<float>(_graphics.GetWindowWidth()) / 2.0f;

        // Action name label
        std::shared_ptr<UI::IButton> actionLabel = _uiFactory.CreateButton();
        actionLabel->SetSize(150, 35);
        actionLabel->SetPosition(centerX - 280, yOffset);
        actionLabel->SetBackgroundColor(0x00000000);
        actionLabel->SetHoverColor(0x00000000);
        actionLabel->SetText(Input::KeyBindings::GetActionName(action));
        actionLabel->SetTextSize(16);
        actionLabel->SetTextColor(0xFFFFFFFF);
        actionLabel->SetFont(-1);
        _menu->AddButton(actionLabel);

        // Primary key button
        std::shared_ptr<UI::IButton> primaryButton = _uiFactory.CreateButton();
        primaryButton->SetSize(120, 35);
        primaryButton->SetPosition(centerX - 100, yOffset);
        primaryButton->SetBackgroundColor(0xFF404040);
        primaryButton->SetHoverColor(0xFF606060);
        primaryButton->SetText(Input::KeyBindings::GetBindingName(bindings.GetPrimaryKey(action)));
        primaryButton->SetTextSize(14);
        primaryButton->SetTextColor(0xFFFFFFFF);
        primaryButton->SetFont(-1);
        primaryButton->SetCallback([this, action]() { StartKeyCapture(action, true); });
        _menu->AddButton(primaryButton);

        // Secondary key button
        std::shared_ptr<UI::IButton> secondaryButton = _uiFactory.CreateButton();
        secondaryButton->SetSize(120, 35);
        secondaryButton->SetPosition(centerX + 40, yOffset);
        secondaryButton->SetBackgroundColor(0xFF404040);
        secondaryButton->SetHoverColor(0xFF606060);
        int secondaryBinding = bindings.GetSecondaryKey(action);
        secondaryButton->SetText(
            secondaryBinding != KEY_NULL ? Input::KeyBindings::GetBindingName(secondaryBinding) : "-");
        secondaryButton->SetTextSize(14);
        secondaryButton->SetTextColor(0xFFCCCCCC);
        secondaryButton->SetFont(-1);
        secondaryButton->SetCallback([this, action]() { StartKeyCapture(action, false); });
        _menu->AddButton(secondaryButton);

        // Clear secondary button
        std::shared_ptr<UI::IButton> clearButton = _uiFactory.CreateButton();
        clearButton->SetSize(30, 35);
        clearButton->SetPosition(centerX + 170, yOffset);
        clearButton->SetBackgroundColor(0xFF603030);
        clearButton->SetHoverColor(0xFF804040);
        clearButton->SetText("X");
        clearButton->SetTextSize(14);
        clearButton->SetTextColor(0xFFFFFFFF);
        clearButton->SetFont(-1);
        clearButton->SetCallback([this, action]() {
            Input::KeyBindings::getInstance().ClearSecondaryKey(action);
            UpdateBindingButtonText(action, false);
            if (_onBindingsChanged) {
                _onBindingsChanged();
            }
        });
        _menu->AddButton(clearButton);

        // Store button references
        _bindingButtons[action] = {primaryButton, secondaryButton};
    }

    void KeyBindingsMenu::Update() {
        if (!_menu || !_menu->IsVisible()) {
            return;
        }

        // Handle key capture mode
        if (_isCapturing) {
            // Check for ESC to cancel
            if (_graphics.IsKeyPressed(KEY_ESCAPE)) {
                CancelKeyCapture();
                return;
            }

            // Check for gamepad button presses first (on any connected gamepad)
            for (int gp = 0; gp < 4; ++gp) {
                if (_graphics.IsGamepadAvailable(gp)) {
                    for (int button = 0; button <= GAMEPAD_BUTTON_RIGHT_THUMB; ++button) {
                        if (_graphics.IsGamepadButtonPressed(gp, button)) {
                            // Convert gamepad button to binding value
                            int binding = Input::GamepadButtonToBinding(button);
                            HandleCapturedKey(binding);
                            return;
                        }
                    }
                }
            }

            // Check for any keyboard key press
            for (int key = KEY_SPACE; key <= KEY_KP_EQUAL; ++key) {
                if (_graphics.IsKeyPressed(key)) {
                    HandleCapturedKey(key);
                    return;
                }
            }
            // Also check letter keys
            for (int key = KEY_A; key <= KEY_Z; ++key) {
                if (_graphics.IsKeyPressed(key)) {
                    HandleCapturedKey(key);
                    return;
                }
            }

            // Don't process menu updates during capture
            return;
        }

        BaseMenu::Update();
    }

    void KeyBindingsMenu::Render() {
        if (!_menu || !_menu->IsVisible()) {
            return;
        }

        // Draw dim overlay if in overlay mode
        if (_mode == Mode::OVERLAY) {
            _graphics.DrawRectFilled(0, 0, _graphics.GetWindowWidth(), _graphics.GetWindowHeight(),
                                     _overlayDimColor);
        }

        BaseMenu::Render();

        // Draw capture overlay
        if (_isCapturing) {
            // Dim the whole screen
            _graphics.DrawRectFilled(0, 0, _graphics.GetWindowWidth(), _graphics.GetWindowHeight(),
                                     0xCC000000);

            // Draw instruction box
            int boxWidth = 400;
            int boxHeight = 100;
            int boxX = (_graphics.GetWindowWidth() - boxWidth) / 2;
            int boxY = (_graphics.GetWindowHeight() - boxHeight) / 2;

            _graphics.DrawRectFilled(boxX, boxY, boxWidth, boxHeight, 0xFF303030);
            _graphics.DrawRect(boxX, boxY, boxWidth, boxHeight, 0xFF4080FF);

            std::string actionName = Input::KeyBindings::GetActionName(_captureAction);
            std::string slotName = _capturePrimary ? "Primary" : "Secondary";
            std::string msg = "Press a key for: " + actionName + " (" + slotName + ")";

            _graphics.DrawText(msg.c_str(), boxX + 20, boxY + 30, 18, 0xFFFFFFFF);
            _graphics.DrawText("Press ESC to cancel", boxX + 120, boxY + 60, 14, 0xFFAAAAAA);
        }
    }

    void KeyBindingsMenu::StartKeyCapture(Input::GameAction action, bool isPrimary) {
        _isCapturing = true;
        _captureAction = action;
        _capturePrimary = isPrimary;

        LOG_INFO("[KeyBindingsMenu] Started key capture for: ", Input::KeyBindings::GetActionName(action),
                 " (", isPrimary ? "primary" : "secondary", ")");
    }

    void KeyBindingsMenu::CancelKeyCapture() {
        _isCapturing = false;
        LOG_INFO("[KeyBindingsMenu] Key capture cancelled");
    }

    void KeyBindingsMenu::HandleCapturedKey(int key) {
        auto &bindings = Input::KeyBindings::getInstance();

        if (_capturePrimary) {
            bindings.SetPrimaryKey(_captureAction, key);
        } else {
            bindings.SetSecondaryKey(_captureAction, key);
        }

        UpdateBindingButtonText(_captureAction, _capturePrimary);

        LOG_INFO("[KeyBindingsMenu] Bound ", Input::KeyBindings::GetActionName(_captureAction), " ",
                 _capturePrimary ? "primary" : "secondary", " to: ", Input::KeyBindings::GetKeyName(key));

        _isCapturing = false;

        if (_onBindingsChanged) {
            _onBindingsChanged();
        }
    }

    void KeyBindingsMenu::UpdateBindingButtonText(Input::GameAction action, bool isPrimary) {
        auto it = _bindingButtons.find(action);
        if (it == _bindingButtons.end()) {
            return;
        }

        auto &bindings = Input::KeyBindings::getInstance();

        if (isPrimary && it->second.primaryButton) {
            it->second.primaryButton->SetText(
                Input::KeyBindings::GetBindingName(bindings.GetPrimaryKey(action)));
        } else if (!isPrimary && it->second.secondaryButton) {
            int secondaryBinding = bindings.GetSecondaryKey(action);
            it->second.secondaryButton->SetText(
                secondaryBinding != KEY_NULL ? Input::KeyBindings::GetBindingName(secondaryBinding) : "-");
        }
    }

    void KeyBindingsMenu::RefreshAllBindings() {
        for (const auto &[action, buttons] : _bindingButtons) {
            UpdateBindingButtonText(action, true);
            UpdateBindingButtonText(action, false);
        }
    }

    void KeyBindingsMenu::SetMode(Mode mode) {
        _mode = mode;
    }

    KeyBindingsMenu::Mode KeyBindingsMenu::GetMode() const {
        return _mode;
    }

    void KeyBindingsMenu::SetOverlayDimColor(unsigned int color) {
        _overlayDimColor = color;
    }

    unsigned int KeyBindingsMenu::GetOverlayDimColor() const {
        return _overlayDimColor;
    }

    bool KeyBindingsMenu::ShouldDimBackground() const {
        return _mode == Mode::OVERLAY;
    }

    void KeyBindingsMenu::SetOnBack(std::function<void()> callback) {
        _onBack = std::move(callback);
    }

    void KeyBindingsMenu::SetOnBindingsChanged(std::function<void()> callback) {
        _onBindingsChanged = std::move(callback);
    }

}  // namespace Game
