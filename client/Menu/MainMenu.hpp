/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** MainMenu - Main menu for the game (business logic)
*/

#pragma once

#include <functional>
#include <memory>

#include "Menu/BaseMenu.hpp"
#include "UI/IUIFactory.hpp"

namespace Game {
    /**
     * @brief Main menu of the game.
     *
     * Business-level class: it *uses* the UI library, but does not belong to it.
     */
    class MainMenu : public BaseMenu {
       public:
        explicit MainMenu(UI::IUIFactory &uiFactory);
        ~MainMenu() override = default;

        /**
         * @brief Set callback triggered when the Quit button is clicked.
         *
         * This keeps the menu class in the business layer (no direct dependency on Graphics/window APIs).
         */
        void SetOnQuit(std::function<void()> onQuit);

        /**
         * @brief Set callback triggered when the Settings button is clicked.
         */
        void SetOnSettings(std::function<void()> onSettings);

        /**
         * @brief Set callback triggered when the Play button is clicked.
         */
        void SetOnPlay(std::function<void()> onPlay);

        /**
         * @brief Set callback triggered when the Profile/Guest button is clicked.
         */
        void SetOnProfile(std::function<void()> onProfile);

        /**
         * @brief Update the text of the Profile button (e.g. "GUEST" -> "PlayerName").
         */
        void SetProfileName(const std::string &name);

        /**
         * @brief Set screen size for layout calculations.
         */
        void SetScreenSize(float width, float height);

        void Initialize() override;

       private:
        void OnPlayClicked();
        void OnQuitClicked();
        void OnSettingsClicked();
        void OnProfileClicked();

        std::function<void()> _onPlay{};
        std::function<void()> _onQuit{};
        std::function<void()> _onSettings{};
        std::function<void()> _onProfile{};

        std::shared_ptr<UI::IButton> _profileButton;

        float _screenWidth = 800.0f;
        float _screenHeight = 600.0f;
    };
}  // namespace Game
