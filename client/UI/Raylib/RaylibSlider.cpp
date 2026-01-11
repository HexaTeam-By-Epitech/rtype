/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RaylibSlider - Raylib implementation of ISlider
*/

#include "UI/Raylib/RaylibSlider.hpp"

#include <raylib.h>
#include <algorithm>
#include <cmath>

namespace UI {
    RaylibSlider::RaylibSlider(Graphics::IGraphics &graphics) : _graphics(graphics) {}

    void RaylibSlider::Update() {
        if (!_enabled) {
            _isDragging = false;
            _isHovered = false;
            return;
        }

        // Check if mouse is over handle
        _isHovered = IsMouseOverHandle();

        // Handle dragging
        bool mousePressed = _graphics.IsMouseButtonPressed(0);
        bool mouseDown = _graphics.IsMouseButtonDown(0);

        if (mousePressed) {
            if (_isHovered) {
                // Start dragging handle
                _isDragging = true;
            } else if (IsMouseOverTrack()) {
                // Click on track - jump to position
                UpdateValueFromMouse();
                _isDragging = true;
            }
        }

        if (!mouseDown) {
            // Stop dragging when mouse released
            _isDragging = false;
        }

        if (_isDragging) {
            // Update value while dragging
            UpdateValueFromMouse();
        }
    }

    void RaylibSlider::Render() {
        // Calculate track dimensions
        float trackHeight = _height * TRACK_HEIGHT_RATIO;
        float trackY = _y + (_height - trackHeight) / 2.0F;

        // Draw track background (unfilled)
        _graphics.DrawRectangle(static_cast<int>(_x), static_cast<int>(trackY), static_cast<int>(_width),
                                static_cast<int>(trackHeight), _trackColor);

        // Calculate filled width based on value (with division by zero protection)
        float valueRatio = 0.0F;
        if (_maxValue != _minValue) {
            valueRatio = (_value - _minValue) / (_maxValue - _minValue);
        }
        valueRatio = std::clamp(valueRatio, 0.0F, 1.0F);
        float filledWidth = _width * valueRatio;

        // Draw filled portion
        if (filledWidth > 0) {
            _graphics.DrawRectangle(static_cast<int>(_x), static_cast<int>(trackY),
                                    static_cast<int>(filledWidth), static_cast<int>(trackHeight),
                                    _filledColor);
        }

        // Calculate handle position
        float handleX = _x + filledWidth;
        float handleY = _y + _height / 2.0F;

        // Draw handle (circle)
        unsigned int handleColor = (_isHovered || _isDragging) ? _handleHoverColor : _handleColor;

        // Draw handle shadow for depth
        _graphics.DrawCircleFilled(static_cast<int>(handleX), static_cast<int>(handleY) + 2,
                                   static_cast<int>(HANDLE_RADIUS), 0x88000000);

        // Draw handle
        _graphics.DrawCircleFilled(static_cast<int>(handleX), static_cast<int>(handleY),
                                   static_cast<int>(HANDLE_RADIUS), handleColor);

        // Draw handle border
        _graphics.DrawCircle(static_cast<int>(handleX), static_cast<int>(handleY),
                             static_cast<int>(HANDLE_RADIUS), 0xFF000000);
    }

    void RaylibSlider::SetOnValueChanged(std::function<void(float)> callback) {
        _onValueChanged = std::move(callback);
    }

    void RaylibSlider::SetPosition(float x, float y) {
        _x = x;
        _y = y;
    }

    void RaylibSlider::GetPosition(float &x, float &y) const {
        x = _x;
        y = _y;
    }

    void RaylibSlider::SetSize(float width, float height) {
        _width = width;
        _height = height;
    }

    void RaylibSlider::GetSize(float &width, float &height) const {
        width = _width;
        height = _height;
    }

    void RaylibSlider::SetTrackColor(unsigned int color) {
        _trackColor = color;
    }

    void RaylibSlider::SetFilledColor(unsigned int color) {
        _filledColor = color;
    }

    void RaylibSlider::SetHandleColor(unsigned int color) {
        _handleColor = color;
    }

    void RaylibSlider::SetHandleHoverColor(unsigned int color) {
        _handleHoverColor = color;
    }

    void RaylibSlider::SetMinValue(float minValue) {
        _minValue = minValue;
        ClampValue();
    }

    void RaylibSlider::SetMaxValue(float maxValue) {
        _maxValue = maxValue;
        ClampValue();
    }

    void RaylibSlider::SetValue(float value) {
        float oldValue = _value;
        _value = value;
        ClampValue();

        // Trigger callback if value changed
        if (_value != oldValue && _onValueChanged) {
            _onValueChanged(_value);
        }
    }

    float RaylibSlider::GetValue() const {
        return _value;
    }

    void RaylibSlider::SetAlign(Align align) {
        _align = align;
    }

    Align RaylibSlider::GetAlign() const {
        return _align;
    }

    void RaylibSlider::ApplyAlignment() {
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

    void RaylibSlider::SetEnabled(bool enabled) {
        _enabled = enabled;
        if (!_enabled) {
            _isDragging = false;
            _isHovered = false;
        }
    }

    bool RaylibSlider::IsEnabled() const {
        return _enabled;
    }

    bool RaylibSlider::IsMouseOverHandle() const {
        int mouseX = _graphics.GetMouseX();
        int mouseY = _graphics.GetMouseY();

        // Calculate handle position (with division by zero protection)
        float valueRatio = 0.0F;
        if (_maxValue != _minValue) {
            valueRatio = (_value - _minValue) / (_maxValue - _minValue);
        }
        valueRatio = std::clamp(valueRatio, 0.0F, 1.0F);
        float handleX = _x + _width * valueRatio;
        float handleY = _y + _height / 2.0F;

        // Check if mouse is within handle radius
        float dx = static_cast<float>(mouseX) - handleX;
        float dy = static_cast<float>(mouseY) - handleY;
        float distanceSquared = dx * dx + dy * dy;

        return distanceSquared <= (HANDLE_RADIUS * HANDLE_RADIUS);
    }

    bool RaylibSlider::IsMouseOverTrack() const {
        int mouseX = _graphics.GetMouseX();
        int mouseY = _graphics.GetMouseY();

        return static_cast<float>(mouseX) >= _x && static_cast<float>(mouseX) <= _x + _width &&
               static_cast<float>(mouseY) >= _y && static_cast<float>(mouseY) <= _y + _height;
    }

    void RaylibSlider::UpdateValueFromMouse() {
        int mouseX = _graphics.GetMouseX();

        // Calculate value from mouse X position
        float relativeX = static_cast<float>(mouseX) - _x;
        float valueRatio = relativeX / _width;

        // Clamp ratio to [0, 1]
        valueRatio = std::max(0.0F, std::min(1.0F, valueRatio));

        // Calculate new value (with protection against min == max)
        float oldValue = _value;
        if (_maxValue != _minValue) {
            _value = _minValue + valueRatio * (_maxValue - _minValue);
        } else {
            _value = _minValue;  // If min == max, value is always min
        }

        // Trigger callback if value changed
        if (_value != oldValue && _onValueChanged) {
            _onValueChanged(_value);
        }
    }

    void RaylibSlider::ClampValue() {
        _value = std::max(_minValue, std::min(_maxValue, _value));
    }
}  // namespace UI
