/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** CreateRoomMenu - Menu for creating a new room
*/

#include "Menu/CreateRoomMenu.hpp"
#include <cstring>
#include "../common/Logger/Logger.hpp"

namespace Game {
    CreateRoomMenu::CreateRoomMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics)
        : BaseMenu(uiFactory), _graphics(graphics) {}

    void CreateRoomMenu::SetOnCreate(
        std::function<void(const std::string &, uint32_t, bool, float)> onCreate) {
        _onCreate = std::move(onCreate);
    }

    void CreateRoomMenu::SetOnCancel(std::function<void()> onCancel) {
        _onCancel = std::move(onCancel);
    }

    void CreateRoomMenu::Initialize() {
        if (!_menu) {
            return;
        }

        _menu->Clear();

        float screenWidth = static_cast<float>(_graphics.GetScreenWidth());
        float centerX = screenWidth / 2.0f;

        // Room Name Input
        _roomNameInput = _uiFactory.CreateTextInput();
        _roomNameInput->SetPosition(centerX - 200.0f, 150.0f);
        _roomNameInput->SetSize(400.0f, 40.0f);
        _roomNameInput->SetPlaceholder("Enter room name (3-30 characters)");
        _roomNameInput->SetMaxLength(30);
        _roomNameInput->SetValidationRegex("[a-zA-Z0-9_ -]+");
        _roomNameInput->SetTextSize(18);
        _roomNameInput->SetBackgroundColor(0xFF2A2A2A);
        _roomNameInput->SetBorderColor(0xFF505050);
        _roomNameInput->SetActiveBorderColor(0xFF4CAF50);
        _roomNameInput->SetTextColor(0xFFFFFFFF);
        _roomNameInput->SetPlaceholderColor(0xFF808080);
        _roomNameInput->SetOnTextChanged([this](const std::string &) {
            // Clear error when user types
            _errorMessage.clear();
        });

        // Max Players Toggle Button
        _maxPlayersButton = _uiFactory.CreateButton();
        _maxPlayersButton->SetPosition(centerX - 200.0f, 220.0f);
        _maxPlayersButton->SetSize(400.0f, 40.0f);
        _maxPlayersButton->SetText("Max Players: 4");
        _maxPlayersButton->SetBackgroundColor(0xFF2196F3);
        _maxPlayersButton->SetHoverColor(0xFF64B5F6);
        _maxPlayersButton->SetTextColor(0xFFFFFFFF);
        _maxPlayersButton->SetCallback(WrapWithClickSound([this]() {
            // Cycle through 2, 4, 8 players
            if (_selectedMaxPlayers == 4) {
                _selectedMaxPlayers = 8;
            } else if (_selectedMaxPlayers == 8) {
                _selectedMaxPlayers = 2;
            } else {
                _selectedMaxPlayers = 4;
            }
            _maxPlayersButton->SetText("Max Players: " + std::to_string(_selectedMaxPlayers));
        }));
        _menu->AddButton(_maxPlayersButton);

        // Private Toggle Button
        _privateButton = _uiFactory.CreateButton();
        _privateButton->SetPosition(centerX - 200.0f, 280.0f);
        _privateButton->SetSize(400.0f, 40.0f);
        _privateButton->SetText("Private: No");
        _privateButton->SetBackgroundColor(0xFF9E9E9E);
        _privateButton->SetHoverColor(0xFFBDBDBD);
        _privateButton->SetTextColor(0xFFFFFFFF);
        _privateButton->SetCallback(WrapWithClickSound([this]() {
            _isPrivate = !_isPrivate;
            _privateButton->SetText(_isPrivate ? "Private: Yes" : "Private: No");
            _privateButton->SetBackgroundColor(_isPrivate ? 0xFFFFA726 : 0xFF9E9E9E);
            _privateButton->SetHoverColor(_isPrivate ? 0xFFFFB74D : 0xFFBDBDBD);
        }));
        _menu->AddButton(_privateButton);

        // Game Speed Button (cycles through speed options: 100%, 75%, 50%, 25%)
        _gameSpeedButton = _uiFactory.CreateButton();
        _gameSpeedButton->SetPosition(centerX - 200.0f, 340.0f);
        _gameSpeedButton->SetSize(400.0f, 40.0f);
        UpdateSpeedButtonText();
        _gameSpeedButton->SetBackgroundColor(0xFFFF9800);  // Orange for speed
        _gameSpeedButton->SetHoverColor(0xFFFFB74D);
        _gameSpeedButton->SetTextColor(0xFFFFFFFF);
        _gameSpeedButton->SetCallback([this]() {
            // Cycle through: 100% -> 75% -> 50% -> 25% -> 100%
            if (_gameSpeedMultiplier >= 1.0f) {
                _gameSpeedMultiplier = 0.75f;
            } else if (_gameSpeedMultiplier >= 0.75f) {
                _gameSpeedMultiplier = 0.50f;
            } else if (_gameSpeedMultiplier >= 0.50f) {
                _gameSpeedMultiplier = 0.25f;
            } else {
                _gameSpeedMultiplier = 1.0f;
            }
            UpdateSpeedButtonText();
        });
        _menu->AddButton(_gameSpeedButton);

        // Create Button
        _createButton = _uiFactory.CreateButton();
        _createButton->SetPosition(centerX - 100.0f, 420.0f);
        _createButton->SetSize(90.0f, 40.0f);
        _createButton->SetText("CREATE");
        _createButton->SetBackgroundColor(0xFF4CAF50);
        _createButton->SetHoverColor(0xFF66BB6A);
        _createButton->SetTextColor(0xFFFFFFFF);
        _createButton->SetCallback(WrapWithClickSound([this]() { OnCreateClicked(); }));
        _menu->AddButton(_createButton);

        // Cancel Button
        _cancelButton = _uiFactory.CreateButton();
        _cancelButton->SetPosition(centerX + 10.0f, 420.0f);
        _cancelButton->SetSize(90.0f, 40.0f);
        _cancelButton->SetText("CANCEL");
        _cancelButton->SetBackgroundColor(0xFF424242);
        _cancelButton->SetHoverColor(0xFF616161);
        _cancelButton->SetTextColor(0xFFFFFFFF);
        _cancelButton->SetCallback(WrapWithClickSound([this]() { OnCancelClicked(); }));
        _menu->AddButton(_cancelButton);
    }

    void CreateRoomMenu::Update() {
        if (!_menu || !_menu->IsVisible()) {
            return;
        }

        // Update text input
        if (_roomNameInput) {
            _roomNameInput->Update();
        }

        BaseMenu::Update();
    }

    void CreateRoomMenu::Render() {
        if (!_menu || !_menu->IsVisible()) {
            return;
        }

        // Draw title
        const char *title = "Create New Room";
        int screenWidth = _graphics.GetScreenWidth();
        int titleFontSize = 32;
        int titleWidth = static_cast<int>(strlen(title) * titleFontSize * 0.5f);
        int titleX = (screenWidth - titleWidth) / 2;
        _graphics.DrawText(title, titleX, 50, titleFontSize, 0xFFFFFFFF);

        // Render text input
        if (_roomNameInput) {
            _roomNameInput->Render();
        }

        // Draw error message if present
        if (!_errorMessage.empty()) {
            int errorFontSize = 18;
            int errorWidth = static_cast<int>(_errorMessage.length() * errorFontSize * 0.5f);
            int errorX = (screenWidth - errorWidth) / 2;
            _graphics.DrawText(_errorMessage.c_str(), errorX, 480, errorFontSize, 0xFFFF0000);
        }

        BaseMenu::Render();
    }

    bool CreateRoomMenu::ValidateInput() {
        std::string roomName = _roomNameInput->GetText();

        if (roomName.empty()) {
            _errorMessage = "Room name cannot be empty!";
            return false;
        }

        if (roomName.length() < 3) {
            _errorMessage = "Room name must be at least 3 characters!";
            return false;
        }

        if (roomName.length() > 30) {
            _errorMessage = "Room name must be less than 30 characters!";
            return false;
        }

        _errorMessage.clear();
        return true;
    }

    void CreateRoomMenu::OnCreateClicked() {
        LOG_INFO("[CreateRoomMenu] Create button clicked");

        if (!ValidateInput()) {
            LOG_WARNING("[CreateRoomMenu] Validation failed: ", _errorMessage);
            return;
        }

        std::string roomName = _roomNameInput->GetText();
        LOG_INFO("[CreateRoomMenu] Creating room: ", roomName, " (Max: ", _selectedMaxPlayers,
                 ", Private: ", _isPrivate ? "Yes" : "No",
                 ", Speed: ", static_cast<int>(_gameSpeedMultiplier * 100), "%)");

        if (_onCreate) {
            _onCreate(roomName, _selectedMaxPlayers, _isPrivate, _gameSpeedMultiplier);
        }

        // Reset form
        _roomNameInput->SetText("");
        _selectedMaxPlayers = 4;
        _isPrivate = false;
        _gameSpeedMultiplier = 1.0f;
        _maxPlayersButton->SetText("Max Players: 4");
        _privateButton->SetText("Private: No");
        _privateButton->SetBackgroundColor(0xFF9E9E9E);
        _privateButton->SetHoverColor(0xFFBDBDBD);
        UpdateSpeedButtonText();
        _errorMessage.clear();
    }

    void CreateRoomMenu::OnCancelClicked() {
        LOG_INFO("[CreateRoomMenu] Cancel button clicked");

        // Reset form
        _roomNameInput->SetText("");
        _gameSpeedMultiplier = 1.0f;
        UpdateSpeedButtonText();
        _errorMessage.clear();

        if (_onCancel) {
            _onCancel();
        }
    }

    void CreateRoomMenu::UpdateSpeedButtonText() {
        int speedPercent = static_cast<int>(_gameSpeedMultiplier * 100);
        std::string speedText = "Game Speed: " + std::to_string(speedPercent) + "%";
        if (speedPercent < 100) {
            speedText += " (Accessibility)";
        }
        if (_gameSpeedButton) {
            _gameSpeedButton->SetText(speedText);
        }
    }
}  // namespace Game
