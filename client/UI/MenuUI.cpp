/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** MenuUI.cpp - Simple lobby menu UI implementation
*/

#include "MenuUI.hpp"

// Colors (0xAARRGGBB)
static constexpr unsigned int COLOR_BACKGROUND = 0xFF1A1A2E;
static constexpr unsigned int COLOR_BUTTON = 0xFF16213E;
static constexpr unsigned int COLOR_BUTTON_HOVER = 0xFF0F3460;
static constexpr unsigned int COLOR_TEXT = 0xFFE94560;
static constexpr unsigned int COLOR_TEXT_HOVER = 0xFFFFFFFF;

MenuUI::MenuUI(Graphics::IGraphics &graphics) : _graphics(graphics) {}

void MenuUI::update() {
    // Handle input based on current state
    float mouseX, mouseY;
    _graphics.GetMousePosition(mouseX, mouseY);

    switch (_state) {
        case MenuState::MAIN_MENU:
            // Check if mouse is over buttons
            break;
        case MenuState::ROOM_LIST:
            // Check if mouse is over room items
            break;
        case MenuState::IN_ROOM:
            // Nothing to do, waiting for others
            break;
    }
}

void MenuUI::draw() {
    switch (_state) {
        case MenuState::MAIN_MENU:
            _drawMainMenu();
            break;
        case MenuState::ROOM_LIST:
            _drawRoomList();
            break;
        case MenuState::IN_ROOM:
            _drawInRoom();
            break;
    }
}

void MenuUI::_drawMainMenu() {
    // Draw title
    _graphics.DrawText(-1, "R-TYPE LOBBY", 400, 100, 40, COLOR_TEXT);

    // Calculate button positions (centered)
    int centerX = 400;
    int startY = 300;

    // Draw "Create Room" button
    int createButtonY = startY;
    float mouseX, mouseY;
    _graphics.GetMousePosition(mouseX, mouseY);

    bool isOverCreate =
        _isMouseOverButton(centerX - BUTTON_WIDTH / 2, createButtonY, BUTTON_WIDTH, BUTTON_HEIGHT);
    unsigned int createButtonColor = isOverCreate ? COLOR_BUTTON_HOVER : COLOR_BUTTON;
    unsigned int createTextColor = isOverCreate ? COLOR_TEXT_HOVER : COLOR_TEXT;

    _graphics.DrawRectFilled(centerX - BUTTON_WIDTH / 2, createButtonY, BUTTON_WIDTH, BUTTON_HEIGHT,
                             createButtonColor);
    _graphics.DrawRect(centerX - BUTTON_WIDTH / 2, createButtonY, BUTTON_WIDTH, BUTTON_HEIGHT, COLOR_TEXT);
    _graphics.DrawText(-1, "CREATE ROOM", centerX - 80, createButtonY + 15, 20, createTextColor);

    // Handle click
    if (isOverCreate && _graphics.IsMouseButtonPressed(0)) {
        if (_onCreateRoom) {
            _onCreateRoom();
        }
    }

    // Draw "Join Room" button
    int joinButtonY = startY + BUTTON_HEIGHT + BUTTON_SPACING;
    bool isOverJoin =
        _isMouseOverButton(centerX - BUTTON_WIDTH / 2, joinButtonY, BUTTON_WIDTH, BUTTON_HEIGHT);
    unsigned int joinButtonColor = isOverJoin ? COLOR_BUTTON_HOVER : COLOR_BUTTON;
    unsigned int joinTextColor = isOverJoin ? COLOR_TEXT_HOVER : COLOR_TEXT;

    _graphics.DrawRectFilled(centerX - BUTTON_WIDTH / 2, joinButtonY, BUTTON_WIDTH, BUTTON_HEIGHT,
                             joinButtonColor);
    _graphics.DrawRect(centerX - BUTTON_WIDTH / 2, joinButtonY, BUTTON_WIDTH, BUTTON_HEIGHT, COLOR_TEXT);
    _graphics.DrawText(-1, "JOIN ROOM", centerX - 65, joinButtonY + 15, 20, joinTextColor);

    // Handle click
    if (isOverJoin && _graphics.IsMouseButtonPressed(0)) {
        _state = MenuState::ROOM_LIST;
        if (_onListRooms) {
            _onListRooms();
        }
    }
}

