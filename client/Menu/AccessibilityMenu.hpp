/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** AccessibilityMenu - Accessibility features menu
*/

#pragma once

#include <functional>
#include <memory>
#include <string>

#include "Graphics/IGraphics.hpp"
#include "Menu/BaseMenu.hpp"
#include "UI/IUIFactory.hpp"

namespace Game {
    /**
     * @brief Accessibility menu for configuring accessibility features.
     *
     * Provides options for:
     * - Key binding customization
     * - Colorblind filters
     * - Visual sound indicators
     * - Game speed adjustment
     */
    class AccessibilityMenu : public BaseMenu {
       public:
        /**
         * @brief Colorblind filter types
         */
        enum class ColorblindFilter {
            NONE,          ///< No filter applied
            PROTANOPIA,    ///< Red-blind
            DEUTERANOPIA,  ///< Green-blind
            TRITANOPIA,    ///< Blue-blind
            MONOCHROMACY   ///< Complete color blindness
        };

        /**
         * @brief Display mode for the accessibility menu
         */
        enum class Mode {
            FULLSCREEN,  ///< Takes the whole window (main menu context)
            OVERLAY      ///< Displays over the game with a dimmed background
        };

        /**
         * @brief Construct a new AccessibilityMenu
         * @param uiFactory UI factory used to create menus and buttons
         * @param graphics Graphics interface for rendering
         */
        explicit AccessibilityMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics);
        ~AccessibilityMenu() override = default;

        /**
         * @brief Initialize UI elements
         */
        void Initialize() override;

        void Update() override;
        void Render() override;

        // --- Display Mode ---
        void SetMode(Mode mode);
        [[nodiscard]] Mode GetMode() const;

        void SetOverlayDimColor(unsigned int color);
        [[nodiscard]] unsigned int GetOverlayDimColor() const;
        [[nodiscard]] bool ShouldDimBackground() const;

        // --- Colorblind Filters ---
        /**
         * @brief Set the colorblind filter type
         * @param filter The filter to apply
         */
        void SetColorblindFilter(ColorblindFilter filter);

        /**
         * @brief Get the current colorblind filter
         * @return Current filter type
         */
        [[nodiscard]] ColorblindFilter GetColorblindFilter() const;

        /**
         * @brief Set callback invoked when colorblind filter changes
         */
        void SetOnColorblindFilterChanged(std::function<void(ColorblindFilter)> callback);

        /**
         * @brief Set colorblind filter without emitting callbacks
         */
        void SetColorblindFilterSilent(ColorblindFilter filter);

        // --- Game Speed ---
        /**
         * @brief Set game speed multiplier (0.5 = 50% speed, 1.0 = normal)
         * @param speed Speed multiplier (clamped between 0.25 and 1.0)
         */
        void SetGameSpeed(float speed);

        /**
         * @brief Get current game speed multiplier
         */
        [[nodiscard]] float GetGameSpeed() const;

        /**
         * @brief Set callback invoked when game speed changes
         */
        void SetOnGameSpeedChanged(std::function<void(float)> callback);

        /**
         * @brief Set game speed without emitting callbacks
         */
        void SetGameSpeedSilent(float speed);

        // --- Key Bindings ---
        /**
         * @brief Open key bindings configuration dialog
         */
        void OpenKeyBindingsConfig();

        /**
         * @brief Set callback invoked when user wants to configure key bindings
         */
        void SetOnConfigureKeyBindings(std::function<void()> callback);

        // --- Callbacks ---
        /**
         * @brief Set callback invoked when Back is clicked
         */
        void SetOnBack(std::function<void()> callback);

        /**
         * @brief Set callback invoked when "Main Menu" is clicked (overlay mode)
         */
        void SetOnMainMenu(std::function<void()> callback);

        /**
         * @brief Refresh button labels/colors to match current state
         */
        void RefreshVisuals();

       private:
        // Helper methods
        void UpdateColorblindFilterVisuals();
        [[nodiscard]] ColorblindFilter NextColorblindFilter(ColorblindFilter current) const;
        [[nodiscard]] const char *ColorblindFilterToString(ColorblindFilter filter) const;
        [[nodiscard]] float ClampGameSpeed(float speed) const;

        // Member variables
        Graphics::IGraphics &_graphics;

        Mode _mode{Mode::FULLSCREEN};
        ColorblindFilter _colorblindFilter{ColorblindFilter::NONE};
        float _gameSpeed{1.0f};  // Normal speed
        unsigned int _overlayDimColor{0x88000000};

        // Callbacks
        std::function<void(ColorblindFilter)> _onColorblindFilterChanged{};
        std::function<void(float)> _onGameSpeedChanged{};
        std::function<void()> _onConfigureKeyBindings{};
        std::function<void()> _onBack{};
        std::function<void()> _onMainMenu{};

        // Button indices
        static constexpr size_t COLORBLIND_FILTER_INDEX = 0;
        static constexpr size_t KEY_BINDINGS_INDEX = 1;
        static constexpr size_t BACK_INDEX = 2;
        static constexpr size_t MAIN_MENU_INDEX = 3;
    };
}  // namespace Game
