/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** LoginMenu - Menu for user authentication (Login/Register/Guest)
*/

#pragma once

#include <string>
#include "BaseMenu.hpp"
#include "UI/ITextInput.hpp"

namespace Game {

    class LoginMenu : public BaseMenu {
       public:
        /**
         * @brief Constructor.
         * @param uiFactory UI Factory reference.
         * @param graphics Graphics interface reference (for screen size).
         */
        LoginMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics);

        ~LoginMenu() override = default;

        void Initialize() override;
        void Update() override;
        void Render() override;

        // --- Status Getters ---
        [[nodiscard]] bool IsLoginSubmitted() const;
        [[nodiscard]] bool IsRegisterSubmitted() const;
        [[nodiscard]] bool IsGuestSubmitted() const;

        [[nodiscard]] std::string GetUsername() const;
        [[nodiscard]] std::string GetPassword() const;

        // --- Feedback methods ---
        void SetErrorMessage(const std::string &message);
        void SetSuccessMessage(const std::string &message);
        void ResetMessages();

        // --- Callbacks ---
        void SetOnBack(std::function<void()> onBack);

        // --- Reset State ---
        void Reset();

       private:
        Graphics::IGraphics &_graphics;

        // UI Components
        std::shared_ptr<UI::ITextInput> _usernameInput;
        std::shared_ptr<UI::ITextInput> _passwordInput;

        // State flags (consumed by main loop)
        bool _loginSubmitted = false;
        bool _registerSubmitted = false;
        bool _guestSubmitted = false;

        // Callbacks
        std::function<void()> _onBack;

        // Feedback messages
        std::string _errorMessage;
        std::string _successMessage;

        // Layout constants
        const float INPUT_WIDTH = 300.0f;
        const float INPUT_HEIGHT = 40.0f;
        const float SPACING = 15.0f;

        // Internal helpers
        void OnLoginClicked();
        void OnRegisterClicked();
        void OnGuestClicked();
        void HandleTabNavigation();
    };

}  // namespace Game
