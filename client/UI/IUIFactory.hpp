/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** IUIFactory - Abstract factory for creating UI elements
*/

#pragma once

#include <memory>

#include "UI/IButton.hpp"
#include "UI/IMenu.hpp"
#include "UI/ISlider.hpp"
#include "UI/ITextInput.hpp"

namespace UI {
    /**
     * @brief Abstract factory interface for creating UI elements.
     *
     * This interface is the main entry point to instantiate UI primitives without
     * depending on a backend implementation (DIP).
     *
     * Ownership:
     * - CreateButton() returns a unique_ptr: the caller owns the button instance.
     * - CreateMenu() returns a unique_ptr: the caller owns the menu instance.
     */
    class IUIFactory {
       public:
        /** @brief Virtual destructor. */
        virtual ~IUIFactory() = default;

        /**
         * @brief Create a button instance.
         * @return Unique pointer to a new button.
         */
        virtual std::unique_ptr<IButton> CreateButton() = 0;

        /**
         * @brief Create a menu instance.
         * @return Unique pointer to a new menu.
         */
        virtual std::unique_ptr<IMenu> CreateMenu() = 0;

        /**
         * @brief Create a text input instance.
         * @return Unique pointer to a new text input.
         */
        virtual std::unique_ptr<ITextInput> CreateTextInput() = 0;

        /**
         * @brief Create a slider instance.
         * @return Unique pointer to a new slider.
         */
        virtual std::unique_ptr<ISlider> CreateSlider() = 0;
    };
}  // namespace UI
