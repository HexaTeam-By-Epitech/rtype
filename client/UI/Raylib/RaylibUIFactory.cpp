/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RaylibUIFactory - Factory for creating Raylib UI elements
*/

#include "UI/Raylib/RaylibUIFactory.hpp"
#include "UI/Raylib/RaylibButton.hpp"
#include "UI/Raylib/RaylibMenu.hpp"
#include "UI/Raylib/RaylibSlider.hpp"
#include "UI/Raylib/RaylibTextInput.hpp"

namespace UI {
    RaylibUIFactory::RaylibUIFactory(Graphics::IGraphics &graphics) : _graphics(graphics) {}

    std::unique_ptr<IButton> RaylibUIFactory::CreateButton() {
        return std::make_unique<RaylibButton>(_graphics);
    }

    std::unique_ptr<IMenu> RaylibUIFactory::CreateMenu() {
        return std::make_unique<RaylibMenu>(_graphics);
    }

    std::unique_ptr<ITextInput> RaylibUIFactory::CreateTextInput() {
        return std::make_unique<RaylibTextInput>(_graphics);
    }

    std::unique_ptr<ISlider> RaylibUIFactory::CreateSlider() {
        return std::make_unique<RaylibSlider>(_graphics);
    }
}  // namespace UI
