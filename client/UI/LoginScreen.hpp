/*
** EPITECH PROJECT, 2025
** Created by Copilot on 10/01/2026
** File description:
** LoginScreen.hpp - Login UI for authentication
*/

#pragma once

#include <functional>
#include <string>
#include "raylib.h"

/**
 * @class LoginScreen
 * @brief Login screen with username/password input
 * 
 * Simple authentication UI using Raylib:
 * - Username text input
 * - Password text input (masked)
 * - Login button
 * - Error message display
 */
class LoginScreen {
   public:
    LoginScreen();
    ~LoginScreen() = default;

    /**
     * @brief Update login screen (handle input)
     */
    void update();

    /**
     * @brief Render login screen
     */
    void render();

    /**
     * @brief Check if login was submitted
     * @return true if user clicked login button
     */
    bool isLoginSubmitted() const { return _loginSubmitted; }

    /**
     * @brief Get entered username
     */
    std::string getUsername() const { return _username; }

    /**
     * @brief Get entered password
     */
    std::string getPassword() const { return _password; }

    /**
     * @brief Set error message to display
     */
    void setErrorMessage(const std::string &error) { _errorMessage = error; }

    /**
     * @brief Set success message to display
     */
    void setSuccessMessage(const std::string &success) { _successMessage = success; }

    /**
     * @brief Check if register was submitted
     */
    bool isRegisterSubmitted() const { return _registerSubmitted; }

    /**
     * @brief Check if guest login was submitted
     */
    bool isGuestSubmitted() const { return _guestSubmitted; }

    /**
     * @brief Reset login state
     */
    void reset();

   private:
    void _handleTextInput();
    void _renderInputBox(const char *label, const std::string &text, bool isFocused, bool isPassword,
                         Rectangle bounds);

    std::string _username;
    std::string _password;
    std::string _errorMessage;
    std::string _successMessage;

    bool _loginSubmitted;
    bool _registerSubmitted;
    bool _guestSubmitted;
    bool _usernameFocused;
    bool _passwordFocused;

    // UI Layout
    static constexpr int SCREEN_WIDTH = 800;
    static constexpr int SCREEN_HEIGHT = 600;
    static constexpr int INPUT_WIDTH = 300;
    static constexpr int INPUT_HEIGHT = 40;
};
