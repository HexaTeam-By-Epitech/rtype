/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ITextInput - Abstract text input interface for UI system
*/

#pragma once

#include <functional>
#include <string>

namespace UI {
    /**
     * @brief Alignment modes relative to the current window.
     */
    enum class Align;

    /**
     * @brief Abstract interface for UI text input fields.
     *
     * This interface defines a text input box with the following features:
     * - Real-time text input with keyboard handling
     * - Max character limit
     * - Regex-based validation (prevents invalid characters from being typed)
     * - Configurable appearance (size, position, colors)
     * - Placeholder text when empty
     * - Focus/blur states with visual feedback (border colors)
     * - Blinking cursor when active
     * - Optional callback on text change
     */
    class ITextInput {
       public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~ITextInput() = default;

        /**
         * @brief Update the text input internal state (focus, cursor, input).
         * @note Should be called once per frame.
         */
        virtual void Update() = 0;

        /**
         * @brief Render the text input box.
         * @note Must be called between the backend BeginDrawing/EndDrawing calls.
         */
        virtual void Render() = 0;

        /**
         * @brief Set callback invoked when text changes.
         * @param callback Function called when the text content changes.
         */
        virtual void SetOnTextChanged(std::function<void(const std::string &)> callback) = 0;

        /**
         * @brief Set the top-left position of the text input.
         * @param x X position in pixels.
         * @param y Y position in pixels.
         */
        virtual void SetPosition(float x, float y) = 0;

        /**
         * @brief Get the current top-left position of the text input.
         * @param x Output X position in pixels.
         * @param y Output Y position in pixels.
         */
        virtual void GetPosition(float &x, float &y) const = 0;

        /**
         * @brief Set the text input size.
         * @param width Width in pixels.
         * @param height Height in pixels.
         */
        virtual void SetSize(float width, float height) = 0;

        /**
         * @brief Get the current size of the text input.
         * @param width Output width in pixels.
         * @param height Output height in pixels.
         */
        virtual void GetSize(float &width, float &height) const = 0;

        /**
         * @brief Set the background color of the text input box.
         * @param color Color in ARGB format (0xAARRGGBB).
         */
        virtual void SetBackgroundColor(unsigned int color) = 0;

        /**
         * @brief Set the border color when the input is not focused.
         * @param color Color in ARGB format (0xAARRGGBB).
         */
        virtual void SetBorderColor(unsigned int color) = 0;

        /**
         * @brief Set the border color when the input is focused/active.
         * @param color Color in ARGB format (0xAARRGGBB).
         */
        virtual void SetActiveBorderColor(unsigned int color) = 0;

        /**
         * @brief Set the text color.
         * @param color Color in ARGB format (0xAARRGGBB).
         */
        virtual void SetTextColor(unsigned int color) = 0;

        /**
         * @brief Set the placeholder text color (when input is empty).
         * @param color Color in ARGB format (0xAARRGGBB).
         */
        virtual void SetPlaceholderColor(unsigned int color) = 0;

        /**
         * @brief Set the text size in pixels.
         * @param size Font size.
         */
        virtual void SetTextSize(int size) = 0;

        /**
         * @brief Set the placeholder text displayed when the input is empty.
         * @param placeholder Placeholder string.
         */
        virtual void SetPlaceholder(const std::string &placeholder) = 0;

        /**
         * @brief Set the maximum number of characters allowed.
         * @param maxLength Maximum character count (0 = unlimited).
         */
        virtual void SetMaxLength(size_t maxLength) = 0;

        /**
         * @brief Set a regex pattern to restrict allowed characters.
         * @param regexPattern Regex pattern string (e.g., "[a-zA-Z0-9]+" for alphanumeric only).
         *                     Empty string = no restriction.
         * @note Validation is applied in real-time: invalid characters are rejected on input.
         */
        virtual void SetValidationRegex(const std::string &regexPattern) = 0;

        /**
         * @brief Get the current text content.
         * @return Current text string.
         */
        [[nodiscard]] virtual const std::string &GetText() const = 0;

        /**
         * @brief Set the text content programmatically.
         * @param text New text content.
         */
        virtual void SetText(const std::string &text) = 0;

        /**
         * @brief Clear the text content.
         */
        virtual void Clear() = 0;

        /**
         * @brief Check if the text input is currently focused/active.
         * @return true if focused.
         */
        [[nodiscard]] virtual bool IsFocused() const = 0;

        /**
         * @brief Set focus state programmatically.
         * @param focused true to focus, false to blur.
         */
        virtual void SetFocused(bool focused) = 0;

        /**
         * @brief Set the font handle for rendering text.
         * @param fontHandle Font handle (-1 for default font).
         */
        virtual void SetFont(int fontHandle) = 0;

        /**
         * @brief Set alignment mode relative to the window.
         * @param align Alignment mode.
         */
        virtual void SetAlign(Align align) = 0;

        /**
         * @brief Get the current alignment mode.
         * @return Alignment mode.
         */
        [[nodiscard]] virtual Align GetAlign() const = 0;

        /**
         * @brief Apply the current alignment mode to the position.
         * @note This modifies the position based on the align setting.
         */
        virtual void ApplyAlignment() = 0;

        /**
         * @brief Enable or disable the text input.
         * @param enabled true to enable, false to disable.
         */
        virtual void SetEnabled(bool enabled) = 0;

        /**
         * @brief Check if the text input is enabled.
         * @return true if enabled.
         */
        [[nodiscard]] virtual bool IsEnabled() const = 0;
    };
}  // namespace UI
