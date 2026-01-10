/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ConnectionMenu - Menu for entering connection details (nickname, IP, port)
*/

#pragma once

#include <functional>
#include <memory>

#include "Graphics/IGraphics.hpp"
#include "UI/IButton.hpp"
#include "UI/IMenu.hpp"
#include "UI/ITextInput.hpp"
#include "UI/IUIFactory.hpp"

namespace Game {
    /**
     * @brief Connection menu for entering player nickname, server IP, and port.
     *
     * This menu is displayed when the user clicks "Play" and allows them to:
     * - Enter their nickname
     * - Enter server IP address
     * - Enter server port
     * - Join the game (triggers callback with entered values)
     * - Go back to main menu
     */
    class ConnectionMenu {
       public:
        explicit ConnectionMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics);
        ~ConnectionMenu() = default;

        /**
         * @brief Set callback triggered when the Join button is clicked.
         * @param onJoin Callback receiving (nickname, ip, port) as parameters.
         */
        void SetOnJoin(
            std::function<void(const std::string &, const std::string &, const std::string &)> onJoin);

        /**
         * @brief Set callback triggered when the Back button is clicked.
         */
        void SetOnBack(std::function<void()> onBack);

        void Initialize();
        void Update();
        void Render();

        void Show();
        void Hide();
        bool IsVisible() const;

       private:
        void OnJoinClicked();
        void OnBackClicked();

        // Validation helpers
        bool ValidateNickname(const std::string &nickname, std::string &errorMsg);
        bool ValidateIP(const std::string &ip, std::string &errorMsg);
        bool ValidatePort(const std::string &port, std::string &errorMsg);
        void ClearError();

        std::function<void(const std::string &, const std::string &, const std::string &)> _onJoin{};
        std::function<void()> _onBack{};

        UI::IUIFactory &_uiFactory;
        Graphics::IGraphics &_graphics;
        std::unique_ptr<UI::IMenu> _menu;

        // Text inputs
        std::unique_ptr<UI::ITextInput> _nicknameInput;
        std::unique_ptr<UI::ITextInput> _ipInput;
        std::unique_ptr<UI::ITextInput> _portInput;

        // Error display
        std::string _errorMessage;
        bool _hasError{false};
        enum class ErrorField { NONE, NICKNAME, IP, PORT };
        ErrorField _errorField{ErrorField::NONE};

        // Button positioning for error message
        float _buttonsY{0.0f};
        float _buttonHeight{0.0f};
    };
}  // namespace Game
