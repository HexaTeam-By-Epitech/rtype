/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** VictoryMenu - Victory screen
*/

#include "Menu/VictoryMenu.hpp"
#include "../common/Logger/Logger.hpp"

namespace Game {
    VictoryMenu::VictoryMenu(UI::IUIFactory &uiFactory) : BaseMenu(uiFactory) {}

    void VictoryMenu::Initialize() {
        if (!_menu) {
            return;
        }

        _menu->Clear();

        const float buttonWidth = 300.0f;
        const float buttonHeight = 60.0f;

        // Single "Return to Menu" button, centered below the victory message
        _menu->AddButton(CreateCenteredButton("RETURN TO MENU", 100.0f, buttonWidth, buttonHeight, 0xFF2E7D32,
                                              0xFF66BB6A,
                                              WrapWithClickSound([this]() { OnReturnToMenuClicked(); })));
    }

    void VictoryMenu::SetOnReturnToMenu(std::function<void()> callback) {
        _onReturnToMenu = std::move(callback);
    }

    void VictoryMenu::SetVictoryMessage(const std::string &message) {
        _victoryMessage = message;
    }

    void VictoryMenu::Render() {
        if (!IsVisible()) {
            return;
        }

        // Draw background and buttons
        BaseMenu::Render();

        // Note: The victory text will need to be drawn by the graphics system
        // in the GameLoop's render method when this menu is active
    }

    void VictoryMenu::OnReturnToMenuClicked() {
        LOG_INFO("[VictoryMenu] Return to menu clicked");
        if (_onReturnToMenu) {
            _onReturnToMenu();
        }
    }
}  // namespace Game
