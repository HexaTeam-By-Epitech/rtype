/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** IButton - Abstract button interface for UI system
*/

#pragma once

#include <functional>
#include <string>

namespace UI {
    /**
     * @brief Button state enumeration.
     */
    enum class ButtonState {
        NORMAL,   ///< Default state
        HOVERED,  ///< Mouse is over the button
        PRESSED   ///< Mouse button is held down on the button
    };

    /**
     * @brief Alignment modes relative to the current window.
     *
     * Alignment is applied by calling ApplyAlignment() (or implicitly by some setters).
     *
     * Notes:
     * - CENTER_HORIZONTAL: modifies X only
     * - CENTER_VERTICAL: modifies Y only
     * - CENTER_BOTH: modifies both X and Y
     */
    enum class Align {
        NONE,               ///< No alignment
        CENTER_HORIZONTAL,  ///< Center on the X axis
        CENTER_VERTICAL,    ///< Center on the Y axis
        CENTER_BOTH         ///< Center on both axes
    };

    /**
     * @brief Abstract interface for UI buttons.
     *
     * This interface defines a minimal, backend-agnostic contract for interactive buttons:
     * - input update (hover/pressed/click)
     * - rendering
     * - visuals (size, position, colors)
     * - label text (font, color, size)
     * - optional alignment helpers relative to the window
     */
    class IButton {
       public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IButton() = default;

        /**
         * @brief Update the button internal state (hover/pressed) and trigger callbacks.
         * @note Should be called once per frame.
         */
        virtual void Update() = 0;

        /**
         * @brief Render the button.
         * @note Must be called between the backend BeginDrawing/EndDrawing calls.
         */
        virtual void Render() = 0;

        /**
         * @brief Set callback invoked on click.
         * @param callback Function called when the button is clicked.
         */
        virtual void SetCallback(std::function<void()> callback) = 0;

        /**
         * @brief Set the top-left position of the button.
         * @param x X position in pixels.
         * @param y Y position in pixels.
         */
        virtual void SetPosition(float x, float y) = 0;

        /**
         * @brief Get the current top-left position of the button.
         * @param x Output X position in pixels.
         * @param y Output Y position in pixels.
         */
        virtual void GetPosition(float &x, float &y) const = 0;

        /**
         * @brief Set the button size.
         * @param width Width in pixels.
         * @param height Height in pixels.
         */
        virtual void SetSize(float width, float height) = 0;

        /**
         * @brief Get the current size of the button.
         * @param width Output width in pixels.
         * @param height Output height in pixels.
         */
        virtual void GetSize(float &width, float &height) const = 0;

        /**
         * @brief Set button background color.
         * @param color Color in 0xAARRGGBB format.
         */
        virtual void SetBackgroundColor(unsigned int color) = 0;

        /**
         * @brief Set button hover color.
         * @param color Color in 0xAARRGGBB format.
         */
        virtual void SetHoverColor(unsigned int color) = 0;

        /**
         * @brief Get current button state.
         * @return ButtonState.
         */
        [[nodiscard]] virtual ButtonState GetState() const = 0;

        /**
         * @brief Check if button is enabled.
         * @return true if enabled.
         */
        [[nodiscard]] virtual bool IsEnabled() const = 0;

        /**
         * @brief Enable/disable the button.
         * @param enabled true to enable.
         */
        virtual void SetEnabled(bool enabled) = 0;

        // ===== Text / Label (centered) =====

        /**
         * @brief Set button label text.
         * @param text UTF-8 text.
         */
        virtual void SetText(const std::string &text) = 0;

        /**
         * @brief Get button label text.
         * @return Reference to internal string.
         */
        [[nodiscard]] virtual const std::string &GetText() const = 0;

        /**
         * @brief Set label font size in pixels.
         * @param size Font size.
         */
        virtual void SetTextSize(int size) = 0;

        /**
         * @brief Get label font size.
         * @return Font size in pixels.
         */
        [[nodiscard]] virtual int GetTextSize() const = 0;

        /**
         * @brief Set label color.
         * @param color Color in 0xAARRGGBB format.
         */
        virtual void SetTextColor(unsigned int color) = 0;

        /**
         * @brief Get label color.
         * @return Color in 0xAARRGGBB format.
         */
        [[nodiscard]] virtual unsigned int GetTextColor() const = 0;

        /**
         * @brief Set font handle to use for button text.
         * @param fontHandle Font handle returned by Graphics::IGraphics::LoadFont(). Use -1 for default font.
         */
        virtual void SetFont(int fontHandle) = 0;

        /**
         * @brief Get current font handle used by this button.
         * @return Font handle (or -1 for default font).
         */
        [[nodiscard]] virtual int GetFont() const = 0;

        /**
         * @brief Set alignment mode relative to the current window.
         * @param align Desired alignment.
         * @note Implementations may call ApplyAlignment() immediately.
         */
        virtual void SetAlign(Align align) = 0;

        /**
         * @brief Get current alignment mode.
         * @return Align.
         */
        [[nodiscard]] virtual Align GetAlign() const = 0;

        /**
         * @brief Apply alignment (recomputes position based on window size).
         * @note Call this after SetSize(), or when the window is resized.
         */
        virtual void ApplyAlignment() = 0;
    };
}  // namespace UI
