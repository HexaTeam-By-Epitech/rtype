/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RaylibTextInput - Raylib implementation of ITextInput
*/

#pragma once

#include <functional>
#include <regex>
#include <string>

#include "Graphics/IGraphics.hpp"
#include "UI/IButton.hpp"
#include "UI/ITextInput.hpp"

namespace UI {
    /**
     * @brief Raylib implementation of the ITextInput interface.
     *
     * Features:
     * - Real-time keyboard input
     * - Blinking cursor when focused
     * - Max character limit
     * - Regex validation (prevents invalid characters)
     * - Click to focus/unfocus
     * - Visual feedback (border colors)
     * - Placeholder text
     */
    class RaylibTextInput : public ITextInput {
       public:
        /**
         * @brief Construct a new RaylibTextInput.
         * @param graphics Graphics wrapper used for input and drawing.
         */
        explicit RaylibTextInput(Graphics::IGraphics &graphics);

        /**
         * @brief Destroy the RaylibTextInput.
         */
        ~RaylibTextInput() override = default;

        /** @copydoc UI::ITextInput::Update */
        void Update() override;

        /** @copydoc UI::ITextInput::Render */
        void Render() override;

        /** @copydoc UI::ITextInput::SetOnTextChanged */
        void SetOnTextChanged(std::function<void(const std::string &)> callback) override;

        /** @copydoc UI::ITextInput::SetPosition */
        void SetPosition(float x, float y) override;

        /** @copydoc UI::ITextInput::GetPosition */
        void GetPosition(float &x, float &y) const override;

        /** @copydoc UI::ITextInput::SetSize */
        void SetSize(float width, float height) override;

        /** @copydoc UI::ITextInput::GetSize */
        void GetSize(float &width, float &height) const override;

        /** @copydoc UI::ITextInput::SetBackgroundColor */
        void SetBackgroundColor(unsigned int color) override;

        /** @copydoc UI::ITextInput::SetBorderColor */
        void SetBorderColor(unsigned int color) override;

        /** @copydoc UI::ITextInput::SetActiveBorderColor */
        void SetActiveBorderColor(unsigned int color) override;

        /** @copydoc UI::ITextInput::SetTextColor */
        void SetTextColor(unsigned int color) override;

        /** @copydoc UI::ITextInput::SetPlaceholderColor */
        void SetPlaceholderColor(unsigned int color) override;

        /** @copydoc UI::ITextInput::SetTextSize */
        void SetTextSize(int size) override;

        /** @copydoc UI::ITextInput::SetPlaceholder */
        void SetPlaceholder(const std::string &placeholder) override;

        /** @copydoc UI::ITextInput::SetMaxLength */
        void SetMaxLength(size_t maxLength) override;

        /** @copydoc UI::ITextInput::SetValidationRegex */
        void SetValidationRegex(const std::string &regexPattern) override;

        /** @copydoc UI::ITextInput::GetText */
        [[nodiscard]] const std::string &GetText() const override;

        /** @copydoc UI::ITextInput::SetText */
        void SetText(const std::string &text) override;

        /** @copydoc UI::ITextInput::Clear */
        void Clear() override;

        /** @copydoc UI::ITextInput::IsFocused */
        [[nodiscard]] bool IsFocused() const override;

        /** @copydoc UI::ITextInput::SetFocused */
        void SetFocused(bool focused) override;

        /** @copydoc UI::ITextInput::SetFont */
        void SetFont(int fontHandle) override;

        /** @copydoc UI::ITextInput::SetAlign */
        void SetAlign(Align align) override;

        /** @copydoc UI::ITextInput::GetAlign */
        [[nodiscard]] Align GetAlign() const override;

        /** @copydoc UI::ITextInput::ApplyAlignment */
        void ApplyAlignment() override;

        /** @copydoc UI::ITextInput::SetEnabled */
        void SetEnabled(bool enabled) override;

        /** @copydoc UI::ITextInput::IsEnabled */
        [[nodiscard]] bool IsEnabled() const override;

        /** @copydoc UI::ITextInput::SetPasswordMode */
        void SetPasswordMode(bool passwordMode) override;

        /** @copydoc UI::ITextInput::IsPasswordMode */
        [[nodiscard]] bool IsPasswordMode() const override;

       private:
        /**
         * @brief Check whether the mouse cursor is currently over the input rectangle.
         */
        [[nodiscard]] bool IsMouseOver() const;

        /**
         * @brief Validate a character against the current regex pattern.
         * @param c Character to validate.
         * @return true if valid or no regex is set.
         */
        [[nodiscard]] bool IsCharValid(char c) const;

        // ===== Helper methods for Update() to reduce cognitive complexity =====

        /**
         * @brief Handle mouse click to set focus state.
         */
        void HandleFocusClick();

        /**
         * @brief Update cursor blink animation.
         */
        void UpdateCursorBlink();

        /**
         * @brief Handle paste operation (Ctrl+V or Cmd+V).
         */
        void HandlePaste();

        /**
         * @brief Handle backspace key with repeat support.
         */
        void HandleBackspace();

        /**
         * @brief Handle regular character input.
         */
        void HandleCharacterInput();

        /**
         * @brief Add a character to the text if valid.
         * @param c Character to add.
         * @return true if character was added.
         */
        bool TryAddCharacter(char c);

        /**
         * @brief Show cursor and reset blink timer.
         */
        void ResetCursor();

        Graphics::IGraphics &_graphics;
        std::function<void(const std::string &)> _onTextChanged{};

        float _x{0.0F};
        float _y{0.0F};
        float _width{200.0F};
        float _height{40.0F};

        unsigned int _backgroundColor{0xFF2A2A2A};
        unsigned int _borderColor{0xFF505050};
        unsigned int _activeBorderColor{0xFF4CAF50};
        unsigned int _textColor{0xFFFFFFFF};
        unsigned int _placeholderColor{0xFF808080};

        std::string _text;
        std::string _placeholder;
        int _textSize{20};
        int _fontHandle{-1};

        size_t _maxLength{0};  // 0 = unlimited
        std::string _regexPattern;
        std::unique_ptr<std::regex> _validationRegex{};

        bool _focused{false};
        bool _enabled{true};
        bool _passwordMode{false};
        float _cursorBlinkTimer{0.0F};
        bool _cursorVisible{true};

        // Backspace key repeat support
        float _backspaceTimer{0.0F};
        int _lastBackspaceRepeat{0};

        Align _align{Align::NONE};

        static constexpr float CURSOR_BLINK_RATE = 0.5F;  // seconds
        static constexpr float BORDER_THICKNESS = 2.0F;
        static constexpr float TEXT_PADDING = 8.0F;
        static constexpr float BACKSPACE_REPEAT_DELAY = 0.5F;  // Initial delay before repeat (seconds)
        static constexpr float BACKSPACE_REPEAT_RATE = 0.05F;  // Time between repeats (seconds)
    };
}  // namespace UI
