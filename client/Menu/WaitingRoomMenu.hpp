/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** WaitingRoomMenu - Menu for waiting before game starts
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
     * @brief Player information in waiting room
     */
    struct PlayerInfo {
        uint32_t playerId;
        std::string playerName;
        bool isHost;
        bool isSpectator;

        PlayerInfo(uint32_t id, const std::string &name, bool host, bool spectator)
            : playerId(id), playerName(name), isHost(host), isSpectator(spectator) {}
    };

    /**
     * @brief Menu displayed while waiting for game to start
     */
    class WaitingRoomMenu : public BaseMenu {
       public:
        explicit WaitingRoomMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics);
        ~WaitingRoomMenu() override = default;

        /**
         * @brief Set callback triggered when Start Game button is clicked (host only)
         */
        void SetOnStartGame(std::function<void()> onStartGame);

        /**
         * @brief Set callback triggered when Back button is clicked
         */
        void SetOnBack(std::function<void()> onBack);

        /**
         * @brief Update the player list
         */
        void UpdatePlayerList(const std::vector<PlayerInfo> &players);

        /**
         * @brief Set room name and player count info
         */
        void SetRoomInfo(const std::string &roomName, uint32_t currentPlayers, uint32_t maxPlayers);

        /**
         * @brief Set whether current player is host (enables Start Game button)
         */
        void SetIsHost(bool isHost);

        /**
         * @brief Set whether current player is spectator (shows spectator banner)
         */
        void SetIsSpectator(bool isSpectator);

        void Initialize() override;
        void Update() override;
        void Render() override;

       private:
        void OnStartGameClicked();
        void OnBackClicked();
        void RebuildPlayerList();

        std::function<void()> _onStartGame{};
        std::function<void()> _onBack{};

        Graphics::IGraphics &_graphics;

        std::string _roomName;
        uint32_t _currentPlayers = 0;
        uint32_t _maxPlayers = 4;
        bool _isHost = false;
        bool _isSpectator = false;  // True if player is in spectator mode

        std::vector<PlayerInfo> _players;
        std::shared_ptr<UI::IButton> _startGameButton;
        std::shared_ptr<UI::IButton> _backButton;

        static constexpr float PLAYER_ITEM_HEIGHT = 50.0f;
        static constexpr float PLAYER_ITEM_SPACING = 10.0f;
        static constexpr float LIST_START_Y = 150.0f;
        static constexpr size_t MAX_VISIBLE_PLAYERS = 8;
    };
}  // namespace Game
