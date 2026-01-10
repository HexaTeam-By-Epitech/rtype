/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RaylibUIFactory - Factory for creating Raylib UI elements
*/

#pragma once

#include "Graphics/IGraphics.hpp"
#include "UI/IUIFactory.hpp"

namespace UI {
    /**
     * @brief Raylib implementation of the IUIFactory interface.
     *
     * Creates Raylib-backed UI primitives (buttons, menus), while returning
     * only abstract interfaces (IButton/IMenu) to keep callers decoupled
     * from the backend.
     *
     * The factory holds a reference to a Graphics::IGraphics implementation.
     * UI elements created by this factory will use that same graphics instance.
     */
    class RaylibUIFactory : public IUIFactory {
       public:
        /**
         * @brief Construct a new RaylibUIFactory.
         * @param graphics Graphics wrapper used by UI elements.
         */
        explicit RaylibUIFactory(Graphics::IGraphics &graphics);

        /** @brief Destructor. */
        ~RaylibUIFactory() override = default;

        /** @copydoc UI::IUIFactory::CreateButton */
        std::unique_ptr<IButton> CreateButton() override;

        /** @copydoc UI::IUIFactory::CreateMenu */
        std::unique_ptr<IMenu> CreateMenu() override;

       private:
        Graphics::IGraphics &_graphics;  ///< Graphics dependency injected into UI elements
    };
}  // namespace UI
