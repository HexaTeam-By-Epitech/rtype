/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ServerListMenu - Menu for selecting a server from saved list
*/

#include "Menu/ServerListMenu.hpp"
#include <cstring>
#include <sstream>
#include "../common/Logger/Logger.hpp"

namespace Game {
    ServerListMenu::ServerListMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics)
        : BaseMenu(uiFactory), _graphics(graphics) {
        // Add default local server
        _servers.emplace_back("Local Server", "127.0.0.1", 4242);
    }

    void ServerListMenu::SetOnServerSelected(
        std::function<void(const std::string &, uint16_t)> onServerSelected) {
        _onServerSelected = std::move(onServerSelected);
    }

    void ServerListMenu::SetOnAddServer(std::function<void()> onAddServer) {
        _onAddServer = std::move(onAddServer);
    }

    void ServerListMenu::SetOnBack(std::function<void()> onBack) {
        _onBack = std::move(onBack);
    }

    void ServerListMenu::AddServer(const std::string &name, const std::string &ip, uint16_t port) {
        if (_servers.size() >= MAX_SERVERS) {
            LOG_WARNING("[ServerListMenu] Cannot add server: maximum limit reached (", MAX_SERVERS, ")");
            return;
        }
        _servers.emplace_back(name, ip, port);
        RebuildServerList();
        UpdateAddServerButtonState();
    }

    void ServerListMenu::RemoveServer(size_t index) {
        if (index >= _servers.size()) {
            LOG_ERROR("[ServerListMenu] Cannot remove server: invalid index ", index);
            return;
        }

        const auto &server = _servers[index];
        LOG_INFO("[ServerListMenu] Removing server: ", server.name, " (", server.ip, ":", server.port, ")");

        _servers.erase(_servers.begin() + static_cast<long>(index));
        RebuildServerList();
        UpdateAddServerButtonState();
    }

    void ServerListMenu::Initialize() {
        if (!_menu) {
            return;
        }

        _menu->Clear();
        _serverButtons.clear();

        RebuildServerList();

        // Add "Add Server" button at bottom center
        const float buttonWidth = 200.0f;
        const float buttonHeight = 45.0f;
        const float bottomMargin = 120.0f;

        auto addServerPtr = _uiFactory.CreateButton();
        addServerPtr->SetSize(buttonWidth, buttonHeight);
        addServerPtr->SetAlign(UI::Align::CENTER_HORIZONTAL);
        addServerPtr->ApplyAlignment();

        float x, y;
        addServerPtr->GetPosition(x, y);
        float screenHeight = static_cast<float>(_graphics.GetScreenHeight());
        addServerPtr->SetPosition(x, screenHeight - bottomMargin);

        addServerPtr->SetText("Add Server");
        addServerPtr->SetBackgroundColor(0xFF4CAF50);  // Green
        addServerPtr->SetHoverColor(0xFF66BB6A);
        addServerPtr->SetTextColor(0xFFFFFFFF);
        addServerPtr->SetCallback([this]() { OnAddServerClicked(); });

        _addServerButton = std::move(addServerPtr);
        _menu->AddButton(_addServerButton);

        // Add "Exit" button at bottom left
        const float exitMargin = 20.0f;
        auto exitPtr = _uiFactory.CreateButton();
        exitPtr->SetSize(150.0f, 45.0f);
        exitPtr->SetPosition(exitMargin, screenHeight - bottomMargin);
        exitPtr->SetText("Exit");
        exitPtr->SetBackgroundColor(0xFFF44336);  // Red
        exitPtr->SetHoverColor(0xFFE57373);
        exitPtr->SetTextColor(0xFFFFFFFF);
        exitPtr->SetCallback([this]() { OnBackClicked(); });

        _backButton = std::move(exitPtr);
        _menu->AddButton(_backButton);

        UpdateAddServerButtonState();
    }

    void ServerListMenu::RebuildServerList() {
        if (!_menu) {
            return;
        }

        // Clear the entire menu and rebuild everything
        _menu->Clear();
        _serverButtons.clear();
        _deleteButtons.clear();

        // Button dimensions
        const float deleteButtonWidth = 60.0f;
        const float buttonSpacing = 10.0f;
        const float serverButtonWidth = SERVER_BUTTON_WIDTH - deleteButtonWidth - buttonSpacing;

        // Create buttons for each server
        float currentY = LIST_START_Y;
        for (size_t i = 0; i < _servers.size() && i < MAX_VISIBLE_SERVERS; ++i) {
            const auto &server = _servers[i];

            // Server button (select)
            auto buttonPtr = _uiFactory.CreateButton();
            buttonPtr->SetSize(serverButtonWidth, SERVER_BUTTON_HEIGHT);
            buttonPtr->SetAlign(UI::Align::CENTER_HORIZONTAL);
            buttonPtr->ApplyAlignment();

            float x, y;
            buttonPtr->GetPosition(x, y);
            // Adjust position to make room for delete button
            float adjustedX = x - (deleteButtonWidth + buttonSpacing) / 2.0f;
            buttonPtr->SetPosition(adjustedX, currentY);

            // Button text: "Server Name - IP:Port"
            std::ostringstream oss;
            oss << server.name << " - " << server.ip << ":" << server.port;
            buttonPtr->SetText(oss.str());

            buttonPtr->SetBackgroundColor(0xFF2196F3);  // Blue
            buttonPtr->SetHoverColor(0xFF64B5F6);
            buttonPtr->SetTextColor(0xFFFFFFFF);
            buttonPtr->SetTextSize(16);

            size_t index = i;  // Capture by value for lambda
            buttonPtr->SetCallback([this, index]() { OnServerClicked(index); });

            std::shared_ptr<UI::IButton> button = std::move(buttonPtr);
            _serverButtons.push_back(button);
            _menu->AddButton(button);

            // Delete button
            auto deletePtr = _uiFactory.CreateButton();
            deletePtr->SetSize(deleteButtonWidth, SERVER_BUTTON_HEIGHT);
            float deleteX = adjustedX + serverButtonWidth + buttonSpacing;
            deletePtr->SetPosition(deleteX, currentY);
            deletePtr->SetText("x");
            deletePtr->SetBackgroundColor(0xFFF44336);  // Red
            deletePtr->SetHoverColor(0xFFE57373);
            deletePtr->SetTextColor(0xFFFFFFFF);
            deletePtr->SetTextSize(20);
            deletePtr->SetCallback([this, index]() { OnDeleteServerClicked(index); });

            std::shared_ptr<UI::IButton> deleteButton = std::move(deletePtr);
            _deleteButtons.push_back(deleteButton);
            _menu->AddButton(deleteButton);

            currentY += SERVER_BUTTON_HEIGHT + SERVER_BUTTON_SPACING;
        }

        // Re-add "Add Server" button at bottom center
        if (_addServerButton) {
            _menu->AddButton(_addServerButton);
        }

        // Re-add "Exit" button at bottom left
        if (_backButton) {
            _menu->AddButton(_backButton);
        }
    }

    void ServerListMenu::Update() {
        BaseMenu::Update();
    }

    void ServerListMenu::Render() {
        if (!_menu || !_menu->IsVisible()) {
            return;
        }

        // Draw title
        const char *title = "Select a Server";
        int screenWidth = _graphics.GetScreenWidth();
        int titleFontSize = 32;
        int titleWidth = static_cast<int>(strlen(title) * titleFontSize * 0.5f);
        int titleX = (screenWidth - titleWidth) / 2;
        _graphics.DrawText(title, titleX, 30, titleFontSize, 0xFFFFFFFF);

        // Draw connecting message if connecting
        if (_isConnecting) {
            std::string connectingMsg = "Connecting to " + _connectingServerName + "...";
            int connectingFontSize = 20;
            int connectingY = 70;
            int connectingWidth = static_cast<int>(connectingMsg.length() * connectingFontSize * 0.5f);
            int connectingX = (screenWidth - connectingWidth) / 2;
            _graphics.DrawText(connectingMsg.c_str(), connectingX, connectingY, connectingFontSize,
                               0xFF00FF00);  // Green
        }
        // Draw error message if present (and not connecting)
        else if (_hasConnectionError && !_connectionError.empty()) {
            int errorFontSize = 20;
            int errorY = 70;  // Below title
            int errorWidth = static_cast<int>(_connectionError.length() * errorFontSize * 0.5f);
            int errorX = (screenWidth - errorWidth) / 2;
            _graphics.DrawText(_connectionError.c_str(), errorX, errorY, errorFontSize, 0xFFFF0000);  // Red
        }

        BaseMenu::Render();
    }

    void ServerListMenu::OnServerClicked(size_t index) {
        if (index >= _servers.size()) {
            LOG_ERROR("[ServerListMenu] Invalid server index: ", index);
            return;
        }

        // Clear previous error
        ClearConnectionError();

        const auto &server = _servers[index];
        LOG_INFO("[ServerListMenu] Selected server: ", server.name, " (", server.ip, ":", server.port, ")");

        if (_onServerSelected) {
            _onServerSelected(server.ip, server.port);
        }
    }

    void ServerListMenu::OnAddServerClicked() {
        LOG_INFO("[ServerListMenu] Add Server button clicked");
        if (_onAddServer) {
            _onAddServer();
        }
    }

    void ServerListMenu::OnBackClicked() {
        LOG_INFO("[ServerListMenu] Back button clicked");
        if (_onBack) {
            _onBack();
        }
    }

    void ServerListMenu::OnDeleteServerClicked(size_t index) {
        LOG_INFO("[ServerListMenu] Delete button clicked for server index: ", index);
        RemoveServer(index);
    }

    void ServerListMenu::UpdateAddServerButtonState() {
        if (!_addServerButton) {
            return;
        }

        if (_servers.size() >= MAX_SERVERS) {
            // Disable button when limit reached
            _addServerButton->SetBackgroundColor(0xFF757575);  // Gray
            _addServerButton->SetHoverColor(0xFF757575);       // Same gray (no hover effect)
            _addServerButton->SetCallback(
                []() { LOG_WARNING("[ServerListMenu] Cannot add server: maximum limit reached"); });
        } else {
            // Enable button
            _addServerButton->SetBackgroundColor(0xFF4CAF50);  // Green
            _addServerButton->SetHoverColor(0xFF66BB6A);
            _addServerButton->SetCallback([this]() { OnAddServerClicked(); });
        }
    }

    void ServerListMenu::SetConnectionError(const std::string &errorMsg) {
        _connectionError = errorMsg;
        _hasConnectionError = true;
        LOG_ERROR("[ServerListMenu] Connection error: ", errorMsg);
    }

    void ServerListMenu::ClearConnectionError() {
        _connectionError.clear();
        _hasConnectionError = false;
    }

    void ServerListMenu::SetConnecting(bool connecting, const std::string &serverName) {
        _isConnecting = connecting;
        _connectingServerName = serverName;
        if (connecting) {
            // Clear error when starting to connect
            ClearConnectionError();
        }
    }
}  // namespace Game
