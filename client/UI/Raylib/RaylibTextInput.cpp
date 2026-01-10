/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RaylibTextInput - Raylib implementation of ITextInput
*/

#include "UI/Raylib/RaylibTextInput.hpp"

#include <raylib.h>
#include <algorithm>

namespace UI {
    RaylibTextInput::RaylibTextInput(Graphics::IGraphics &graphics) : _graphics(graphics) {}

    void RaylibTextInput::Update() {
        if (!_enabled) {
            return;
        }

        // Handle focus click
        bool mousePressed = _graphics.IsMouseButtonPressed(0);
        if (mousePressed) {
            bool wasOver = IsMouseOver();
            SetFocused(wasOver);
        }

        // Handle keyboard input when focused
        if (_focused) {
            // Update cursor blink
            _cursorBlinkTimer += _graphics.GetFrameTime();
            if (_cursorBlinkTimer >= CURSOR_BLINK_RATE) {
                _cursorBlinkTimer = 0.0F;
                _cursorVisible = !_cursorVisible;
            }

            // Handle paste (Ctrl+V or Cmd+V) - cross-platform
            bool ctrlDown = _graphics.IsKeyDown(KEY_LEFT_CONTROL) || _graphics.IsKeyDown(KEY_RIGHT_CONTROL);
            bool cmdDown = _graphics.IsKeyDown(KEY_LEFT_SUPER) || _graphics.IsKeyDown(KEY_RIGHT_SUPER);

            if (_graphics.IsKeyPressed(KEY_V) && (ctrlDown || cmdDown)) {
                // Get clipboard text
                const char *clipboardText = GetClipboardText();
                if (clipboardText != nullptr) {
                    std::string pasteText(clipboardText);

                    // Validate and add each character from clipboard
                    for (char c : pasteText) {
                        // Only paste printable characters
                        if (c >= 32 && c <= 126) {
                            // Check max length
                            if (_maxLength == 0 || _text.length() < _maxLength) {
                                // Check regex validation
                                if (IsCharValid(c)) {
                                    _text += c;
                                } else {
                                    // Stop pasting if we hit an invalid character
                                    break;
                                }
                            } else {
                                // Stop if we reached max length
                                break;
                            }
                        }
                    }

                    if (_onTextChanged) {
                        _onTextChanged(_text);
                    }
                    _cursorVisible = true;
                    _cursorBlinkTimer = 0.0F;
                }
            }

            // Handle backspace (with key repeat support)
            if (_graphics.IsKeyDown(KEY_BACKSPACE) && !_text.empty()) {
                // First press or repeat delay exceeded
                bool shouldDelete = false;

                if (_graphics.IsKeyPressed(KEY_BACKSPACE)) {
                    // Initial press - delete immediately and start timer
                    shouldDelete = true;
                    _backspaceTimer = 0.0F;
                } else {
                    // Key is held down - check repeat timing
                    _backspaceTimer += _graphics.GetFrameTime();

                    // Initial delay before repeat starts
                    if (_backspaceTimer >= BACKSPACE_REPEAT_DELAY) {
                        // Calculate how many characters to delete based on repeat rate
                        float timeSinceDelay = _backspaceTimer - BACKSPACE_REPEAT_DELAY;
                        int repeatCount = static_cast<int>(timeSinceDelay / BACKSPACE_REPEAT_RATE);

                        if (repeatCount > _lastBackspaceRepeat) {
                            shouldDelete = true;
                            _lastBackspaceRepeat = repeatCount;
                        }
                    }
                }

                if (shouldDelete) {
                    _text.pop_back();
                    if (_onTextChanged) {
                        _onTextChanged(_text);
                    }
                    _cursorVisible = true;
                    _cursorBlinkTimer = 0.0F;
                }
            } else {
                // Reset backspace repeat counter when key is released
                _backspaceTimer = 0.0F;
                _lastBackspaceRepeat = 0;
            }

            // Handle character input
            int key = _graphics.GetCharPressed();
            while (key > 0) {
                // Check if character is printable and within limits
                if (key >= 32 && key <= 126) {
                    // Check max length
                    if (_maxLength == 0 || _text.length() < _maxLength) {
                        // Check regex validation
                        char c = static_cast<char>(key);
                        if (IsCharValid(c)) {
                            _text += c;
                            if (_onTextChanged) {
                                _onTextChanged(_text);
                            }
                            _cursorVisible = true;
                            _cursorBlinkTimer = 0.0F;
                        }
                    }
                }
                key = _graphics.GetCharPressed();
            }
        }
    }

