/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** VictoryMenu - Victory screen
*/

#pragma once

#include <functional>
#include <memory>
#include <string>

#include "Menu/BaseMenu.hpp"
#include "UI/IUIFactory.hpp"

namespace Game {
    /**
     * @brief Victory screen displayed when the player wins the level.
     * 
     * Shows victory message and allows player to return to main menu or continue.
     */
    class VictoryMenu : public BaseMenu {
       public:
        /**
         * @brief Construct a new VictoryMenu.
         * @param uiFactory UI factory used to create menus and buttons.
         */
        explicit VictoryMenu(UI::IUIFactory &uiFactory);
        ~VictoryMenu() override = default;

        /**
         * @brief Initialize UI elements (creates buttons and text).
         */
        void Initialize() override;

        /**
         * @brief Set callback invoked when user clicks return to menu button.
         */
        void SetOnReturnToMenu(std::function<void()> callback);

        /**
         * @brief Set the victory message to display.
         * @param message Victory message (e.g., "Level Complete!", "Victory!")
         */
        void SetVictoryMessage(const std::string &message);

        /**
         * @brief Custom render to display victory message.
         */
        void Render() override;

       private:
        void OnReturnToMenuClicked();

        std::function<void()> _onReturnToMenu{};
        std::string _victoryMessage{"VICTORY!"};
    };
}  // namespace Game
