/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** BaseMenu - Base class for all menus to reduce code duplication
*/

#pragma once

#include <functional>
#include <memory>
#include "Audio/ISoundEffectService.hpp"
#include "UI/IButton.hpp"
#include "UI/IMenu.hpp"
#include "UI/Raylib/RaylibUIFactory.hpp"

namespace Game {
    /**
     * @brief Base class for all menu implementations.
     * 
     * Provides common functionality for menu management including:
     * - Menu lifecycle (Show/Hide/IsVisible)
     * - Button creation with consistent styling
     * - Update and Render delegation to UI components
     */
    class BaseMenu {
       public:
        /**
         * @brief Constructor with UI factory reference.
         * @param uiFactory Factory for creating UI elements.
         */
        explicit BaseMenu(UI::IUIFactory &uiFactory);

        /**
         * @brief Virtual destructor.
         */
        virtual ~BaseMenu() = default;

        /**
         * @brief Update menu state (should be called every frame).
         */
        virtual void Update();

        /**
         * @brief Render menu (should be called every frame).
         */
        virtual void Render();

        /**
         * @brief Show the menu.
         */
        virtual void Show();

        /**
         * @brief Hide the menu.
         */
        virtual void Hide();

        /**
         * @brief Check if menu is currently visible.
         * @return true if visible, false otherwise.
         */
        [[nodiscard]] virtual bool IsVisible() const;

        /**
         * @brief Initialize menu (must be implemented by derived classes).
         */
        virtual void Initialize() = 0;

        /**
         * @brief Set the sound effect service for playing UI sounds.
         * @param soundService Pointer to sound effect service (can be nullptr to disable sounds).
         */
        void SetSoundEffectService(Audio::ISoundEffectService *soundService);

       protected:
        /**
         * @brief Wrap a callback to play click sound before executing.
         * @param callback Original callback function.
         * @return Wrapped callback that plays click sound.
         */
        std::function<void()> WrapWithClickSound(std::function<void()> callback);
        /**
         * @brief Create a button with standard styling and positioning.
         * 
         * @param label Button text label.
         * @param offsetY Vertical offset from center.
         * @param width Button width.
         * @param height Button height.
         * @param backgroundColor Button background color (RGBA).
         * @param hoverColor Button hover color (RGBA).
         * @param callback Function to call when button is clicked.
         * @return Shared pointer to created button.
         */
        std::shared_ptr<UI::IButton> CreateCenteredButton(const char *label, float offsetY, float width,
                                                          float height, unsigned int backgroundColor,
                                                          unsigned int hoverColor,
                                                          std::function<void()> callback);

        UI::IUIFactory &_uiFactory;
        std::shared_ptr<UI::IMenu> _menu;
        Audio::ISoundEffectService *_soundService{nullptr};
    };
}  // namespace Game
