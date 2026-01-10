/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** ConfirmQuitMenu - Quit confirmation dialog (business logic)
*/

#include "Menu/ConfirmQuitMenu.hpp"
#include "../common/Logger/Logger.hpp"

namespace Game {
    ConfirmQuitMenu::ConfirmQuitMenu(UI::IUIFactory &uiFactory) : _uiFactory(uiFactory) {
        _menu = _uiFactory.CreateMenu();
    }

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

        _menu->AddButton(
            makeButton("YES, QUIT", offsets[0], 0xFFF44336, 0xFFE57373, [this]() { OnConfirmClicked(); }));
        _menu->AddButton(
            makeButton("NO", offsets[1], 0xFF424242, 0xFF616161, [this]() { OnCancelClicked(); }));
    }

    void ConfirmQuitMenu::Update() {
        if (_menu) {
            _menu->Update();
        }
    }

    void ConfirmQuitMenu::Render() {
        if (_menu) {
            _menu->Render();
        }
    }

    void ConfirmQuitMenu::Show() {
        if (_menu) {
            _menu->SetVisible(true);
        }
    }

    void ConfirmQuitMenu::Hide() {
        if (_menu) {
            _menu->SetVisible(false);
        }
    }

    bool ConfirmQuitMenu::IsVisible() const {
        return _menu && _menu->IsVisible();
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
