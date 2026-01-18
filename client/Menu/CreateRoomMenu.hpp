/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** CreateRoomMenu - Menu for creating a new room
*/

#pragma once

#include <functional>
#include <memory>
#include <string>

#include "Graphics/IGraphics.hpp"
#include "Menu/BaseMenu.hpp"
#include "UI/ISlider.hpp"
#include "UI/IUIFactory.hpp"

namespace Game {
    /**
     * @brief Menu for creating a new game room
     */
    class CreateRoomMenu : public BaseMenu {
       public:
        explicit CreateRoomMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics);
        ~CreateRoomMenu() override = default;

        /**
         * @brief Set callback triggered when Create button is clicked
         * @param onCreate Callback receiving (roomName, maxPlayers, isPrivate, gameSpeedMultiplier)
         */
        void SetOnCreate(std::function<void(const std::string &, uint32_t, bool, float)> onCreate);

        /**
         * @brief Set callback triggered when Cancel button is clicked
         */
        void SetOnCancel(std::function<void()> onCancel);

        void Initialize() override;
        void Update() override;
        void Render() override;

       private:
        void OnCreateClicked();
        void OnCancelClicked();
        bool ValidateInput();
        void UpdateSpeedButtonText();

        std::function<void(const std::string &, uint32_t, bool, float)> _onCreate{};
        std::function<void()> _onCancel{};

        Graphics::IGraphics &_graphics;

        std::shared_ptr<UI::ITextInput> _roomNameInput;
        std::shared_ptr<UI::IButton> _maxPlayersButton;
        std::shared_ptr<UI::IButton> _privateButton;
        std::shared_ptr<UI::IButton> _gameSpeedButton;
        std::shared_ptr<UI::IButton> _createButton;
        std::shared_ptr<UI::IButton> _cancelButton;

        uint32_t _selectedMaxPlayers = 4;  // Default 4 players
        bool _isPrivate = false;
        float _gameSpeedMultiplier = 1.0f;  // Default normal speed (100%)

        std::string _errorMessage;
    };
}  // namespace Game
