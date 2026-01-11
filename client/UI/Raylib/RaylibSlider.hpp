/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RaylibSlider - Raylib implementation of ISlider
*/

#pragma once

#include <functional>
#include <string>

#include "Graphics/IGraphics.hpp"
#include "UI/IButton.hpp"
#include "UI/ISlider.hpp"

namespace UI {
    /**
     * @brief Raylib implementation of the ISlider interface.
     *
     * Features:
     * - Draggable handle
     * - Click on track to jump to position
     * - Visual feedback (hover, pressed)
     * - Configurable range and colors
     */
    class RaylibSlider : public ISlider {
       public:
        /**
         * @brief Construct a new RaylibSlider.
         * @param graphics Graphics wrapper used for input and drawing.
         */
        explicit RaylibSlider(Graphics::IGraphics &graphics);

        /**
         * @brief Destroy the RaylibSlider.
         */
        ~RaylibSlider() override = default;

        /** @copydoc UI::ISlider::Update */
        void Update() override;

        /** @copydoc UI::ISlider::Render */
        void Render() override;

        /** @copydoc UI::ISlider::SetOnValueChanged */
        void SetOnValueChanged(std::function<void(float)> callback) override;

        /** @copydoc UI::ISlider::SetPosition */
        void SetPosition(float x, float y) override;

        /** @copydoc UI::ISlider::GetPosition */
        void GetPosition(float &x, float &y) const override;

        /** @copydoc UI::ISlider::SetSize */
        void SetSize(float width, float height) override;

        /** @copydoc UI::ISlider::GetSize */
        void GetSize(float &width, float &height) const override;

        /** @copydoc UI::ISlider::SetTrackColor */
        void SetTrackColor(unsigned int color) override;

        /** @copydoc UI::ISlider::SetFilledColor */
        void SetFilledColor(unsigned int color) override;

        /** @copydoc UI::ISlider::SetHandleColor */
        void SetHandleColor(unsigned int color) override;

        /** @copydoc UI::ISlider::SetHandleHoverColor */
        void SetHandleHoverColor(unsigned int color) override;

        /** @copydoc UI::ISlider::SetMinValue */
        void SetMinValue(float minValue) override;

        /** @copydoc UI::ISlider::SetMaxValue */
        void SetMaxValue(float maxValue) override;

        /** @copydoc UI::ISlider::SetValue */
        void SetValue(float value) override;

        /** @copydoc UI::ISlider::GetValue */
        [[nodiscard]] float GetValue() const override;

        /** @copydoc UI::ISlider::SetAlign */
        void SetAlign(Align align) override;

        /** @copydoc UI::ISlider::GetAlign */
        [[nodiscard]] Align GetAlign() const override;

        /** @copydoc UI::ISlider::ApplyAlignment */
        void ApplyAlignment() override;

        /** @copydoc UI::ISlider::SetEnabled */
        void SetEnabled(bool enabled) override;

        /** @copydoc UI::ISlider::IsEnabled */
        [[nodiscard]] bool IsEnabled() const override;

       private:
        /**
         * @brief Check whether the mouse cursor is over the handle.
         */
        [[nodiscard]] bool IsMouseOverHandle() const;

        /**
         * @brief Check whether the mouse cursor is over the track.
         */
        [[nodiscard]] bool IsMouseOverTrack() const;

        /**
         * @brief Update value based on mouse position.
         */
        void UpdateValueFromMouse();

        /**
         * @brief Clamp value to [min, max] range.
         */
        void ClampValue();

        Graphics::IGraphics &_graphics;
        std::function<void(float)> _onValueChanged{};

        float _x{0.0F};
        float _y{0.0F};
        float _width{200.0F};
        float _height{10.0F};

        unsigned int _trackColor{0xFF505050};
        unsigned int _filledColor{0xFF4CAF50};
        unsigned int _handleColor{0xFFFFFFFF};
        unsigned int _handleHoverColor{0xFFE0E0E0};

        float _minValue{0.0F};
        float _maxValue{100.0F};
        float _value{50.0F};

        bool _enabled{true};
        bool _isDragging{false};
        bool _isHovered{false};

        Align _align{Align::NONE};

        static constexpr float HANDLE_RADIUS = 12.0F;      // Increased from 8.0F for better visibility
        static constexpr float TRACK_HEIGHT_RATIO = 0.4F;  // Track height relative to total height
    };
}  // namespace UI