void MenuUI::_drawRoomList() {
    // Draw title
    _graphics.DrawText(-1, "AVAILABLE ROOMS", 350, 50, 30, COLOR_TEXT);

    // Draw room list
    int startY = 120;
    int itemHeight = 60;

    if (_rooms.empty()) {
        _graphics.DrawText(-1, "No rooms available", 350, 300, 20, COLOR_TEXT);
        _graphics.DrawText(-1, "Press ESC to go back", 330, 340, 20, COLOR_TEXT);
    } else {
        for (size_t i = 0; i < _rooms.size(); ++i) {
            const auto &room = _rooms[i];
            int y = startY + static_cast<int>(i) * (itemHeight + 10);

            float mouseX, mouseY;
            _graphics.GetMousePosition(mouseX, mouseY);
            bool isOver = _isMouseOverButton(50, y, 700, itemHeight);

            unsigned int bgColor = isOver ? COLOR_BUTTON_HOVER : COLOR_BUTTON;
            _graphics.DrawRectFilled(50, y, 700, itemHeight, bgColor);
            _graphics.DrawRect(50, y, 700, itemHeight, COLOR_TEXT);

            // Draw room info
            std::string roomText = room.roomName + " (" + std::to_string(room.playerCount) + "/" +
                                   std::to_string(room.maxPlayers) + ")";
            _graphics.DrawText(-1, roomText.c_str(), 60, y + 10, 20, COLOR_TEXT);

            std::string stateText = "State: ";
            if (room.state == 0)
                stateText += "WAITING";
            else if (room.state == 1)
                stateText += "STARTING";
            else if (room.state == 2)
                stateText += "IN PROGRESS";
            else
                stateText += "FINISHED";
            _graphics.DrawText(-1, stateText.c_str(), 60, y + 35, 16, COLOR_TEXT);

            // Handle click
            if (isOver && _graphics.IsMouseButtonPressed(0)) {
                if (_onJoinRoom) {
                    _onJoinRoom(room.roomId);
                }
            }
        }
    }

    // Draw back button
    _graphics.DrawText(-1, "Press ESC to go back", 320, 550, 18, COLOR_TEXT);
    if (_graphics.IsKeyPressed(256)) {  // ESC key (Raylib KEY_ESCAPE = 256)
        _state = MenuState::MAIN_MENU;
    }
}

void MenuUI::_drawInRoom() {
    // Draw title
    _graphics.DrawText(-1, "IN ROOM", 400, 100, 40, COLOR_TEXT);

    // Draw room ID
    std::string roomText = "Room: " + _currentRoomId;
    _graphics.DrawText(-1, roomText.c_str(), 300, 200, 20, COLOR_TEXT);

    // Draw waiting message
    _graphics.DrawText(-1, "Waiting for other players...", 280, 280, 20, COLOR_TEXT);

    // Draw "START GAME" button
    int centerX = 400;
    int buttonY = 350;
    float mouseX, mouseY;
    _graphics.GetMousePosition(mouseX, mouseY);

    bool isOverStart = _isMouseOverButton(centerX - BUTTON_WIDTH / 2, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT);
    unsigned int startButtonColor = isOverStart ? COLOR_BUTTON_HOVER : COLOR_BUTTON;
    unsigned int startTextColor = isOverStart ? COLOR_TEXT_HOVER : COLOR_TEXT;

    _graphics.DrawRectFilled(centerX - BUTTON_WIDTH / 2, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT,
                             startButtonColor);
    _graphics.DrawRect(centerX - BUTTON_WIDTH / 2, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, COLOR_TEXT);
    _graphics.DrawText(-1, "START GAME", centerX - 70, buttonY + 15, 20, startTextColor);

    // Handle click
    if (isOverStart && _graphics.IsMouseButtonPressed(0)) {
        if (_onStartGame) {
            _onStartGame();
        }
    }
}

bool MenuUI::_isMouseOverButton(int x, int y, int width, int height) {
    float mouseX, mouseY;
    _graphics.GetMousePosition(mouseX, mouseY);
    return mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height;
}
