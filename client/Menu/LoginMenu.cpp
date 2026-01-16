/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** LoginMenu.cpp
*/

#include "LoginMenu.hpp"
#include "UI/TextUtils.hpp"  // For text measurement if needed
#include "raylib.h"          // For KEY_TAB, KEY_ENTER

namespace Game {

    LoginMenu::LoginMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics)
        : BaseMenu(uiFactory), _graphics(graphics) {}

    void LoginMenu::SetOnBack(std::function<void()> onBack) {
        _onBack = std::move(onBack);
    }

    void LoginMenu::Initialize() {
        if (!_menu) {
            return;
        }
        _menu->Clear();

        const float startY = _graphics.GetScreenHeight() / 2.0f - 100.0f;
        float currentY = startY;

        // --- Username Input ---
        _usernameInput = _uiFactory.CreateTextInput();
        _usernameInput->SetSize(INPUT_WIDTH, INPUT_HEIGHT);
        _usernameInput->SetAlign(UI::Align::CENTER_HORIZONTAL);
        _usernameInput->ApplyAlignment();

        float x, y;
        _usernameInput->GetPosition(x, y);
        _usernameInput->SetPosition(x, currentY);

        _usernameInput->SetPlaceholder("Username");
        _usernameInput->SetMaxLength(20);
        _usernameInput->SetValidationRegex("[a-zA-Z0-9_-]+");
        _usernameInput->SetBackgroundColor(0xFFFFFFFF);
        _usernameInput->SetBorderColor(0xFF808080);
        _usernameInput->SetActiveBorderColor(0xFF0000FF);  // Blue when active
        _usernameInput->SetTextColor(0xFF000000);
        _usernameInput->SetPlaceholderColor(0xFF808080);

        currentY += INPUT_HEIGHT + SPACING;

        // --- Password Input ---
        _passwordInput = _uiFactory.CreateTextInput();
        _passwordInput->SetSize(INPUT_WIDTH, INPUT_HEIGHT);
        _passwordInput->SetAlign(UI::Align::CENTER_HORIZONTAL);
        _passwordInput->ApplyAlignment();

        _passwordInput->GetPosition(x, y);
        _passwordInput->SetPosition(x, currentY);

        _passwordInput->SetPlaceholder("Password");
        _passwordInput->SetMaxLength(20);
        _passwordInput->SetPasswordMode(true);  // Mask password characters with asterisks
        _passwordInput->SetValidationRegex("[a-zA-Z0-9_-]+");

        _passwordInput->SetBackgroundColor(0xFFFFFFFF);
        _passwordInput->SetBorderColor(0xFF808080);
        _passwordInput->SetActiveBorderColor(0xFF0000FF);
        _passwordInput->SetTextColor(0xFF000000);
        _passwordInput->SetPlaceholderColor(0xFF808080);

        currentY += INPUT_HEIGHT + SPACING * 2.0f;

        // --- Buttons ---
        const float btnWidth = 100.0f;
        const float btnHeight = 40.0f;
        const float btnSpacing = 10.0f;

        // Helper to place buttons horizontally centered
        auto createBtn = [&](const char *label, float xOffset, unsigned int color, unsigned int hover,
                             auto cb) {
            // Convert unique_ptr to shared_ptr explicitly for IMenu::AddButton
            std::shared_ptr<UI::IButton> btn = _uiFactory.CreateButton();

            btn->SetSize(btnWidth, btnHeight);
            btn->SetAlign(UI::Align::CENTER_HORIZONTAL);
            btn->ApplyAlignment();

            float bx, by;
            btn->GetPosition(bx, by);
            // xOffset is relative to center
            btn->SetPosition(bx + xOffset, currentY);

            btn->SetText(label);
            btn->SetBackgroundColor(color);
            btn->SetHoverColor(hover);
            btn->SetTextColor(0xFFFFFFFF);
            btn->SetCallback(cb);

            _menu->AddButton(btn);
        };

        // Login (Left)
        createBtn("LOGIN", -(btnWidth + btnSpacing), 0xFF006400, 0xFF008000, [this]() { OnLoginClicked(); });

        // Register (Center)
        createBtn("REGISTER", 0, 0xFF00008B, 0xFF0000FF, [this]() { OnRegisterClicked(); });

        // Guest (Right)
        createBtn("GUEST", (btnWidth + btnSpacing), 0xFF646464, 0xFF808080, [this]() { OnGuestClicked(); });

        // Back (Bottom)
        std::shared_ptr<UI::IButton> backBtn = _uiFactory.CreateButton();
        backBtn->SetSize(btnWidth, btnHeight);
        backBtn->SetAlign(UI::Align::CENTER_HORIZONTAL);
        backBtn->ApplyAlignment();

        float bx, by;
        backBtn->GetPosition(bx, by);
        backBtn->SetPosition(bx, currentY + btnHeight + SPACING);

        backBtn->SetText("BACK");
        backBtn->SetBackgroundColor(0xFF424242);
        backBtn->SetHoverColor(0xFF616161);
        backBtn->SetTextColor(0xFFFFFFFF);
        backBtn->SetCallback([this]() {
            if (_onBack)
                _onBack();
        });
        _menu->AddButton(backBtn);
    }

    void LoginMenu::Update() {
        BaseMenu::Update();

        if (_usernameInput)
            _usernameInput->Update();
        if (_passwordInput)
            _passwordInput->Update();

        HandleTabNavigation();

        // Handle Enter key for submission
        if (_graphics.IsKeyPressed(KEY_ENTER)) {
            OnLoginClicked();
        }
    }

    void LoginMenu::Render() {
        // Draw background
        _graphics.ClearWindow();
        _graphics.DrawRectFilled(0, 0, _graphics.GetScreenWidth(), _graphics.GetScreenHeight(),
                                 0xFFF5F5F5);  // RAYWHITE-ish

        // Draw Title
        const char *title = "R-TYPE LOGIN";
        int titleFontSize = 40;
        int titleWidth = UI::TextUtils::EstimateTextWidth(title, titleFontSize);
        _graphics.DrawText(-1, title, (_graphics.GetScreenWidth() - titleWidth) / 2,
                           _graphics.GetScreenHeight() / 2 - 160, titleFontSize, 0xFF00008B);  // DARKBLUE

        // Draw Inputs
        if (_usernameInput)
            _usernameInput->Render();
        if (_passwordInput)
            _passwordInput->Render();

        // Draw Buttons
        BaseMenu::Render();

        // Draw Messages
        if (!_errorMessage.empty()) {
            int w = UI::TextUtils::EstimateTextWidth(_errorMessage, 20);
            _graphics.DrawText(-1, _errorMessage.c_str(), (_graphics.GetScreenWidth() - w) / 2,
                               _graphics.GetScreenHeight() / 2 + 150, 20, 0xFFFF0000);  // RED
        }
        if (!_successMessage.empty()) {
            int w = UI::TextUtils::EstimateTextWidth(_successMessage, 20);
            _graphics.DrawText(-1, _successMessage.c_str(), (_graphics.GetScreenWidth() - w) / 2,
                               _graphics.GetScreenHeight() / 2 + 150, 20, 0xFF008000);  // GREEN
        }

        // Hint
        const char *hint = "Tab to switch fields | Enter to login";
        int hintW = UI::TextUtils::EstimateTextWidth(hint, 15);
        _graphics.DrawText(-1, hint, (_graphics.GetScreenWidth() - hintW) / 2,
                           _graphics.GetScreenHeight() - 40, 15, 0xFF808080);
    }

    void LoginMenu::HandleTabNavigation() {
        if (_graphics.IsKeyPressed(KEY_TAB)) {
            if (_usernameInput && _passwordInput) {
                if (_usernameInput->IsFocused()) {
                    _usernameInput->SetFocused(false);
                    _passwordInput->SetFocused(true);
                } else {
                    // Default to username if password focused or neither
                    _usernameInput->SetFocused(true);
                    _passwordInput->SetFocused(false);
                }
            }
        }
    }

    void LoginMenu::OnLoginClicked() {
        if (GetUsername().empty() || GetPassword().empty()) {
            SetErrorMessage("Please enter username and password");
            return;
        }
        _loginSubmitted = true;
    }

    void LoginMenu::OnRegisterClicked() {
        if (GetUsername().empty() || GetPassword().empty()) {
            SetErrorMessage("Please enter username and password");
            return;
        }
        _registerSubmitted = true;
    }

    void LoginMenu::OnGuestClicked() {
        _guestSubmitted = true;
    }

    // Getters
    bool LoginMenu::IsLoginSubmitted() const {
        return _loginSubmitted;
    }
    bool LoginMenu::IsRegisterSubmitted() const {
        return _registerSubmitted;
    }
    bool LoginMenu::IsGuestSubmitted() const {
        return _guestSubmitted;
    }

    std::string LoginMenu::GetUsername() const {
        return _usernameInput ? _usernameInput->GetText() : "";
    }

    std::string LoginMenu::GetPassword() const {
        return _passwordInput ? _passwordInput->GetText() : "";
    }

    void LoginMenu::SetErrorMessage(const std::string &message) {
        _errorMessage = message;
        _successMessage.clear();
    }

    void LoginMenu::SetSuccessMessage(const std::string &message) {
        _successMessage = message;
        _errorMessage.clear();
    }

    void LoginMenu::ResetMessages() {
        _errorMessage.clear();
        _successMessage.clear();
    }

    void LoginMenu::Reset() {
        if (_usernameInput)
            _usernameInput->Clear();
        if (_passwordInput)
            _passwordInput->Clear();
        _loginSubmitted = false;
        _registerSubmitted = false;
        _guestSubmitted = false;
        ResetMessages();

        // Focus username by default
        if (_usernameInput)
            _usernameInput->SetFocused(true);
        if (_passwordInput)
            _passwordInput->SetFocused(false);
    }

}  // namespace Game
