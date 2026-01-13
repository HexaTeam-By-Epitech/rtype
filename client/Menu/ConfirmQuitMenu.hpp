/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** ConfirmQuitMenu - Quit confirmation dialog (business logic)
*/

#pragma once

#include <functional>
#include <memory>

#include "Menu/BaseMenu.hpp"
#include "UI/IUIFactory.hpp"

namespace Game {
    /**
     * @brief Simple quit confirmation dialog.
     *
     * Business-level class: it uses the UI library, but does not depend on Graphics.
     * It is meant to be displayed either fullscreen (main menu context) or as an
     * overlay (in-game context) depending on how the caller renders it.
     */
    class ConfirmQuitMenu : public BaseMenu {
       public:
        /**
         * @brief Construct a new ConfirmQuitMenu.
         * @param uiFactory UI factory used to create menus and buttons.
         */
        explicit ConfirmQuitMenu(UI::IUIFactory &uiFactory);
        ~ConfirmQuitMenu() override = default;

        /**
         * @brief Initialize UI elements (creates buttons).
         */
        void Initialize() override;

        /**
         * @brief Set callback invoked when user confirms quit.
         */
        void SetOnConfirm(std::function<void()> callback);

        /**
         * @brief Set callback invoked when user cancels.
         */
        void SetOnCancel(std::function<void()> callback);

       private:
        void OnConfirmClicked();
        void OnCancelClicked();

        std::function<void()> _onConfirm{};
        std::function<void()> _onCancel{};
    };
}  // namespace Game
