/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** AddServerMenu - Dialog for adding a new server to the list
*/

#include "Menu/AddServerMenu.hpp"
#include <cstring>
#include <regex>
#include <sstream>
#include "../common/Logger/Logger.hpp"

namespace Game {
    AddServerMenu::AddServerMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics)
        : BaseMenu(uiFactory), _graphics(graphics) {}

    void AddServerMenu::SetOnAdd(
        std::function<void(const std::string &, const std::string &, const std::string &)> onAdd) {
        _onAdd = std::move(onAdd);
    }

    void AddServerMenu::SetOnCancel(std::function<void()> onCancel) {
        _onCancel = std::move(onCancel);
    }

    void AddServerMenu::Initialize() {
        if (!_menu) {
            return;
        }

        const float inputWidth = 350.0f;
        const float inputHeight = 40.0f;
        const float spacing = 20.0f;
        const float buttonWidth = 140.0f;
        const float buttonHeight = 45.0f;
        const float buttonSpacing = 20.0f;

        const float startY = 150.0f;  // Start from top with margin

        // Clear menu and create text inputs
        _menu->Clear();

        // --- Server Name Input ---
        _nameInput = _uiFactory.CreateTextInput();
        _nameInput->SetSize(inputWidth, inputHeight);
        _nameInput->SetAlign(UI::Align::CENTER_HORIZONTAL);
        _nameInput->ApplyAlignment();

        float x, y;
        _nameInput->GetPosition(x, y);
        _nameInput->SetPosition(x, startY);

        _nameInput->SetPlaceholder("Server name (e.g., My Server)");
        _nameInput->SetMaxLength(30);
        _nameInput->SetValidationRegex("[a-zA-Z0-9_ -]+");
        _nameInput->SetTextSize(18);
        _nameInput->SetBackgroundColor(0xFF2A2A2A);
        _nameInput->SetBorderColor(0xFF505050);
        _nameInput->SetActiveBorderColor(0xFF4CAF50);
        _nameInput->SetTextColor(0xFFFFFFFF);
        _nameInput->SetPlaceholderColor(0xFF808080);
        _nameInput->SetOnTextChanged([this](const std::string &text) {
            LOG_INFO("[AddServerMenu] Name: " + text);
            ClearError();
        });

        // --- IP Input ---
        _ipInput = _uiFactory.CreateTextInput();
        _ipInput->SetSize(inputWidth, inputHeight);
        _ipInput->SetAlign(UI::Align::CENTER_HORIZONTAL);
        _ipInput->ApplyAlignment();

        _ipInput->GetPosition(x, y);
        float ipY = startY + inputHeight + spacing;
        _ipInput->SetPosition(x, ipY);

        _ipInput->SetPlaceholder("IP Address (e.g., 127.0.0.1)");
        _ipInput->SetMaxLength(15);
        _ipInput->SetValidationRegex("[0-9.]+");
        _ipInput->SetTextSize(18);
        _ipInput->SetBackgroundColor(0xFF2A2A2A);
        _ipInput->SetBorderColor(0xFF505050);
        _ipInput->SetActiveBorderColor(0xFF4CAF50);
        _ipInput->SetTextColor(0xFFFFFFFF);
        _ipInput->SetPlaceholderColor(0xFF808080);
        _ipInput->SetOnTextChanged([this](const std::string &text) {
            LOG_INFO("[AddServerMenu] IP: " + text);
            ClearError();
        });

        // --- Port Input ---
        _portInput = _uiFactory.CreateTextInput();
        _portInput->SetSize(inputWidth, inputHeight);
        _portInput->SetAlign(UI::Align::CENTER_HORIZONTAL);
        _portInput->ApplyAlignment();

        _portInput->GetPosition(x, y);
        float portY = ipY + inputHeight + spacing;
        _portInput->SetPosition(x, portY);

        _portInput->SetPlaceholder("Port (e.g., 4242)");
        _portInput->SetMaxLength(5);
        _portInput->SetValidationRegex("[0-9]+");
        _portInput->SetTextSize(18);
        _portInput->SetBackgroundColor(0xFF2A2A2A);
        _portInput->SetBorderColor(0xFF505050);
        _portInput->SetActiveBorderColor(0xFF4CAF50);
        _portInput->SetTextColor(0xFFFFFFFF);
        _portInput->SetPlaceholderColor(0xFF808080);
        _portInput->SetOnTextChanged([this](const std::string &text) {
            LOG_INFO("[AddServerMenu] Port: " + text);
            ClearError();
        });

        // --- Buttons (Add and Cancel) ---
        _buttonsY = portY + inputHeight + spacing * 2;
        _buttonHeight = buttonHeight;
        float totalButtonWidth = buttonWidth * 2 + buttonSpacing;
        float addButtonX = -(totalButtonWidth / 2.0f);
        float cancelButtonX = addButtonX + buttonWidth + buttonSpacing;

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

        _menu->AddButton(
            createHorizontalButton("ADD", addButtonX, 0xFF4CAF50, 0xFF66BB6A, [this]() { OnAddClicked(); }));

        _menu->AddButton(createHorizontalButton("CANCEL", cancelButtonX, 0xFF424242, 0xFF616161,
                                                [this]() { OnCancelClicked(); }));
    }

    void AddServerMenu::Update() {
        BaseMenu::Update();
        if (_nameInput) {
            _nameInput->Update();
        }
        if (_ipInput) {
            _ipInput->Update();
        }
        if (_portInput) {
            _portInput->Update();
        }
    }

    void AddServerMenu::Render() {
        if (!_menu || !_menu->IsVisible()) {
            return;
        }

        // Draw title
        const char *title = "Add New Server";
        int screenWidth = _graphics.GetScreenWidth();
        int titleFontSize = 28;
        int titleWidth = static_cast<int>(strlen(title) * titleFontSize * 0.5f);
        int titleX = (screenWidth - titleWidth) / 2;
        _graphics.DrawText(title, titleX, 80, titleFontSize, 0xFFFFFFFF);

        // Render the inputs and buttons
        if (_nameInput) {
            _nameInput->Render();
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
            const int fontSize = 16;
            const int errorMargin = 15;

            const int errorY = static_cast<int>(_buttonsY + _buttonHeight + errorMargin);

            int textWidth = static_cast<int>(_errorMessage.length() * fontSize * 0.6);
            int errorX = (screenWidth - textWidth) / 2;

            _graphics.DrawText(_errorMessage.c_str(), errorX, errorY, fontSize, 0xFFFF0000);
        }
    }

    void AddServerMenu::OnAddClicked() {
        LOG_INFO("[AddServerMenu] Add button clicked");

        std::string name = _nameInput ? _nameInput->GetText() : "";
        std::string ip = _ipInput ? _ipInput->GetText() : "";
        std::string port = _portInput ? _portInput->GetText() : "";

        std::string errorMsg;

        // Validate all fields
        if (!ValidateName(name, errorMsg)) {
            _errorMessage = errorMsg;
            _errorField = ErrorField::NAME;
            _hasError = true;
            return;
        }

        if (!ValidateIP(ip, errorMsg)) {
            _errorMessage = errorMsg;
            _errorField = ErrorField::IP;
            _hasError = true;
            return;
        }

        if (!ValidatePort(port, errorMsg)) {
            _errorMessage = errorMsg;
            _errorField = ErrorField::PORT;
            _hasError = true;
            return;
        }

        // All valid - trigger callback
        LOG_INFO("[AddServerMenu] Server validated: ", name, " - ", ip, ":", port);
        if (_onAdd) {
            _onAdd(name, ip, port);
        }

        // Clear inputs for next time
        if (_nameInput)
            _nameInput->SetText("");
        if (_ipInput)
            _ipInput->SetText("");
        if (_portInput)
            _portInput->SetText("");
        ClearError();
    }

    void AddServerMenu::OnCancelClicked() {
        LOG_INFO("[AddServerMenu] Cancel button clicked");

        // Clear inputs
        if (_nameInput)
            _nameInput->SetText("");
        if (_ipInput)
            _ipInput->SetText("");
        if (_portInput)
            _portInput->SetText("");
        ClearError();

        if (_onCancel) {
            _onCancel();
        }
    }

    bool AddServerMenu::ValidateName(const std::string &name, std::string &errorMsg) {
        if (name.empty()) {
            errorMsg = "Server name cannot be empty";
            return false;
        }
        if (name.length() < 2) {
            errorMsg = "Server name must be at least 2 characters";
            return false;
        }
        return true;
    }

    bool AddServerMenu::ValidateIP(const std::string &ip, std::string &errorMsg) {
        if (ip.empty()) {
            errorMsg = "IP address cannot be empty";
            return false;
        }

        // Basic IP validation (simplified)
        std::regex ipRegex(R"(^(\d{1,3}\.){3}\d{1,3}$)");
        if (!std::regex_match(ip, ipRegex)) {
            errorMsg = "Invalid IP address format";
            return false;
        }

        // Check each octet is 0-255
        std::istringstream iss(ip);
        std::string octet;
        while (std::getline(iss, octet, '.')) {
            try {
                int value = std::stoi(octet);
                if (value < 0 || value > 255) {
                    errorMsg = "IP octets must be between 0 and 255";
                    return false;
                }
            } catch (...) {
                errorMsg = "Invalid IP address";
                return false;
            }
        }

        return true;
    }

    bool AddServerMenu::ValidatePort(const std::string &port, std::string &errorMsg) {
        if (port.empty()) {
            errorMsg = "Port cannot be empty";
            return false;
        }

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

    void AddServerMenu::ClearError() {
        _hasError = false;
        _errorMessage.clear();
        _errorField = ErrorField::NONE;
    }
}  // namespace Game
