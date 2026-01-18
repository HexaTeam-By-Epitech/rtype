/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RaylibButton - Raylib implementation of IButton
*/

#include <math.h>

#include "UI/Raylib/RaylibButton.hpp"
#include "UI/TextUtils.hpp"

namespace UI {
    RaylibButton::RaylibButton(Graphics::IGraphics &graphics) : _graphics(graphics) {}

    void RaylibButton::Update() {
        if (!_enabled) {
            _state = ButtonState::NORMAL;
            return;
        }

        bool mouseOver = IsMouseOver();
        bool mouseDown = _graphics.IsMouseButtonDown(0);  // Left mouse button

        if (mouseOver) {
            if (mouseDown) {
                _state = ButtonState::PRESSED;
            } else {
                _state = ButtonState::HOVERED;

                // Detect click (mouse was down and now released over button)
                if (_wasMouseDown && !mouseDown && _callback) {
                    _callback();
                }
            }
        } else {
            _state = ButtonState::NORMAL;
        }

        _wasMouseDown = mouseDown && mouseOver;
    }

    void RaylibButton::Render() {
        if (!_enabled) {
            // Render disabled state (could use a different color)
            constexpr unsigned int disabledColor = 0xFF606060;  // Darker gray
            _graphics.DrawRectFilled(static_cast<int>(_x), static_cast<int>(_y), static_cast<int>(_width),
                                     static_cast<int>(_height), disabledColor);
            return;
        }

        // Choose color based on state (focused or hovered takes priority)
        unsigned int currentColor = _backgroundColor;
        if (_focused) {
            currentColor = _hoverColor;  // Use hover color when focused
        } else if (_state == ButtonState::HOVERED || _state == ButtonState::PRESSED) {
            currentColor = _hoverColor;
        }

        // Draw filled rectangle for button background
        _graphics.DrawRectFilled(static_cast<int>(_x), static_cast<int>(_y), static_cast<int>(_width),
                                 static_cast<int>(_height), currentColor);

        // Draw border - blue if focused, black otherwise
        unsigned int borderColor = _focused ? _focusColor : 0xFF000000;
        _graphics.DrawRect(static_cast<int>(_x), static_cast<int>(_y), static_cast<int>(_width),
                           static_cast<int>(_height), borderColor);

        // Draw thicker border when focused (draw inner border)
        if (_focused) {
            _graphics.DrawRect(static_cast<int>(_x) + 1, static_cast<int>(_y) + 1,
                               static_cast<int>(_width) - 2, static_cast<int>(_height) - 2, borderColor);
        }

        // ===== Centered text rendering =====
        if (!_text.empty()) {
            const int textWidth = TextUtils::EstimateTextWidth(_text, _textSize);
            const int textHeight = _graphics.GetFontHeight(_fontHandle, _textSize);

            const int textX = static_cast<int>(_x + ((_width - static_cast<float>(textWidth)) / 2.0f));
            const int textY = static_cast<int>(_y + ((_height - static_cast<float>(textHeight)) / 2.0f));

            _graphics.DrawText(_fontHandle, _text.c_str(), textX, textY, _textSize, _textColor);
        }
    }

    void RaylibButton::SetCallback(std::function<void()> callback) {
        _callback = std::move(callback);
    }

    void RaylibButton::SetPosition(float x, float y) {
        _x = x;
        _y = y;
    }

    void RaylibButton::GetPosition(float &x, float &y) const {
        x = _x;
        y = _y;
    }

    void RaylibButton::SetSize(float width, float height) {
        _width = width;
        _height = height;
        ApplyAlignment();
    }

    void RaylibButton::GetSize(float &width, float &height) const {
        width = _width;
        height = _height;
    }

    void RaylibButton::SetAlign(Align align) {
        _align = align;
        ApplyAlignment();
    }

    Align RaylibButton::GetAlign() const {
        return _align;
    }

    void RaylibButton::ApplyAlignment() {
        if (_align == Align::NONE) {
            return;
        }

        const auto winW = static_cast<float>(_graphics.GetWindowWidth());
        const auto winH = static_cast<float>(_graphics.GetWindowHeight());

        if (_align == Align::CENTER_HORIZONTAL || _align == Align::CENTER_BOTH) {
            _x = (winW - _width) / 2.0F;
        }
        if (_align == Align::CENTER_VERTICAL || _align == Align::CENTER_BOTH) {
            _y = (winH - _height) / 2.0F;
        }
    }

    void RaylibButton::SetBackgroundColor(unsigned int color) {
        _backgroundColor = color;
    }

    void RaylibButton::SetHoverColor(unsigned int color) {
        _hoverColor = color;
    }

    ButtonState RaylibButton::GetState() const {
        return _state;
    }

    bool RaylibButton::IsEnabled() const {
        return _enabled;
    }

    void RaylibButton::SetEnabled(bool enabled) {
        _enabled = enabled;
    }

    bool RaylibButton::IsMouseOver() const {
        float mouseX = 0;
        float mouseY = 0;
        _graphics.GetMousePosition(mouseX, mouseY);

        return mouseX >= _x && mouseX <= _x + _width && mouseY >= _y && mouseY <= _y + _height;
    }

    void RaylibButton::SetText(const std::string &text) {
        _text = text;
    }

    const std::string &RaylibButton::GetText() const {
        return _text;
    }

    void RaylibButton::SetTextSize(int size) {
        _textSize = size;
    }

    int RaylibButton::GetTextSize() const {
        return _textSize;
    }

    void RaylibButton::SetTextColor(unsigned int color) {
        _textColor = color;
    }

    unsigned int RaylibButton::GetTextColor() const {
        return _textColor;
    }

    void RaylibButton::SetFont(int fontHandle) {
        _fontHandle = fontHandle;
    }

    int RaylibButton::GetFont() const {
        return _fontHandle;
    }

    void RaylibButton::SetFocused(bool focused) {
        _focused = focused;
    }

    bool RaylibButton::IsFocused() const {
        return _focused;
    }

    void RaylibButton::TriggerClick() {
        if (_enabled && _callback) {
            _callback();
        }
    }
}  // namespace UI
