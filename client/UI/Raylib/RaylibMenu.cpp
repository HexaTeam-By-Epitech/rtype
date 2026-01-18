/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RaylibMenu - Raylib implementation of IMenu
*/

#include "UI/Raylib/RaylibMenu.hpp"
#include <raylib.h>  // For KEY_TAB, KEY_ENTER, KEY_LEFT_SHIFT, KEY_RIGHT_SHIFT
#include <cstddef>   // For std::ptrdiff_t
#include "UI/IButton.hpp"

namespace UI {
    RaylibMenu::RaylibMenu(Graphics::IGraphics &graphics) : _graphics(graphics) {}

    void RaylibMenu::Update() {
        if (!_visible) {
            return;
        }

        // Handle keyboard navigation
        HandleKeyboardNavigation();

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
            // Adjust selected index if needed
            if (static_cast<int>(index) == _selectedIndex) {
                _selectedIndex = -1;  // Clear selection if removed button was selected
            } else if (static_cast<int>(index) < _selectedIndex) {
                _selectedIndex--;  // Adjust index if button before selected was removed
            }
            _buttons.erase(_buttons.begin() + static_cast<std::ptrdiff_t>(index));
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
        _selectedIndex = -1;  // Reset selection when clearing
    }

    void RaylibMenu::SetVisible(bool visible) {
        _visible = visible;
        // Reset selection when hiding menu
        if (!visible) {
            SetSelectedIndex(-1);
        }
    }

    bool RaylibMenu::IsVisible() const {
        return _visible;
    }

    void RaylibMenu::HandleKeyboardNavigation() {
        if (_buttons.empty()) {
            return;
        }

        bool shiftDown = _graphics.IsKeyDown(KEY_LEFT_SHIFT) || _graphics.IsKeyDown(KEY_RIGHT_SHIFT);

        // Tab / Shift+Tab for navigation
        if (_graphics.IsKeyPressed(KEY_TAB)) {
            if (shiftDown) {
                SelectPrevious();
            } else {
                SelectNext();
            }
        }

        // Enter to trigger selected button
        if (_graphics.IsKeyPressed(KEY_ENTER) || _graphics.IsKeyPressed(KEY_KP_ENTER)) {
            TriggerSelected();
        }
    }

    void RaylibMenu::SelectNext() {
        if (_buttons.empty()) {
            return;
        }

        // Clear focus from current selection
        if (_selectedIndex >= 0 && _selectedIndex < static_cast<int>(_buttons.size())) {
            if (_buttons[_selectedIndex]) {
                _buttons[_selectedIndex]->SetFocused(false);
            }
        }

        // Find next enabled button
        int startIndex = _selectedIndex;
        int buttonsCount = static_cast<int>(_buttons.size());

        do {
            _selectedIndex = (_selectedIndex + 1) % buttonsCount;
            if (_buttons[_selectedIndex] && _buttons[_selectedIndex]->IsEnabled()) {
                _buttons[_selectedIndex]->SetFocused(true);
                return;
            }
        } while (_selectedIndex != startIndex);

        // No enabled buttons found
        _selectedIndex = -1;
    }

    void RaylibMenu::SelectPrevious() {
        if (_buttons.empty()) {
            return;
        }

        // Clear focus from current selection
        if (_selectedIndex >= 0 && _selectedIndex < static_cast<int>(_buttons.size())) {
            if (_buttons[_selectedIndex]) {
                _buttons[_selectedIndex]->SetFocused(false);
            }
        }

        // Find previous enabled button
        int startIndex = _selectedIndex;
        int buttonsCount = static_cast<int>(_buttons.size());

        // Handle -1 case (no selection)
        if (_selectedIndex < 0) {
            _selectedIndex = buttonsCount;
        }

        do {
            _selectedIndex = (_selectedIndex - 1 + buttonsCount) % buttonsCount;
            if (_buttons[_selectedIndex] && _buttons[_selectedIndex]->IsEnabled()) {
                _buttons[_selectedIndex]->SetFocused(true);
                return;
            }
        } while (_selectedIndex != startIndex);

        // No enabled buttons found
        _selectedIndex = -1;
    }

    void RaylibMenu::TriggerSelected() {
        if (_selectedIndex >= 0 && _selectedIndex < static_cast<int>(_buttons.size())) {
            if (_buttons[_selectedIndex]) {
                _buttons[_selectedIndex]->TriggerClick();
            }
        }
    }

    int RaylibMenu::GetSelectedIndex() const {
        return _selectedIndex;
    }

    void RaylibMenu::SetSelectedIndex(int index) {
        // Clear previous selection
        if (_selectedIndex >= 0 && _selectedIndex < static_cast<int>(_buttons.size())) {
            if (_buttons[_selectedIndex]) {
                _buttons[_selectedIndex]->SetFocused(false);
            }
        }

        // Set new selection
        if (index >= 0 && index < static_cast<int>(_buttons.size())) {
            _selectedIndex = index;
            if (_buttons[_selectedIndex]) {
                _buttons[_selectedIndex]->SetFocused(true);
            }
        } else {
            _selectedIndex = -1;
        }
    }
}  // namespace UI