    void RaylibTextInput::Render() {
        // Draw background
        _graphics.DrawRectangle(static_cast<int>(_x), static_cast<int>(_y), static_cast<int>(_width),
                                static_cast<int>(_height), _backgroundColor);

        // Draw border (different color when focused)
        unsigned int borderColor = _focused ? _activeBorderColor : _borderColor;
        _graphics.DrawRectangleLines(static_cast<int>(_x), static_cast<int>(_y), static_cast<int>(_width),
                                     static_cast<int>(_height), borderColor);

        // Draw thicker border when focused
        if (_focused) {
            _graphics.DrawRectangleLines(static_cast<int>(_x + 1), static_cast<int>(_y + 1),
                                         static_cast<int>(_width - 2), static_cast<int>(_height - 2),
                                         borderColor);
        }

        // Calculate text position (left-aligned with padding)
        float textX = _x + TEXT_PADDING;
        float textY = _y + (_height / 2.0F) - (_textSize / 2.0F);

        // Draw text or placeholder
        if (_text.empty() && !_placeholder.empty()) {
            // Draw placeholder
            _graphics.DrawText(_placeholder.c_str(), static_cast<int>(textX), static_cast<int>(textY),
                               _textSize, _placeholderColor);
        } else if (!_text.empty()) {
            // Draw actual text
            _graphics.DrawText(_text.c_str(), static_cast<int>(textX), static_cast<int>(textY), _textSize,
                               _textColor);
        }

        // Draw blinking cursor when focused
        if (_focused && _cursorVisible && _enabled) {
            // Use Raylib's MeasureText for accurate positioning
            int textWidth = 0;
            if (!_text.empty()) {
                textWidth = MeasureText(_text.c_str(), _textSize);
            }
            float cursorX = textX + static_cast<float>(textWidth);
            float cursorY = textY;
            float cursorHeight = static_cast<float>(_textSize);

            _graphics.DrawRectangle(static_cast<int>(cursorX), static_cast<int>(cursorY), 2,
                                    static_cast<int>(cursorHeight), _textColor);
        }
    }

    void RaylibTextInput::SetOnTextChanged(std::function<void(const std::string &)> callback) {
        _onTextChanged = std::move(callback);
    }

    void RaylibTextInput::SetPosition(float x, float y) {
        _x = x;
        _y = y;
    }

    void RaylibTextInput::GetPosition(float &x, float &y) const {
        x = _x;
        y = _y;
    }

    void RaylibTextInput::SetSize(float width, float height) {
        _width = width;
        _height = height;
    }

    void RaylibTextInput::GetSize(float &width, float &height) const {
        width = _width;
        height = _height;
    }

    void RaylibTextInput::SetBackgroundColor(unsigned int color) {
        _backgroundColor = color;
    }

    void RaylibTextInput::SetBorderColor(unsigned int color) {
        _borderColor = color;
    }

    void RaylibTextInput::SetActiveBorderColor(unsigned int color) {
        _activeBorderColor = color;
    }

    void RaylibTextInput::SetTextColor(unsigned int color) {
        _textColor = color;
    }

    void RaylibTextInput::SetPlaceholderColor(unsigned int color) {
        _placeholderColor = color;
    }

    void RaylibTextInput::SetTextSize(int size) {
        _textSize = size;
    }

    void RaylibTextInput::SetPlaceholder(const std::string &placeholder) {
        _placeholder = placeholder;
    }

    void RaylibTextInput::SetMaxLength(size_t maxLength) {
        _maxLength = maxLength;
        // Truncate existing text if needed
        if (_maxLength > 0 && _text.length() > _maxLength) {
            _text = _text.substr(0, _maxLength);
        }
    }

    void RaylibTextInput::SetValidationRegex(const std::string &regexPattern) {
        _regexPattern = regexPattern;
        if (!regexPattern.empty()) {
            try {
                _validationRegex = std::make_unique<std::regex>(regexPattern);
            } catch (const std::regex_error &) {
                _validationRegex.reset();
            }
        } else {
            _validationRegex.reset();
        }
    }

    const std::string &RaylibTextInput::GetText() const {
        return _text;
    }

    void RaylibTextInput::SetText(const std::string &text) {
        _text = text;
        // Apply max length constraint
        if (_maxLength > 0 && _text.length() > _maxLength) {
            _text = _text.substr(0, _maxLength);
        }
    }

    void RaylibTextInput::Clear() {
        _text.clear();
    }

    bool RaylibTextInput::IsFocused() const {
        return _focused;
    }

    void RaylibTextInput::SetFocused(bool focused) {
        _focused = focused;
        if (_focused) {
            _cursorVisible = true;
            _cursorBlinkTimer = 0.0F;
        }
    }

    void RaylibTextInput::SetFont(int fontHandle) {
        _fontHandle = fontHandle;
    }

    void RaylibTextInput::SetAlign(Align align) {
        _align = align;
    }

    Align RaylibTextInput::GetAlign() const {
        return _align;
    }

    void RaylibTextInput::ApplyAlignment() {
        int screenWidth = _graphics.GetScreenWidth();
        int screenHeight = _graphics.GetScreenHeight();

        switch (_align) {
            case Align::CENTER_HORIZONTAL:
                _x = (static_cast<float>(screenWidth) - _width) / 2.0F;
                break;
            case Align::CENTER_VERTICAL:
                _y = (static_cast<float>(screenHeight) - _height) / 2.0F;
                break;
            case Align::CENTER_BOTH:
                _x = (static_cast<float>(screenWidth) - _width) / 2.0F;
                _y = (static_cast<float>(screenHeight) - _height) / 2.0F;
                break;
            case Align::NONE:
            default:
                break;
        }
    }

    void RaylibTextInput::SetEnabled(bool enabled) {
        _enabled = enabled;
        if (!_enabled) {
            _focused = false;
        }
    }

    bool RaylibTextInput::IsEnabled() const {
        return _enabled;
    }

    bool RaylibTextInput::IsMouseOver() const {
        int mouseX = _graphics.GetMouseX();
        int mouseY = _graphics.GetMouseY();

        return static_cast<float>(mouseX) >= _x && static_cast<float>(mouseX) <= _x + _width &&
               static_cast<float>(mouseY) >= _y && static_cast<float>(mouseY) <= _y + _height;
    }

    bool RaylibTextInput::IsCharValid(char c) const {
        if (!_validationRegex) {
            return true;  // No validation = all chars allowed
        }

        std::string singleChar(1, c);
        return std::regex_match(singleChar, *_validationRegex);
    }
}  // namespace UI
