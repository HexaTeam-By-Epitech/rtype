/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RoomListMenu - Menu for selecting or creating a room
*/

#include "Menu/RoomListMenu.hpp"
#include <cstring>
#include <sstream>
#include "../common/Logger/Logger.hpp"

namespace Game {
    RoomListMenu::RoomListMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics)
        : BaseMenu(uiFactory), _graphics(graphics) {}

    void RoomListMenu::SetOnRoomSelected(std::function<void(const std::string &)> onRoomSelected) {
        _onRoomSelected = std::move(onRoomSelected);
    }

    void RoomListMenu::SetOnCreateRoom(std::function<void()> onCreateRoom) {
        _onCreateRoom = std::move(onCreateRoom);
    }

    void RoomListMenu::SetOnRefresh(std::function<void()> onRefresh) {
        _onRefresh = std::move(onRefresh);
    }

    void RoomListMenu::SetOnBack(std::function<void()> onBack) {
        _onBack = std::move(onBack);
    }

    void RoomListMenu::UpdateRoomList(const std::vector<RoomInfo> &rooms) {
        _rooms = rooms;
        RebuildRoomList();
    }

    void RoomListMenu::ClearRooms() {
        _rooms.clear();
        RebuildRoomList();
    }

    void RoomListMenu::Initialize() {
        if (!_menu) {
            return;
        }

        _menu->Clear();
        _roomButtons.clear();

        RebuildRoomList();

        // Add "Create Room" button at bottom center
        const float buttonWidth = 180.0f;
        const float buttonHeight = 45.0f;
        const float bottomMargin = 120.0f;

        auto createRoomPtr = _uiFactory.CreateButton();
        createRoomPtr->SetSize(buttonWidth, buttonHeight);
        createRoomPtr->SetAlign(UI::Align::CENTER_HORIZONTAL);
        createRoomPtr->ApplyAlignment();

        float x, y;
        createRoomPtr->GetPosition(x, y);
        float screenHeight = static_cast<float>(_graphics.GetScreenHeight());
        createRoomPtr->SetPosition(x, screenHeight - bottomMargin);

        createRoomPtr->SetText("Create Room");
        createRoomPtr->SetBackgroundColor(0xFF4CAF50);  // Green
        createRoomPtr->SetHoverColor(0xFF66BB6A);
        createRoomPtr->SetTextColor(0xFFFFFFFF);
        createRoomPtr->SetCallback([this]() { OnCreateRoomClicked(); });

        _createRoomButton = std::move(createRoomPtr);
        _menu->AddButton(_createRoomButton);

        // Add "Refresh" button at bottom right
        const float rightMargin = 20.0f;
        auto refreshPtr = _uiFactory.CreateButton();
        refreshPtr->SetSize(150.0f, 45.0f);
        float screenWidth = static_cast<float>(_graphics.GetScreenWidth());
        refreshPtr->SetPosition(screenWidth - rightMargin - 150.0f, screenHeight - bottomMargin);
        refreshPtr->SetText("Refresh");
        refreshPtr->SetBackgroundColor(0xFF2196F3);  // Blue
        refreshPtr->SetHoverColor(0xFF64B5F6);
        refreshPtr->SetTextColor(0xFFFFFFFF);
        refreshPtr->SetCallback([this]() { OnRefreshClicked(); });

        _refreshButton = std::move(refreshPtr);
        _menu->AddButton(_refreshButton);

        // Add "Back" button at bottom left
        const float leftMargin = 20.0f;
        auto backPtr = _uiFactory.CreateButton();
        backPtr->SetSize(150.0f, 45.0f);
        backPtr->SetPosition(leftMargin, screenHeight - bottomMargin);
        backPtr->SetText("Back");
        backPtr->SetBackgroundColor(0xFF424242);  // Dark gray
        backPtr->SetHoverColor(0xFF616161);
        backPtr->SetTextColor(0xFFFFFFFF);
        backPtr->SetCallback([this]() { OnBackClicked(); });

        _backButton = std::move(backPtr);
        _menu->AddButton(_backButton);
    }

    void RoomListMenu::Update() {
        if (!_menu || !_menu->IsVisible()) {
            return;
        }
        BaseMenu::Update();
    }

    void RoomListMenu::Render() {
        if (!_menu || !_menu->IsVisible()) {
            return;
        }

        // Draw title
        const char *title = "Select a Room";
        int screenWidth = _graphics.GetScreenWidth();
        int titleFontSize = 32;
        int titleWidth = static_cast<int>(strlen(title) * titleFontSize * 0.5f);
        int titleX = (screenWidth - titleWidth) / 2;
        _graphics.DrawText(title, titleX, 30, titleFontSize, 0xFFFFFFFF);

        // Draw room count
        std::string countText = "Available Rooms: " + std::to_string(_rooms.size());
        int countFontSize = 18;
        int countWidth = static_cast<int>(countText.length() * countFontSize * 0.5f);
        int countX = (screenWidth - countWidth) / 2;
        _graphics.DrawText(countText.c_str(), countX, 70, countFontSize, 0xFFAAAAAA);

        BaseMenu::Render();
    }

    void RoomListMenu::RebuildRoomList() {
        if (!_menu) {
            return;
        }

        // Clear the entire menu and rebuild everything
        _menu->Clear();
        _roomButtons.clear();

        // Create buttons for each room
        float currentY = LIST_START_Y;
        for (size_t i = 0; i < _rooms.size() && i < MAX_VISIBLE_ROOMS; ++i) {
            const auto &room = _rooms[i];

            // Room button
            auto buttonPtr = _uiFactory.CreateButton();
            buttonPtr->SetSize(ROOM_BUTTON_WIDTH, ROOM_BUTTON_HEIGHT);
            buttonPtr->SetAlign(UI::Align::CENTER_HORIZONTAL);
            buttonPtr->ApplyAlignment();

            float x, y;
            buttonPtr->GetPosition(x, y);
            buttonPtr->SetPosition(x, currentY);

            // Button text: "RoomName [2/4] - WAITING"
            std::ostringstream oss;
            oss << room.roomName << " [" << room.playerCount << "/" << room.maxPlayers << "]";
            if (room.isPrivate) {
                oss << " 🔒";
            }

            // State text
            const char *stateText = " - WAITING";
            if (room.state == 1)
                stateText = " - STARTING";
            else if (room.state == 2)
                stateText = " - IN PROGRESS";
            else if (room.state == 3)
                stateText = " - FINISHED";
            oss << stateText;

            buttonPtr->SetText(oss.str());

            // Color based on room state
            if (room.state == 0) {                          // WAITING
                buttonPtr->SetBackgroundColor(0xFF4CAF50);  // Green
                buttonPtr->SetHoverColor(0xFF66BB6A);
            } else if (room.state == 1) {                   // STARTING
                buttonPtr->SetBackgroundColor(0xFFFFA726);  // Orange
                buttonPtr->SetHoverColor(0xFFFFB74D);
            } else if (room.state == 2) {                   // IN_PROGRESS (spectator join allowed)
                buttonPtr->SetBackgroundColor(0xFF2196F3);  // Blue for spectate
                buttonPtr->SetHoverColor(0xFF64B5F6);
            } else {                                        // FINISHED
                buttonPtr->SetBackgroundColor(0xFF9E9E9E);  // Gray (disabled)
                buttonPtr->SetHoverColor(0xFF9E9E9E);
            }

            buttonPtr->SetTextColor(0xFFFFFFFF);
            buttonPtr->SetTextSize(16);

            size_t index = i;  // Capture by value for lambda

            // Allow joining WAITING/STARTING normally and IN_PROGRESS as spectator
            if (room.state <= 2) {  // WAITING, STARTING, IN_PROGRESS
                buttonPtr->SetCallback([this, index]() { OnRoomClicked(index); });
            } else {
                buttonPtr->SetCallback(
                    []() { LOG_INFO("[RoomListMenu] Cannot join this room - game finished"); });
            }

            std::shared_ptr<UI::IButton> button = std::move(buttonPtr);
            _roomButtons.push_back(button);
            _menu->AddButton(button);

            currentY += ROOM_BUTTON_HEIGHT + ROOM_BUTTON_SPACING;
        }

        // Re-add persistent buttons
        if (_createRoomButton) {
            _menu->AddButton(_createRoomButton);
        }
        if (_refreshButton) {
            _menu->AddButton(_refreshButton);
        }
        if (_backButton) {
            _menu->AddButton(_backButton);
        }
    }

    void RoomListMenu::OnRoomClicked(size_t index) {
        if (index >= _rooms.size()) {
            LOG_ERROR("[RoomListMenu] Invalid room index: ", index);
            return;
        }

        const auto &room = _rooms[index];
        LOG_INFO("[RoomListMenu] Selected room: ", room.roomName, " (ID: ", room.roomId, ")");

        if (_onRoomSelected) {
            _onRoomSelected(room.roomId);
        }
    }

    void RoomListMenu::OnCreateRoomClicked() {
        LOG_INFO("[RoomListMenu] Create Room button clicked");
        if (_onCreateRoom) {
            _onCreateRoom();
        }
    }

    void RoomListMenu::OnRefreshClicked() {
        LOG_INFO("[RoomListMenu] Refresh button clicked");
        if (_onRefresh) {
            _onRefresh();
        }
    }

    void RoomListMenu::OnBackClicked() {
        LOG_INFO("[RoomListMenu] Back button clicked");
        if (_onBack) {
            _onBack();
        }
    }
}  // namespace Game
