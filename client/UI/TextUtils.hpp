/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** TextUtils - Common text calculation utilities
*/

#pragma once

#include <string>

namespace UI {
    namespace TextUtils {
        /**
         * @brief Estimate the width of text in pixels.
         * 
         * This is a rough approximation based on the assumption that
         * each character is approximately 60% of the font size in width.
         * 
         * @param text Text string to measure
         * @param fontSize Font size in pixels
         * @return Estimated width in pixels
         * 
         * @note This is an approximation and may not be pixel-perfect.
         *       For accurate measurements, use Raylib's MeasureText function.
         */
        inline int EstimateTextWidth(const std::string &text, int fontSize) {
            return static_cast<int>(text.size() * static_cast<size_t>(fontSize) * 6 / 10);
        }
    }  // namespace TextUtils
}  // namespace UI
