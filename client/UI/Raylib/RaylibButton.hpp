/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RaylibButton - Raylib implementation of IButton
*/

#pragma once

#include <functional>
#include <string>

#include "Graphics/IGraphics.hpp"
#include "UI/IButton.hpp"

namespace UI {
    /**
     * @brief Raylib implementation of the IButton interface.
     *
     * Rendering is performed through the project's Graphics::IGraphics wrapper.
     *
     * Supported features:
     * - background + hover colors
     * - click callback
     * - centered label text (font handle, size, color)
     * - alignment relative to window (center X/Y)
     */
    class RaylibButton : public IButton {
       public:
        /**
         * @brief Construct a new RaylibButton.
         * @param graphics Graphics wrapper used for input and drawing.
         */
        explicit RaylibButton(Graphics::IGraphics &graphics);

        /**
         * @brief Destroy the RaylibButton.
         */
        ~RaylibButton() override = default;

        /** @copydoc UI::IButton::Update */
        void Update() override;

        /** @copydoc UI::IButton::Render */
        void Render() override;

        /** @copydoc UI::IButton::SetCallback */
        void SetCallback(std::function<void()> callback) override;

        /** @copydoc UI::IButton::SetPosition */
        void SetPosition(float x, float y) override;

        /** @copydoc UI::IButton::GetPosition */
        void GetPosition(float &x, float &y) const override;

        /** @copydoc UI::IButton::SetSize */
        void SetSize(float width, float height) override;

        /** @copydoc UI::IButton::GetSize */
        void GetSize(float &width, float &height) const override;

        /** @copydoc UI::IButton::SetBackgroundColor */
        void SetBackgroundColor(unsigned int color) override;

        /** @copydoc UI::IButton::SetHoverColor */
        void SetHoverColor(unsigned int color) override;

        /** @copydoc UI::IButton::SetText */
        void SetText(const std::string &text) override;

        /** @copydoc UI::IButton::SetTextSize */
        void SetTextSize(int size) override;

        /** @copydoc UI::IButton::SetTextColor */
        void SetTextColor(unsigned int color) override;

        /** @copydoc UI::IButton::SetFont */
        void SetFont(int fontHandle) override;

        /** @copydoc UI::IButton::SetAlign */
        void SetAlign(Align align) override;

        /** @copydoc UI::IButton::GetAlign */
        [[nodiscard]] Align GetAlign() const override;

        /** @copydoc UI::IButton::ApplyAlignment */
        void ApplyAlignment() override;

        /** @copydoc UI::IButton::GetState */
        [[nodiscard]] ButtonState GetState() const override;

        /** @copydoc UI::IButton::IsEnabled */
        [[nodiscard]] bool IsEnabled() const override;

        /** @copydoc UI::IButton::SetEnabled */
        void SetEnabled(bool enabled) override;

        /** @copydoc UI::IButton::GetText */
        [[nodiscard]] const std::string &GetText() const override;

        /** @copydoc UI::IButton::GetTextSize */
        [[nodiscard]] int GetTextSize() const override;

        /** @copydoc UI::IButton::GetTextColor */
        [[nodiscard]] unsigned int GetTextColor() const override;

        /** @copydoc UI::IButton::GetFont */
        [[nodiscard]] int GetFont() const override;

        /** @copydoc UI::IButton::SetFocused */
        void SetFocused(bool focused) override;

        /** @copydoc UI::IButton::IsFocused */
        [[nodiscard]] bool IsFocused() const override;

        /** @copydoc UI::IButton::TriggerClick */
        void TriggerClick() override;

       private:
        /**
         * @brief Check whether the mouse cursor is currently over the button rectangle.
         */
        [[nodiscard]] bool IsMouseOver() const;

        Graphics::IGraphics &_graphics;
        std::function<void()> _callback{};

        float _x{0.0F};
        float _y{0.0F};
        float _width{100.0F};
        float _height{40.0F};

        unsigned int _backgroundColor{0xFF808080};
        unsigned int _hoverColor{0xFFA0A0A0};
        unsigned int _focusColor{0xFF4080FF};  // Blue border for focused state

        ButtonState _state{ButtonState::NORMAL};
        bool _enabled{true};
        bool _wasMouseDown{false};
        bool _focused{false};  // Keyboard focus state

        // Text properties
        std::string _text;
        int _textSize{20};
        unsigned int _textColor{0xFFFFFFFF};
        int _fontHandle{-1};

        Align _align{Align::NONE};
    };
}  // namespace UI
