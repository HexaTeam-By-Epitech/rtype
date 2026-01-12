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

    void MainMenu::SetOnProfile(std::function<void()> onProfile) {
        _onProfile = std::move(onProfile);
    }

    void MainMenu::SetProfileName(const std::string &name) {
        if (_profileButton) {
            _profileButton->SetText(name);
        }
    }

    void MainMenu::SetScreenSize(float width, float height) {
        _screenWidth = width;
        _screenHeight = height;
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

        // Profile Button (Top Left)
        const float profileBtnWidth = 120.0f;
        const float profileBtnHeight = 40.0f;
        const float margin = 20.0f;

        _profileButton = _uiFactory.CreateButton();
        _profileButton->SetSize(profileBtnWidth, profileBtnHeight);
        _profileButton->SetPosition(margin, margin);
        _profileButton->SetText("GUEST");                // Default
        _profileButton->SetBackgroundColor(0xFF2196F3);  // Blue
        _profileButton->SetHoverColor(0xFF64B5F6);
        _profileButton->SetTextColor(0xFFFFFFFF);
        _profileButton->SetCallback([this]() { OnProfileClicked(); });

        _menu->AddButton(_profileButton);
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

    void MainMenu::OnProfileClicked() {
        LOG_INFO("[MainMenu] Profile button clicked!");
        if (_onProfile) {
            _onProfile();
        }
    }
}  // namespace Game
