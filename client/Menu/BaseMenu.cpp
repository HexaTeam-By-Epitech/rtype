/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** BaseMenu - Base class for all menus to reduce code duplication
*/

#include "Menu/BaseMenu.hpp"
#include "Audio/ISoundEffectService.hpp"

namespace Game {
    BaseMenu::BaseMenu(UI::IUIFactory &uiFactory) : _uiFactory(uiFactory) {
        _menu = _uiFactory.CreateMenu();
    }

    void BaseMenu::Update() {
        if (_menu) {
            _menu->Update();
        }
    }

    void BaseMenu::Render() {
        if (_menu) {
            _menu->Render();
        }
    }

    void BaseMenu::Show() {
        if (_menu) {
            _menu->SetVisible(true);
        }
    }

    void BaseMenu::Hide() {
        if (_menu) {
            _menu->SetVisible(false);
        }
    }

    bool BaseMenu::IsVisible() const {
        return _menu && _menu->IsVisible();
    }

    void BaseMenu::SetSoundEffectService(Audio::ISoundEffectService *soundService) {
        _soundService = soundService;
    }

    std::function<void()> BaseMenu::WrapWithClickSound(std::function<void()> callback) {
        if (!callback) {
            return callback;
        }
        return [this, cb = std::move(callback)]() {
            if (_soundService) {
                _soundService->PlayClickSound();
            }
            cb();
        };
    }

    std::shared_ptr<UI::IButton> BaseMenu::CreateCenteredButton(const char *label, float offsetY, float width,
                                                                float height, unsigned int backgroundColor,
                                                                unsigned int hoverColor,
                                                                std::function<void()> callback) {
        auto button = _uiFactory.CreateButton();
        button->SetSize(width, height);
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
        button->SetCallback(WrapWithClickSound(std::move(callback)));
        return button;
    }
}  // namespace Game
