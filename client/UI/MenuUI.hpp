/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** MenuUI.hpp - Simple lobby menu UI
*/

#pragma once

#include <functional>
#include <string>
#include <vector>
#include "Capnp/Messages/Messages.hpp"
#include "Graphics/IGraphics.hpp"

/**
 * @class MenuUI
 * @brief Simple placeholder UI for lobby/room selection
 * 
 * This is a temporary UI for testing the lobby/room system.
 * It will be replaced with a proper UI later.
 */
class MenuUI {
   public:
    enum class MenuState {
        MAIN_MENU,  // Show "Create Room" and "Join Room" buttons
        ROOM_LIST,  // Show list of available rooms
        IN_ROOM     // Player is in a room (waiting for others)
    };

    /**
     * @brief Constructor
     * @param graphics Graphics interface for rendering
     */
    explicit MenuUI(Graphics::IGraphics &graphics);

    /**
     * @brief Update menu state and handle input
     */
    void update();

    /**
     * @brief Draw the menu
     */
    void draw();

    /**
     * @brief Set the callback for when user wants to create a room
     */
    void setOnCreateRoom(std::function<void()> callback) { _onCreateRoom = callback; }

    /**
     * @brief Set the callback for when user wants to list rooms
     */
    void setOnListRooms(std::function<void()> callback) { _onListRooms = callback; }

    /**
     * @brief Set the callback for when user wants to join a room
     */
    void setOnJoinRoom(std::function<void(const std::string &roomId)> callback) { _onJoinRoom = callback; }

    /**
     * @brief Update the room list
     */
    void setRoomList(const std::vector<RType::Messages::S2C::RoomInfoData> &rooms) { _rooms = rooms; }

    /**
     * @brief Set current menu state
     */
    void setState(MenuState state) { _state = state; }

    /**
     * @brief Get current menu state
     */
    MenuState getState() const { return _state; }

    /**
     * @brief Set current room ID (when in a room)
     */
    void setCurrentRoomId(const std::string &roomId) { _currentRoomId = roomId; }

   private:
    void _drawMainMenu();
    void _drawRoomList();
    void _drawInRoom();
    bool _isMouseOverButton(int x, int y, int width, int height);

    Graphics::IGraphics &_graphics;
    MenuState _state = MenuState::MAIN_MENU;

    // Room data
    std::vector<RType::Messages::S2C::RoomInfoData> _rooms;
    std::string _currentRoomId;
    int _selectedRoomIndex = -1;

    // Callbacks
    std::function<void()> _onCreateRoom;
    std::function<void()> _onListRooms;
    std::function<void(const std::string &roomId)> _onJoinRoom;

    // UI Constants
    static constexpr int BUTTON_WIDTH = 300;
    static constexpr int BUTTON_HEIGHT = 50;
    static constexpr int BUTTON_SPACING = 20;
};
