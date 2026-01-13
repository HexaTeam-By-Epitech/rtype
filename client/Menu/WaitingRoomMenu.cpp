/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** WaitingRoomMenu - Menu for waiting before game starts
*/

#include "Menu/WaitingRoomMenu.hpp"
#include <cstring>
#include <sstream>
#include "../common/Logger/Logger.hpp"

namespace Game {
    WaitingRoomMenu::WaitingRoomMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics)
        : BaseMenu(uiFactory), _graphics(graphics) {}

    void WaitingRoomMenu::SetOnStartGame(std::function<void()> onStartGame) {
        _onStartGame = std::move(onStartGame);
    }

    void WaitingRoomMenu::SetOnBack(std::function<void()> onBack) {
        _onBack = std::move(onBack);
    }

    void WaitingRoomMenu::UpdatePlayerList(const std::vector<PlayerInfo> &players) {
        _players = players;
        RebuildPlayerList();
    }

    void WaitingRoomMenu::SetRoomInfo(const std::string &roomName, uint32_t currentPlayers,
                                      uint32_t maxPlayers) {
        _roomName = roomName;
        _currentPlayers = currentPlayers;
        _maxPlayers = maxPlayers;
    }

    void WaitingRoomMenu::SetIsHost(bool isHost) {
        _isHost = isHost;
        // Re-initialize menu to show/hide Start Game button based on host status
        Initialize();
    }

    void WaitingRoomMenu::Initialize() {
        if (!_menu) {
            return;
        }

        _menu->Clear();

        float screenWidth = static_cast<float>(_graphics.GetScreenWidth());
        float screenHeight = static_cast<float>(_graphics.GetScreenHeight());

        // Add "Start Game" button (host only, bottom right)
        const float buttonWidth = 180.0f;
        const float buttonHeight = 50.0f;
        const float margin = 20.0f;

        // Only add Start Game button if this player is the host
        if (_isHost) {
            auto startGamePtr = _uiFactory.CreateButton();
            startGamePtr->SetSize(buttonWidth, buttonHeight);
            startGamePtr->SetPosition(screenWidth - margin - buttonWidth,
                                      screenHeight - margin - buttonHeight);
            startGamePtr->SetText("START GAME");
            startGamePtr->SetBackgroundColor(0xFF4CAF50);  // Green
            startGamePtr->SetHoverColor(0xFF66BB6A);
            startGamePtr->SetTextColor(0xFFFFFFFF);
            startGamePtr->SetCallback([this]() { OnStartGameClicked(); });

            _startGameButton = std::move(startGamePtr);
            _menu->AddButton(_startGameButton);
        }

        // Add "Back" button (bottom left)
        auto backPtr = _uiFactory.CreateButton();
        backPtr->SetSize(buttonWidth, buttonHeight);
        backPtr->SetPosition(margin, screenHeight - margin - buttonHeight);
        backPtr->SetText("BACK");
        backPtr->SetBackgroundColor(0xFF424242);  // Dark gray
        backPtr->SetHoverColor(0xFF616161);
        backPtr->SetTextColor(0xFFFFFFFF);
        backPtr->SetCallback([this]() { OnBackClicked(); });

        _backButton = std::move(backPtr);
        _menu->AddButton(_backButton);

        RebuildPlayerList();
    }

    void WaitingRoomMenu::Update() {
        if (!_menu || !_menu->IsVisible()) {
            return;
        }
        BaseMenu::Update();
    }

    void WaitingRoomMenu::Render() {
        if (!_menu || !_menu->IsVisible()) {
            return;
        }

        int screenWidth = _graphics.GetScreenWidth();

        // Draw title
        const char *title = "Waiting for Game Start";
        int titleFontSize = 36;
        int titleWidth = static_cast<int>(strlen(title) * titleFontSize * 0.5f);
        int titleX = (screenWidth - titleWidth) / 2;
        _graphics.DrawText(title, titleX, 30, titleFontSize, 0xFFFFFFFF);

        // Draw room info
        std::string roomInfo = _roomName + " [" + std::to_string(_currentPlayers) + "/" +
                               std::to_string(_maxPlayers) + " players]";
        int infoFontSize = 20;
        int infoWidth = static_cast<int>(roomInfo.length() * infoFontSize * 0.5f);
        int infoX = (screenWidth - infoWidth) / 2;
        _graphics.DrawText(roomInfo.c_str(), infoX, 80, infoFontSize, 0xFFAAAAAA);

        // Draw "Players:" label
        _graphics.DrawText("Players:", 50, 130, 22, 0xFFFFFFFF);

        // Draw player list
        float playerY = 160.0f;
        int playerFontSize = 18;
        int playerBoxHeight = 40;
        int playerBoxWidth = 400;

        for (size_t i = 0; i < _players.size(); ++i) {
            const auto &player = _players[i];

            // Determine color: yellow for host, white for others
            uint32_t backgroundColor =
                player.isHost ? 0xFFFFD700 : 0xFF505050;  // Gold for host, gray for others
            uint32_t textColor =
                player.isHost ? 0xFF000000 : 0xFFFFFFFF;  // Black text on gold, white on gray

            // Draw player box
            _graphics.DrawRectFilled(50, static_cast<int>(playerY), playerBoxWidth, playerBoxHeight,
                                     backgroundColor);
            _graphics.DrawRect(50, static_cast<int>(playerY), playerBoxWidth, playerBoxHeight, 0xFF888888);

            // Draw player name
            std::string playerLabel = player.playerName;
            if (player.isHost) {
                playerLabel += " (HOST)";
            }
            if (player.isSpectator) {
                playerLabel += " [SPECTATOR]";
            }

            _graphics.DrawText(playerLabel.c_str(), 60, static_cast<int>(playerY) + 10, playerFontSize,
                               textColor);

            playerY += playerBoxHeight + 10;
        }

        BaseMenu::Render();
    }

    void WaitingRoomMenu::RebuildPlayerList() {
        if (!_menu) {
            return;
        }

        // Don't clear entire menu, just rebuild player display
        // (buttons are persistent)

        // Players are drawn in Render() method as text, not as UI buttons
    }

    void WaitingRoomMenu::OnStartGameClicked() {
        LOG_INFO("[WaitingRoomMenu] Start Game button clicked");
        if (_onStartGame) {
            _onStartGame();
        }
    }

    void WaitingRoomMenu::OnBackClicked() {
        LOG_INFO("[WaitingRoomMenu] Back button clicked");
        if (_onBack) {
            _onBack();
        }
    }

}  // namespace Game
