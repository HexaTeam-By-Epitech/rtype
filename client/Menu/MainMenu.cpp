/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** MainMenu - Main menu for the game (business logic)
*/

#include "Menu/MainMenu.hpp"
#include "../common/Logger/Logger.hpp"

namespace Game {
    MainMenu::MainMenu(UI::IUIFactory &uiFactory) : _uiFactory(uiFactory) {
        _menu = _uiFactory.CreateMenu();
    }

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

        auto makeButton = [&](const char *label, float offsetY, unsigned int backgroundColor,
                              unsigned int hoverColor, std::function<void()> callback) {
            auto button = _uiFactory.CreateButton();
            button->SetSize(buttonWidth, buttonHeight);
            button->SetAlign(UI::Align::CENTER_BOTH);
            button->ApplyAlignment();

            float x = 0.0f;
            float y = 0.0f;
            button->GetPosition(x, y);
            button->SetPosition(x, y + offsetY);

            button->SetBackgroundColor(backgroundColor);
            button->SetHoverColor(hoverColor);
            button->SetText(label);
            button->SetTextSize(18);
            button->SetTextColor(0xFFFFFFFF);
            button->SetFont(-1);
            button->SetCallback(std::move(callback));
            return button;
        };

        _menu->Clear();
        _menu->AddButton(
            makeButton("PLAY", offsets[0], 0xFF4CAF50, 0xFF66BB6A, [this]() { OnPlayClicked(); }));
        _menu->AddButton(
            makeButton("SETTINGS", offsets[1], 0xFF424242, 0xFF616161, [this]() { OnSettingsClicked(); }));
        _menu->AddButton(
            makeButton("QUIT", offsets[2], 0xFFF44336, 0xFFE57373, [this]() { OnQuitClicked(); }));
    }

    void MainMenu::Update() {
        if (_menu) {
            _menu->Update();
        }
    }

    void MainMenu::Render() {
        if (_menu) {
            _menu->Render();
        }
    }

    void MainMenu::Show() {
        if (_menu) {
            _menu->SetVisible(true);
        }
    }

    void MainMenu::Hide() {
        if (_menu) {
            _menu->SetVisible(false);
        }
    }

    bool MainMenu::IsVisible() const {
        return _menu && _menu->IsVisible();
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
