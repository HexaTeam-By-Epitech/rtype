/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RaylibMenu - Raylib implementation of IMenu
*/

#include "UI/Raylib/RaylibMenu.hpp"
#include "UI/IButton.hpp"

namespace UI {
    RaylibMenu::RaylibMenu(Graphics::IGraphics &graphics) : _graphics(graphics) {}

    void RaylibMenu::Update() {
        if (!_visible) {
            return;
        }

        // Update all buttons
        for (auto &button : _buttons) {
            if (button) {
                button->Update();
            }
        }
    }

    void RaylibMenu::Render() {
        if (!_visible) {
            return;
        }

        // Render all buttons
        for (auto &button : _buttons) {
            if (button) {
                button->Render();
            }
        }
    }

    void RaylibMenu::AddButton(std::shared_ptr<IButton> button) {
        if (button) {
            _buttons.push_back(std::move(button));
        }
    }

    void RaylibMenu::RemoveButton(size_t index) {
        if (index < _buttons.size()) {
            _buttons.erase(_buttons.begin() + index);
        }
    }

    std::shared_ptr<IButton> RaylibMenu::GetButton(size_t index) {
        if (index < _buttons.size()) {
            return _buttons[index];
        }
        return nullptr;
    }

    size_t RaylibMenu::GetButtonCount() const {
        return _buttons.size();
    }

    void RaylibMenu::Clear() {
        _buttons.clear();
    }

    void RaylibMenu::SetVisible(bool visible) {
        _visible = visible;
    }

    bool RaylibMenu::IsVisible() const {
        return _visible;
    }
}  // namespace UI
