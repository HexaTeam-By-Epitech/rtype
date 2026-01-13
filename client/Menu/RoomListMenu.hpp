/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RoomListMenu - Menu for selecting or creating a room
*/

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Graphics/IGraphics.hpp"
#include "Menu/BaseMenu.hpp"
#include "UI/IUIFactory.hpp"

namespace Game {
    /**
     * @brief Room list menu for selecting and managing game rooms
     */
    class RoomListMenu : public BaseMenu {
       public:
        struct RoomInfo {
            std::string roomId;
            std::string roomName;
            uint32_t playerCount;
            uint32_t maxPlayers;
            bool isPrivate;
            uint8_t state;  // 0=WAITING, 1=STARTING, 2=IN_PROGRESS, 3=FINISHED

            RoomInfo(const std::string &id, const std::string &name, uint32_t count, uint32_t max, bool priv,
                     uint8_t st)
                : roomId(id),
                  roomName(name),
                  playerCount(count),
                  maxPlayers(max),
                  isPrivate(priv),
                  state(st) {}
        };

        explicit RoomListMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics);
        ~RoomListMenu() override = default;

        /**
         * @brief Set callback triggered when a room is selected
         */
        void SetOnRoomSelected(std::function<void(const std::string &roomId)> onRoomSelected);

        /**
         * @brief Set callback triggered when "Create Room" button is clicked
         */
        void SetOnCreateRoom(std::function<void()> onCreateRoom);

        /**
         * @brief Set callback triggered when Back button is clicked
         */
        void SetOnBack(std::function<void()> onBack);

        /**
         * @brief Update the room list
         */
        void UpdateRoomList(const std::vector<RoomInfo> &rooms);

        /**
         * @brief Clear all rooms
         */
        void ClearRooms();

        void Initialize() override;
        void Update() override;
        void Render() override;

       private:
        void OnRoomClicked(size_t index);
        void OnCreateRoomClicked();
        void OnBackClicked();
        void RebuildRoomList();

        std::function<void(const std::string &)> _onRoomSelected{};
        std::function<void()> _onCreateRoom{};
        std::function<void()> _onBack{};

        Graphics::IGraphics &_graphics;

        std::vector<RoomInfo> _rooms;
        std::vector<std::shared_ptr<UI::IButton>> _roomButtons;

        std::shared_ptr<UI::IButton> _createRoomButton;
        std::shared_ptr<UI::IButton> _backButton;

        static constexpr float ROOM_BUTTON_WIDTH = 500.0f;
        static constexpr float ROOM_BUTTON_HEIGHT = 60.0f;
        static constexpr float ROOM_BUTTON_SPACING = 10.0f;
        static constexpr float LIST_START_Y = 100.0f;
        static constexpr size_t MAX_VISIBLE_ROOMS = 6;
    };
}  // namespace Game
