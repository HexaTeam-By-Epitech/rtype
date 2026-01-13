/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** ConfirmQuitMenu - Quit confirmation dialog (business logic)
*/

#include "Menu/ConfirmQuitMenu.hpp"
#include "../common/Logger/Logger.hpp"

namespace Game {
    ConfirmQuitMenu::ConfirmQuitMenu(UI::IUIFactory &uiFactory) : BaseMenu(uiFactory) {}

    void ConfirmQuitMenu::Initialize() {
        if (!_menu) {
            return;
        }

        _menu->Clear();

        const float buttonWidth = 280.0f;
        const float buttonHeight = 50.0f;
        const float spacing = 18.0f;

        // 2 buttons stack, centered
        const float totalHeight = (buttonHeight * 2.0f) + spacing;
        const float offsets[2] = {-(totalHeight / 2.0f) + (buttonHeight / 2.0f),
                                  (totalHeight / 2.0f) - (buttonHeight / 2.0f)};

        _menu->AddButton(CreateCenteredButton("YES, QUIT", offsets[0], buttonWidth, buttonHeight, 0xFFF44336,
                                              0xFFE57373, [this]() { OnConfirmClicked(); }));
        _menu->AddButton(CreateCenteredButton("NO", offsets[1], buttonWidth, buttonHeight, 0xFF424242,
                                              0xFF616161, [this]() { OnCancelClicked(); }));
    }

    void ConfirmQuitMenu::SetOnConfirm(std::function<void()> callback) {
        _onConfirm = std::move(callback);
    }

    void ConfirmQuitMenu::SetOnCancel(std::function<void()> callback) {
        _onCancel = std::move(callback);
    }

    void ConfirmQuitMenu::OnConfirmClicked() {
        LOG_INFO("[ConfirmQuitMenu] Quit confirmed");
        if (_onConfirm) {
            _onConfirm();
        }
    }

    void ConfirmQuitMenu::OnCancelClicked() {
        LOG_INFO("[ConfirmQuitMenu] Quit canceled");
        if (_onCancel) {
            _onCancel();
        } else {
            Hide();
        }
    }
}  // namespace Game
