/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ConnectionMenu - Menu for entering connection details
*/

#include "Menu/ConnectionMenu.hpp"
#include <regex>
#include <sstream>
#include "../common/Logger/Logger.hpp"

namespace Game {
    ConnectionMenu::ConnectionMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics)
        : BaseMenu(uiFactory), _graphics(graphics) {}

    void ConnectionMenu::SetOnJoin(
        std::function<void(const std::string &, const std::string &, const std::string &)> onJoin) {
        _onJoin = std::move(onJoin);
    }

    void ConnectionMenu::SetOnBack(std::function<void()> onBack) {
        _onBack = std::move(onBack);
    }

    void ConnectionMenu::Initialize() {
        if (!_menu) {
            return;
        }

        const float inputWidth = 300.0f;
        const float inputHeight = 40.0f;
        const float spacing = 20.0f;
        const float buttonWidth = 140.0f;
        const float buttonHeight = 45.0f;
        const float buttonSpacing = 20.0f;

        const float startY = 150.0f;  // Start from top with margin

        // Clear menu and create text inputs
        _menu->Clear();

        // --- Nickname Input ---
        _nicknameInput = _uiFactory.CreateTextInput();
        _nicknameInput->SetSize(inputWidth, inputHeight);
        _nicknameInput->SetAlign(UI::Align::CENTER_HORIZONTAL);
        _nicknameInput->ApplyAlignment();

        float x, y;
        _nicknameInput->GetPosition(x, y);
        _nicknameInput->SetPosition(x, startY);

        _nicknameInput->SetPlaceholder("Enter your nickname...");
        _nicknameInput->SetMaxLength(20);
        _nicknameInput->SetValidationRegex("[a-zA-Z0-9_-]+");
        _nicknameInput->SetTextSize(18);
        _nicknameInput->SetBackgroundColor(0xFF2A2A2A);
        _nicknameInput->SetBorderColor(0xFF505050);
        _nicknameInput->SetActiveBorderColor(0xFF4CAF50);
        _nicknameInput->SetTextColor(0xFFFFFFFF);
        _nicknameInput->SetPlaceholderColor(0xFF808080);
        _nicknameInput->SetOnTextChanged([this](const std::string &text) {
            LOG_INFO("[ConnectionMenu] Nickname: " + text);
            ClearError();  // Clear error when user types
        });

        // --- IP Input ---
        _ipInput = _uiFactory.CreateTextInput();
        _ipInput->SetSize(inputWidth, inputHeight);
        _ipInput->SetAlign(UI::Align::CENTER_HORIZONTAL);
        _ipInput->ApplyAlignment();

        _ipInput->GetPosition(x, y);
        float ipY = startY + inputHeight + spacing;
        _ipInput->SetPosition(x, ipY);

        _ipInput->SetPlaceholder("127.0.0.1");
        _ipInput->SetMaxLength(15);
        _ipInput->SetValidationRegex("[0-9.]+");
        _ipInput->SetTextSize(18);
        _ipInput->SetBackgroundColor(0xFF2A2A2A);
        _ipInput->SetBorderColor(0xFF505050);
        _ipInput->SetActiveBorderColor(0xFF4CAF50);
        _ipInput->SetTextColor(0xFFFFFFFF);
        _ipInput->SetPlaceholderColor(0xFF808080);
        _ipInput->SetOnTextChanged([this](const std::string &text) {
            LOG_INFO("[ConnectionMenu] IP: " + text);
            ClearError();  // Clear error when user types
        });

        // --- Port Input ---
        _portInput = _uiFactory.CreateTextInput();
        _portInput->SetSize(inputWidth, inputHeight);
        _portInput->SetAlign(UI::Align::CENTER_HORIZONTAL);
        _portInput->ApplyAlignment();

        _portInput->GetPosition(x, y);
        float portY = ipY + inputHeight + spacing;
        _portInput->SetPosition(x, portY);

        _portInput->SetPlaceholder("4242");
        _portInput->SetMaxLength(5);
        _portInput->SetValidationRegex("[0-9]+");
        _portInput->SetTextSize(18);
        _portInput->SetBackgroundColor(0xFF2A2A2A);
        _portInput->SetBorderColor(0xFF505050);
        _portInput->SetActiveBorderColor(0xFF4CAF50);
        _portInput->SetTextColor(0xFFFFFFFF);
        _portInput->SetPlaceholderColor(0xFF808080);
        _portInput->SetOnTextChanged([this](const std::string &text) {
            LOG_INFO("[ConnectionMenu] Port: " + text);
            ClearError();  // Clear error when user types
        });

        // --- Buttons (Join and Back) ---
        _buttonsY = portY + inputHeight + spacing * 2;  // Store for error message positioning
        _buttonHeight = buttonHeight;
        float totalButtonWidth = buttonWidth * 2 + buttonSpacing;
        float joinButtonX = -(totalButtonWidth / 2.0f);
        float backButtonX = joinButtonX + buttonWidth + buttonSpacing;

        // Helper to create buttons with horizontal alignment
        auto createHorizontalButton = [&](const char *label, float posX, unsigned int backgroundColor,
                                          unsigned int hoverColor, std::function<void()> callback) {
            auto button = _uiFactory.CreateButton();
            button->SetSize(buttonWidth, buttonHeight);
            button->SetAlign(UI::Align::CENTER_HORIZONTAL);
            button->ApplyAlignment();

            float bx, by;
            button->GetPosition(bx, by);
            button->SetPosition(bx + posX + (totalButtonWidth / 2.0f) - (buttonWidth / 2.0f), _buttonsY);

            button->SetBackgroundColor(backgroundColor);
            button->SetHoverColor(hoverColor);
            button->SetText(label);
            button->SetTextSize(18);
            button->SetTextColor(0xFFFFFFFF);
            button->SetFont(-1);
            button->SetCallback(std::move(callback));
            return button;
        };

        _menu->AddButton(createHorizontalButton("JOIN", joinButtonX, 0xFF4CAF50, 0xFF66BB6A,
                                                [this]() { OnJoinClicked(); }));

        _menu->AddButton(createHorizontalButton("BACK", backButtonX, 0xFF424242, 0xFF616161,
                                                [this]() { OnBackClicked(); }));
    }

    void ConnectionMenu::Update() {
        BaseMenu::Update();
        if (_nicknameInput) {
            _nicknameInput->Update();
        }
        if (_ipInput) {
            _ipInput->Update();
        }
        if (_portInput) {
            _portInput->Update();
        }
    }

    void ConnectionMenu::Render() {
        if (!_menu || !_menu->IsVisible()) {
            return;
        }

        // Render the inputs and buttons
        if (_nicknameInput) {
            _nicknameInput->Render();
        }
        if (_ipInput) {
            _ipInput->Render();
        }
        if (_portInput) {
            _portInput->Render();
        }

        BaseMenu::Render();

        // Render error message if present
        if (_hasError && !_errorMessage.empty()) {
            int screenWidth = _graphics.GetScreenWidth();

            const int fontSize = 16;
            const int errorMargin = 15;  // Spacing below buttons

            // Position error message below the buttons
            const int errorY = static_cast<int>(_buttonsY + _buttonHeight + errorMargin);

            // Estimate text width and center it
            int textWidth = static_cast<int>(_errorMessage.length() * fontSize * 0.6);
            int errorX = (screenWidth - textWidth) / 2;

            // Draw error message in red
            _graphics.DrawText(_errorMessage.c_str(), errorX, errorY, fontSize, 0xFFFF0000);
        }
    }

    void ConnectionMenu::OnJoinClicked() {
        std::string nickname = _nicknameInput ? _nicknameInput->GetText() : "";
        std::string ip = _ipInput ? _ipInput->GetText() : "";
        std::string port = _portInput ? _portInput->GetText() : "";

        // Validate inputs BEFORE applying defaults
        std::string errorMsg;

        // Validate nickname
        if (!ValidateNickname(nickname, errorMsg)) {
            _errorMessage = errorMsg;
            _hasError = true;
            _errorField = ErrorField::NICKNAME;
            if (_nicknameInput) {
                _nicknameInput->SetBorderColor(0xFFFF0000);  // Red border
                _nicknameInput->SetActiveBorderColor(0xFFFF0000);
            }
            return;
        }

        // Validate IP
        if (!ValidateIP(ip, errorMsg)) {
            _errorMessage = errorMsg;
            _hasError = true;
            _errorField = ErrorField::IP;
            if (_ipInput) {
                _ipInput->SetBorderColor(0xFFFF0000);  // Red border
                _ipInput->SetActiveBorderColor(0xFFFF0000);
            }
            return;
        }

        // Validate port
        if (!ValidatePort(port, errorMsg)) {
            _errorMessage = errorMsg;
            _hasError = true;
            _errorField = ErrorField::PORT;
            if (_portInput) {
                _portInput->SetBorderColor(0xFFFF0000);  // Red border
                _portInput->SetActiveBorderColor(0xFFFF0000);
            }
            return;
        }

        // All validations passed - now apply defaults if needed
        if (nickname.empty()) {
            nickname = "Player";
        }
        if (ip.empty()) {
            ip = "127.0.0.1";
        }
        if (port.empty()) {
            port = "4242";
        }

        LOG_INFO("[ConnectionMenu] Join clicked!");
        LOG_INFO("[ConnectionMenu] Nickname: " + nickname);
        LOG_INFO("[ConnectionMenu] IP: " + ip);
        LOG_INFO("[ConnectionMenu] Port: " + port);

        if (_onJoin) {
            _onJoin(nickname, ip, port);
        }
    }

    void ConnectionMenu::OnBackClicked() {
        LOG_INFO("[ConnectionMenu] Back button clicked!");
        if (_onBack) {
            _onBack();
        }
    }

    bool ConnectionMenu::ValidateNickname(const std::string &nickname, std::string &errorMsg) {
        if (nickname.empty()) {
            errorMsg = "Nickname cannot be empty";
            return false;
        }
        if (nickname.length() < 3) {
            errorMsg = "Nickname must be at least 3 characters";
            return false;
        }
        return true;
    }

    bool ConnectionMenu::ValidateIP(const std::string &ip, std::string &errorMsg) {
        if (ip.empty()) {
            errorMsg = "IP address cannot be empty";
            return false;
        }

        // Regex for valid IP: xxx.xxx.xxx.xxx where xxx is 0-255
        std::regex ipPattern(
            "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
            "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");

        if (!std::regex_match(ip, ipPattern)) {
            errorMsg = "Invalid IP address format (expected: xxx.xxx.xxx.xxx)";
            return false;
        }

        return true;
    }

    bool ConnectionMenu::ValidatePort(const std::string &port, std::string &errorMsg) {
        if (port.empty()) {
            errorMsg = "Port cannot be empty";
            return false;
        }

        // Check if all characters are digits
        for (char c : port) {
            if (!std::isdigit(c)) {
                errorMsg = "Port must contain only digits";
                return false;
            }
        }

        // Convert to int and check range
        try {
            int portNum = std::stoi(port);
            if (portNum < 1 || portNum > 65535) {
                errorMsg = "Port must be between 1 and 65535";
                return false;
            }
        } catch (...) {
            errorMsg = "Invalid port number";
            return false;
        }

        return true;
    }

    void ConnectionMenu::ClearError() {
        _hasError = false;
        _errorMessage.clear();

        // Reset border colors to normal
        if (_nicknameInput) {
            _nicknameInput->SetBorderColor(0xFF505050);
            _nicknameInput->SetActiveBorderColor(0xFF4CAF50);
        }
        if (_ipInput) {
            _ipInput->SetBorderColor(0xFF505050);
            _ipInput->SetActiveBorderColor(0xFF4CAF50);
        }
        if (_portInput) {
            _portInput->SetBorderColor(0xFF505050);
            _portInput->SetActiveBorderColor(0xFF4CAF50);
        }

        _errorField = ErrorField::NONE;
    }
}  // namespace Game
