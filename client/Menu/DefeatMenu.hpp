/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** DefeatMenu - Game over defeat screen
*/

#pragma once

#include <functional>
#include <memory>
#include <string>

#include "Menu/BaseMenu.hpp"
#include "UI/IUIFactory.hpp"

namespace Game {
    /**
     * @brief Defeat screen displayed when the player dies.
     * 
     * Shows defeat message and allows player to return to main menu.
     */
    class DefeatMenu : public BaseMenu {
       public:
        /**
         * @brief Construct a new DefeatMenu.
         * @param uiFactory UI factory used to create menus and buttons.
         */
        explicit DefeatMenu(UI::IUIFactory &uiFactory);
        ~DefeatMenu() override = default;

        /**
         * @brief Initialize UI elements (creates buttons and text).
         */
        void Initialize() override;

        /**
         * @brief Set callback invoked when user clicks return to menu button.
         */
        void SetOnReturnToMenu(std::function<void()> callback);

        /**
         * @brief Set the defeat reason/message to display.
         * @param reason Reason for defeat (e.g., "Player Defeated", "Game Over")
         */
        void SetDefeatReason(const std::string &reason);

        /**
         * @brief Custom render to display defeat message.
         */
        void Render() override;

       private:
        void OnReturnToMenuClicked();

        std::function<void()> _onReturnToMenu{};
        std::string _defeatReason{"DEFEAT"};
    };
}  // namespace Game
