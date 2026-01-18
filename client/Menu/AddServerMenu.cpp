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
        _nameInput = CreateInput(inputWidth, inputHeight, startY, "Server name (e.g., My Server)", 30,
                                 "[a-zA-Z0-9_ -]+", "Name");

        // --- IP Input ---
        float ipY = startY + inputHeight + spacing;
        _ipInput =
            CreateInput(inputWidth, inputHeight, ipY, "IP Address (e.g., 127.0.0.1)", 15, "[0-9.]+", "IP");

        // --- Port Input ---
        float portY = ipY + inputHeight + spacing;
        _portInput = CreateInput(inputWidth, inputHeight, portY, "Port (e.g., 4242)", 5, "[0-9]+", "Port");

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
            button->SetCallback(WrapWithClickSound(std::move(callback)));
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
        ClearInputs();
    }

    void AddServerMenu::OnCancelClicked() {
        LOG_INFO("[AddServerMenu] Cancel button clicked");

        // Clear inputs
        ClearInputs();

        if (_onCancel) {
            _onCancel();
        }
    }

    std::unique_ptr<UI::ITextInput> AddServerMenu::CreateInput(float width, float height, float yPos,
                                                               const std::string &placeholder, int maxLength,
                                                               const std::string &regex,
                                                               const std::string &logName) {
        auto input = _uiFactory.CreateTextInput();
        input->SetSize(width, height);
        input->SetAlign(UI::Align::CENTER_HORIZONTAL);
        input->ApplyAlignment();

        float x, y;
        input->GetPosition(x, y);
        input->SetPosition(x, yPos);

        input->SetPlaceholder(placeholder);
        input->SetMaxLength(maxLength);
        input->SetValidationRegex(regex);
        input->SetTextSize(18);
        input->SetBackgroundColor(0xFF2A2A2A);
        input->SetBorderColor(0xFF505050);
        input->SetActiveBorderColor(0xFF4CAF50);
        input->SetTextColor(0xFFFFFFFF);
        input->SetPlaceholderColor(0xFF808080);
        input->SetOnTextChanged([this, logName](const std::string &text) {
            LOG_INFO("[AddServerMenu] " + logName + ": " + text);
            ClearError();
        });

        return input;
    }

    void AddServerMenu::ClearInputs() {
        if (_nameInput)
            _nameInput->SetText("");
        if (_ipInput)
            _ipInput->SetText("");
        if (_portInput)
            _portInput->SetText("");
        ClearError();
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
