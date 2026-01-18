/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** DefeatMenu - Game over defeat screen
*/

#include "Menu/DefeatMenu.hpp"
#include "../common/Logger/Logger.hpp"

namespace Game {
    DefeatMenu::DefeatMenu(UI::IUIFactory &uiFactory) : BaseMenu(uiFactory) {}

    void DefeatMenu::Initialize() {
        if (!_menu) {
            return;
        }

        _menu->Clear();

        const float buttonWidth = 300.0f;
        const float buttonHeight = 60.0f;

        // Single "Return to Menu" button, centered below the defeat message
        _menu->AddButton(CreateCenteredButton("RETURN TO MENU", 100.0f, buttonWidth, buttonHeight, 0xFF1976D2,
                                              0xFF42A5F5,
                                              WrapWithClickSound([this]() { OnReturnToMenuClicked(); })));
    }

    void DefeatMenu::SetOnReturnToMenu(std::function<void()> callback) {
        _onReturnToMenu = std::move(callback);
    }

    void DefeatMenu::SetDefeatReason(const std::string &reason) {
        _defeatReason = reason;
    }

    void DefeatMenu::Render() {
        if (!IsVisible()) {
            return;
        }

        // Draw dark overlay background
        // This will be rendered by the graphics system
        // For now, just call the base menu render which will draw buttons
        BaseMenu::Render();

        // Note: The defeat text will need to be drawn by the graphics system
        // in the GameLoop's render method when this menu is active
    }

    void DefeatMenu::OnReturnToMenuClicked() {
        LOG_INFO("[DefeatMenu] Return to menu clicked");
        if (_onReturnToMenu) {
            _onReturnToMenu();
        }
    }
}  // namespace Game
