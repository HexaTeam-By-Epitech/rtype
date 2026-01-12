/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ISlider - Abstract slider interface for UI system
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
     * @brief Abstract interface for UI sliders.
     *
     * This interface defines a slider control with the following features:
     * - Draggable handle to select a value within a range
     * - Visual feedback (track, filled portion, handle)
     * - Mouse interaction (click on track, drag handle)
     * - Configurable range (min, max)
     * - Configurable appearance (colors, sizes)
     * - Optional callback on value change
     */
    class ISlider {
       public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~ISlider() = default;

        /**
         * @brief Update the slider internal state (hover, drag).
         * @note Should be called once per frame.
         */
        virtual void Update() = 0;

        /**
         * @brief Render the slider.
         * @note Must be called between the backend BeginDrawing/EndDrawing calls.
         */
        virtual void Render() = 0;

        /**
         * @brief Set callback invoked when value changes.
         * @param callback Function called when the slider value changes.
         */
        virtual void SetOnValueChanged(std::function<void(float)> callback) = 0;

        /**
         * @brief Set the top-left position of the slider.
         * @param x X position in pixels.
         * @param y Y position in pixels.
         */
        virtual void SetPosition(float x, float y) = 0;

        /**
         * @brief Get the current top-left position of the slider.
         * @param x Output X position in pixels.
         * @param y Output Y position in pixels.
         */
        virtual void GetPosition(float &x, float &y) const = 0;

        /**
         * @brief Set the slider size.
         * @param width Width in pixels (track length).
         * @param height Height in pixels (track thickness).
         */
        virtual void SetSize(float width, float height) = 0;

        /**
         * @brief Get the current size of the slider.
         * @param width Output width in pixels.
         * @param height Output height in pixels.
         */
        virtual void GetSize(float &width, float &height) const = 0;

        /**
         * @brief Set the track color (unfilled portion).
         * @param color Color in ARGB format (0xAARRGGBB).
         */
        virtual void SetTrackColor(unsigned int color) = 0;

        /**
         * @brief Set the filled track color (portion before handle).
         * @param color Color in ARGB format (0xAARRGGBB).
         */
        virtual void SetFilledColor(unsigned int color) = 0;

        /**
         * @brief Set the handle color.
         * @param color Color in ARGB format (0xAARRGGBB).
         */
        virtual void SetHandleColor(unsigned int color) = 0;

        /**
         * @brief Set the handle hover color.
         * @param color Color in ARGB format (0xAARRGGBB).
         */
        virtual void SetHandleHoverColor(unsigned int color) = 0;

        /**
         * @brief Set the minimum value.
         * @param minValue Minimum value.
         */
        virtual void SetMinValue(float minValue) = 0;

        /**
         * @brief Set the maximum value.
         * @param maxValue Maximum value.
         */
        virtual void SetMaxValue(float maxValue) = 0;

        /**
         * @brief Set the current value.
         * @param value Current value (will be clamped to [min, max]).
         */
        virtual void SetValue(float value) = 0;

        /**
         * @brief Get the current value.
         * @return Current value.
         */
        [[nodiscard]] virtual float GetValue() const = 0;

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
         * @brief Enable or disable the slider.
         * @param enabled true to enable, false to disable.
         */
        virtual void SetEnabled(bool enabled) = 0;

        /**
         * @brief Check if the slider is enabled.
         * @return true if enabled.
         */
        [[nodiscard]] virtual bool IsEnabled() const = 0;
    };
}  // namespace UI
