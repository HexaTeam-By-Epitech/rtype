/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** KeyBindingsMenu - Menu for remapping key bindings
*/

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Graphics/IGraphics.hpp"
#include "Input/KeyBindings.hpp"
#include "Menu/BaseMenu.hpp"
#include "UI/IUIFactory.hpp"

namespace Game {

    /**
     * @brief Menu for viewing and remapping key bindings.
     *
     * Displays all game actions with their current key bindings.
     * Allows users to remap keys by clicking on a binding and pressing a new key.
     */
    class KeyBindingsMenu : public BaseMenu {
       public:
        /**
         * @brief Display mode for the menu
         */
        enum class Mode {
            FULLSCREEN,  ///< Takes the whole window
            OVERLAY      ///< Displays over the game with dimmed background
        };

        /**
         * @brief Construct a new KeyBindingsMenu
         * @param uiFactory UI factory for creating elements
         * @param graphics Graphics interface for rendering
         */
        explicit KeyBindingsMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics);
        ~KeyBindingsMenu() override = default;

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

        // --- Callbacks ---

        /**
         * @brief Set callback for when back button is pressed
         */
        void SetOnBack(std::function<void()> callback);

        /**
         * @brief Set callback for when bindings are changed
         */
        void SetOnBindingsChanged(std::function<void()> callback);

       private:
        /**
         * @brief Create a binding row for an action
         * @param action The game action
         * @param yOffset Vertical offset for positioning
         */
        void CreateBindingRow(Input::GameAction action, float yOffset);

        /**
         * @brief Update the display text for a binding button
         * @param action The game action whose button to update
         * @param isPrimary Whether to update primary or secondary button
         */
        void UpdateBindingButtonText(Input::GameAction action, bool isPrimary);

        /**
         * @brief Start listening for a new key press to remap
         * @param action The action to remap
         * @param isPrimary Whether remapping primary or secondary key
         */
        void StartKeyCapture(Input::GameAction action, bool isPrimary);

        /**
         * @brief Cancel key capture mode
         */
        void CancelKeyCapture();

        /**
         * @brief Handle a key press during capture mode
         * @param key The key that was pressed
         */
        void HandleCapturedKey(int key);

        /**
         * @brief Refresh all binding button texts
         */
        void RefreshAllBindings();

        Graphics::IGraphics &_graphics;

        // Display mode
        Mode _mode{Mode::FULLSCREEN};
        unsigned int _overlayDimColor{0x80000000};  // Semi-transparent black

        // Callbacks
        std::function<void()> _onBack;
        std::function<void()> _onBindingsChanged;

        // Key capture state
        bool _isCapturing{false};
        Input::GameAction _captureAction{Input::GameAction::MOVE_UP};
        bool _capturePrimary{true};

        // Binding buttons (for updating text)
        struct BindingButtons {
            std::shared_ptr<UI::IButton> primaryButton;
            std::shared_ptr<UI::IButton> secondaryButton;
        };
        std::unordered_map<Input::GameAction, BindingButtons> _bindingButtons;

        // UI Elements
        std::shared_ptr<UI::IButton> _backButton;
        std::shared_ptr<UI::IButton> _resetButton;
    };

}  // namespace Game
