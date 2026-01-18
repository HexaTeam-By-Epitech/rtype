/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RaylibMenu - Raylib implementation of IMenu
*/

#pragma once

#include <memory>
#include <vector>
#include "Graphics/IGraphics.hpp"
#include "UI/IButton.hpp"
#include "UI/IMenu.hpp"

namespace UI {
    /**
     * @brief Raylib implementation of the IMenu interface.
     *
     * Owns a list of buttons and calls Update()/Render() on them.
     * Uses Graphics::IGraphics only as a dependency carrier for potential
     * future menu-level drawing (background/panels) and to keep a consistent
     * creation signature with the UI factory.
     */
    class RaylibMenu : public IMenu {
       public:
        /**
         * @brief Construct a new RaylibMenu.
         * @param graphics Graphics wrapper dependency.
         */
        explicit RaylibMenu(Graphics::IGraphics &graphics);

        /**
         * @brief Destroy the RaylibMenu.
         */
        ~RaylibMenu() override = default;

        /** @copydoc UI::IMenu::Update */
        void Update() override;

        /** @copydoc UI::IMenu::Render */
        void Render() override;

        /** @copydoc UI::IMenu::AddButton */
        void AddButton(std::shared_ptr<IButton> button) override;

        /** @copydoc UI::IMenu::RemoveButton */
        void RemoveButton(size_t index) override;

        /** @copydoc UI::IMenu::GetButton */
        std::shared_ptr<IButton> GetButton(size_t index) override;

        /** @copydoc UI::IMenu::GetButtonCount */
        [[nodiscard]] size_t GetButtonCount() const override;

        /** @copydoc UI::IMenu::Clear */
        void Clear() override;

        /** @copydoc UI::IMenu::SetVisible */
        void SetVisible(bool visible) override;

        /** @copydoc UI::IMenu::IsVisible */
        [[nodiscard]] bool IsVisible() const override;

        /** @copydoc UI::IMenu::HandleKeyboardNavigation */
        void HandleKeyboardNavigation() override;

        /** @copydoc UI::IMenu::SelectNext */
        void SelectNext() override;

        /** @copydoc UI::IMenu::SelectPrevious */
        void SelectPrevious() override;

        /** @copydoc UI::IMenu::TriggerSelected */
        void TriggerSelected() override;

        /** @copydoc UI::IMenu::GetSelectedIndex */
        [[nodiscard]] int GetSelectedIndex() const override;

        /** @copydoc UI::IMenu::SetSelectedIndex */
        void SetSelectedIndex(int index) override;

       private:
        Graphics::IGraphics &_graphics;                  ///< Graphics dependency
        std::vector<std::shared_ptr<IButton>> _buttons;  ///< Collection of buttons
        bool _visible{true};                             ///< Menu visibility state
        int _selectedIndex{-1};                          ///< Currently selected button index (-1 = none)
    };
}  // namespace UI
