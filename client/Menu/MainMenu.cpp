/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** MainMenu - Main menu for the game (business logic)
*/

#include "Menu/MainMenu.hpp"
#include "../common/Logger/Logger.hpp"

namespace Game {
    MainMenu::MainMenu(UI::IUIFactory &uiFactory) : BaseMenu(uiFactory) {}

    void MainMenu::SetOnQuit(std::function<void()> onQuit) {
        _onQuit = std::move(onQuit);
    }

    void MainMenu::SetOnSettings(std::function<void()> onSettings) {
        _onSettings = std::move(onSettings);
    }

    void MainMenu::SetOnPlay(std::function<void()> onPlay) {
        _onPlay = std::move(onPlay);
    }

    void MainMenu::Initialize() {
        if (!_menu) {
            return;
        }

        const float buttonWidth = 200.0f;
        const float buttonHeight = 50.0f;
        const float spacing = 20.0f;

        // 3 buttons stack
        const float totalHeight = (buttonHeight * 3.0f) + (spacing * 2.0f);
        const float offsets[3] = {-(totalHeight / 2.0f) + (buttonHeight / 2.0f), 0.0f,
                                  (totalHeight / 2.0f) - (buttonHeight / 2.0f)};

        _menu->Clear();
        _menu->AddButton(CreateCenteredButton("PLAY", offsets[0], buttonWidth, buttonHeight, 0xFF4CAF50,
                                              0xFF66BB6A, [this]() { OnPlayClicked(); }));
        _menu->AddButton(CreateCenteredButton("SETTINGS", offsets[1], buttonWidth, buttonHeight, 0xFF424242,
                                              0xFF616161, [this]() { OnSettingsClicked(); }));
        _menu->AddButton(CreateCenteredButton("QUIT", offsets[2], buttonWidth, buttonHeight, 0xFFF44336,
                                              0xFFE57373, [this]() { OnQuitClicked(); }));
    }

    void MainMenu::OnPlayClicked() {
        LOG_INFO("hello world");

        if (_onPlay) {
            _onPlay();
        }
    }

    void MainMenu::OnSettingsClicked() {
        LOG_INFO("[MainMenu] Settings button clicked!");
        if (_onSettings) {
            _onSettings();
        }
    }

    void MainMenu::OnQuitClicked() {
        LOG_INFO("[MainMenu] Quit button clicked!");
        if (_onQuit) {
            _onQuit();
        } else {
            LOG_WARNING("[MainMenu] Quit requested but no onQuit callback was set");
        }
    }
}  // namespace Game
