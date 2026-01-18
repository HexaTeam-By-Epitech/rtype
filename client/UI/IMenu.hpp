/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** IMenu - Abstract menu interface for UI system
*/

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace UI {
    // Forward declaration
    class IButton;

    /**
     * @brief Abstract interface for UI menus.
     *
     * A menu is a container of UI elements (currently: buttons only).
     * It is responsible for:
     * - updating contained elements (input handling)
     * - rendering contained elements
     * - controlling visibility
     *
     * This interface is part of the UI library layer and is designed to be backend-agnostic.
     */
    class IMenu {
       public:
        /** @brief Virtual destructor. */
        virtual ~IMenu() = default;

        /**
         * @brief Update all menu elements.
         * @note Should be called once per frame.
         */
        virtual void Update() = 0;

        /**
         * @brief Render all menu elements.
         * @note Must be called between the backend BeginDrawing/EndDrawing calls.
         */
        virtual void Render() = 0;

        /**
         * @brief Add a button to the menu.
         * @param button Shared pointer to the button to add.
         */
        virtual void AddButton(std::shared_ptr<IButton> button) = 0;

        /**
         * @brief Remove a button from the menu by index.
         * @param index Index of the button to remove.
         */
        virtual void RemoveButton(size_t index) = 0;

        /**
         * @brief Get a button by index.
         * @param index Index of the button.
         * @return Shared pointer to the button, or nullptr if out of range.
         */
        virtual std::shared_ptr<IButton> GetButton(size_t index) = 0;

        /**
         * @brief Get the number of buttons in the menu.
         * @return Number of buttons.
         */
        [[nodiscard]] virtual size_t GetButtonCount() const = 0;

        /**
         * @brief Remove all buttons from the menu.
         */
        virtual void Clear() = 0;

        /**
         * @brief Set menu visibility.
         * @param visible true to show the menu, false to hide it.
         */
        virtual void SetVisible(bool visible) = 0;

        /**
         * @brief Check menu visibility.
         * @return true if visible.
         */
        [[nodiscard]] virtual bool IsVisible() const = 0;

        // ===== Keyboard Navigation =====

        /**
         * @brief Handle keyboard navigation input.
         * @note Call this in Update() to enable Tab/Shift+Tab/Enter navigation.
         */
        virtual void HandleKeyboardNavigation() = 0;

        /**
         * @brief Select the next navigable element.
         * @note Wraps around to first element when at the end.
         */
        virtual void SelectNext() = 0;

        /**
         * @brief Select the previous navigable element.
         * @note Wraps around to last element when at the beginning.
         */
        virtual void SelectPrevious() = 0;

        /**
         * @brief Trigger the currently selected element (simulate click).
         */
        virtual void TriggerSelected() = 0;

        /**
         * @brief Get the index of the currently selected element.
         * @return Selected index, or -1 if none selected.
         */
        [[nodiscard]] virtual int GetSelectedIndex() const = 0;

        /**
         * @brief Set the selected element by index.
         * @param index Index of element to select, or -1 to clear selection.
         */
        virtual void SetSelectedIndex(int index) = 0;
    };
}  // namespace UI
