/*
** EPITECH PROJECT, 2025
** Created by Copilot on 10/01/2026
** File description:
** LoginScreen.cpp
*/

#include "LoginScreen.hpp"
#include <algorithm>

LoginScreen::LoginScreen()
    : _loginSubmitted(false),
      _registerSubmitted(false),
      _guestSubmitted(false),
      _usernameFocused(true),
      _passwordFocused(false) {}

void LoginScreen::reset() {
    _username.clear();
    _password.clear();
    _errorMessage.clear();
    _successMessage.clear();
    _loginSubmitted = false;
    _registerSubmitted = false;
    _guestSubmitted = false;
    _usernameFocused = true;
    _passwordFocused = false;
}

void LoginScreen::update() {
    _handleTextInput();

    // Calculate button bounds - 3 buttons: LOGIN, REGISTER, GUEST
    Rectangle loginButton = {SCREEN_WIDTH / 2.0f - INPUT_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f + 80.0f,
                             INPUT_WIDTH / 3.0f - 7.0f, INPUT_HEIGHT};

    Rectangle registerButton = {SCREEN_WIDTH / 2.0f - INPUT_WIDTH / 6.0f + 2.0f, SCREEN_HEIGHT / 2.0f + 80.0f,
                                INPUT_WIDTH / 3.0f - 7.0f, INPUT_HEIGHT};

    Rectangle guestButton = {SCREEN_WIDTH / 2.0f + INPUT_WIDTH / 6.0f + 4.0f, SCREEN_HEIGHT / 2.0f + 80.0f,
                             INPUT_WIDTH / 3.0f - 7.0f, INPUT_HEIGHT};

    // Check for mouse clicks on input fields
    Rectangle usernameBox = {SCREEN_WIDTH / 2.0f - INPUT_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f - 60.0f,
                             INPUT_WIDTH, INPUT_HEIGHT};

    Rectangle passwordBox = {SCREEN_WIDTH / 2.0f - INPUT_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f + 10.0f,
                             INPUT_WIDTH, INPUT_HEIGHT};

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();

        if (CheckCollisionPointRec(mousePos, usernameBox)) {
            _usernameFocused = true;
            _passwordFocused = false;
        } else if (CheckCollisionPointRec(mousePos, passwordBox)) {
            _usernameFocused = false;
            _passwordFocused = true;
        } else if (CheckCollisionPointRec(mousePos, loginButton)) {
            if (!_username.empty() && !_password.empty()) {
                _loginSubmitted = true;
            } else {
                _errorMessage = "Please enter username and password";
            }
        } else if (CheckCollisionPointRec(mousePos, registerButton)) {
            if (!_username.empty() && !_password.empty()) {
                _registerSubmitted = true;
            } else {
                _errorMessage = "Please enter username and password";
            }
        } else if (CheckCollisionPointRec(mousePos, guestButton)) {
            // Guest doesn't need username/password
            _guestSubmitted = true;
        } else {
            _usernameFocused = false;
            _passwordFocused = false;
        }
    }

    // Tab to switch fields
    if (IsKeyPressed(KEY_TAB)) {
        if (_usernameFocused) {
            _usernameFocused = false;
            _passwordFocused = true;
        } else if (_passwordFocused) {
            _passwordFocused = false;
            _usernameFocused = true;
        }
    }

    // Enter to submit
    if (IsKeyPressed(KEY_ENTER)) {
        if (!_username.empty() && !_password.empty()) {
            _loginSubmitted = true;
        }
    }
}

void LoginScreen::_handleTextInput() {
    int key = GetCharPressed();

    while (key > 0) {
        // Only allow printable characters (32-126)
        if (key >= 32 && key <= 126) {
            if (_usernameFocused && _username.length() < 20) {
                _username += static_cast<char>(key);
            } else if (_passwordFocused && _password.length() < 20) {
                _password += static_cast<char>(key);
            }
        }
        key = GetCharPressed();
    }

    // Handle backspace
    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (_usernameFocused && !_username.empty()) {
            _username.pop_back();
        } else if (_passwordFocused && !_password.empty()) {
            _password.pop_back();
        }
    }
}

void LoginScreen::render() {
    ClearBackground(RAYWHITE);

    // Title
    const char *title = "R-TYPE LOGIN";
    int titleWidth = MeasureText(title, 40);
    DrawText(title, SCREEN_WIDTH / 2 - titleWidth / 2, SCREEN_HEIGHT / 2 - 150, 40, DARKBLUE);

    // Username input box
    Rectangle usernameBox = {SCREEN_WIDTH / 2.0f - INPUT_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f - 60.0f,
                             INPUT_WIDTH, INPUT_HEIGHT};
    _renderInputBox("Username:", _username, _usernameFocused, false, usernameBox);

    // Password input box
    Rectangle passwordBox = {SCREEN_WIDTH / 2.0f - INPUT_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f + 10.0f,
                             INPUT_WIDTH, INPUT_HEIGHT};
    _renderInputBox("Password:", _password, _passwordFocused, true, passwordBox);

    // Login button (left)
    Rectangle loginButton = {SCREEN_WIDTH / 2.0f - INPUT_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f + 80.0f,
                             INPUT_WIDTH / 3.0f - 7.0f, INPUT_HEIGHT};

    Color loginColor = DARKGREEN;
    if (CheckCollisionPointRec(GetMousePosition(), loginButton)) {
        loginColor = GREEN;
    }

    DrawRectangleRec(loginButton, loginColor);
    DrawRectangleLinesEx(loginButton, 2, BLACK);

    const char *loginText = "LOGIN";
    int loginTextWidth = MeasureText(loginText, 18);
    DrawText(loginText, loginButton.x + loginButton.width / 2 - loginTextWidth / 2,
             loginButton.y + loginButton.height / 2 - 9, 18, WHITE);

    // Register button (middle)
    Rectangle registerButton = {SCREEN_WIDTH / 2.0f - INPUT_WIDTH / 6.0f + 2.0f, SCREEN_HEIGHT / 2.0f + 80.0f,
                                INPUT_WIDTH / 3.0f - 7.0f, INPUT_HEIGHT};

    Color registerColor = DARKBLUE;
    if (CheckCollisionPointRec(GetMousePosition(), registerButton)) {
        registerColor = BLUE;
    }

    DrawRectangleRec(registerButton, registerColor);
    DrawRectangleLinesEx(registerButton, 2, BLACK);

    const char *registerText = "REGISTER";
    int registerTextWidth = MeasureText(registerText, 16);
    DrawText(registerText, registerButton.x + registerButton.width / 2 - registerTextWidth / 2,
             registerButton.y + registerButton.height / 2 - 8, 16, WHITE);

    // Guest button (right)
    Rectangle guestButton = {SCREEN_WIDTH / 2.0f + INPUT_WIDTH / 6.0f + 4.0f, SCREEN_HEIGHT / 2.0f + 80.0f,
                             INPUT_WIDTH / 3.0f - 7.0f, INPUT_HEIGHT};

    Color guestColor = Color{100, 100, 100, 255};  // DARKGRAY
    if (CheckCollisionPointRec(GetMousePosition(), guestButton)) {
        guestColor = GRAY;
    }

    DrawRectangleRec(guestButton, guestColor);
    DrawRectangleLinesEx(guestButton, 2, BLACK);

    const char *guestText = "GUEST";
    int guestTextWidth = MeasureText(guestText, 18);
    DrawText(guestText, guestButton.x + guestButton.width / 2 - guestTextWidth / 2,
             guestButton.y + guestButton.height / 2 - 9, 18, WHITE);

    // Success message
    if (!_successMessage.empty()) {
        int successWidth = MeasureText(_successMessage.c_str(), 20);
        DrawText(_successMessage.c_str(), SCREEN_WIDTH / 2 - successWidth / 2, SCREEN_HEIGHT / 2 + 140, 20,
                 GREEN);
    }

    // Error message
    if (!_errorMessage.empty()) {
        int errorWidth = MeasureText(_errorMessage.c_str(), 20);
        DrawText(_errorMessage.c_str(), SCREEN_WIDTH / 2 - errorWidth / 2, SCREEN_HEIGHT / 2 + 140, 20, RED);
    }

    // Instructions
    const char *instructions = "Press TAB to switch fields | Press ENTER to login";
    int instWidth = MeasureText(instructions, 15);
    DrawText(instructions, SCREEN_WIDTH / 2 - instWidth / 2, SCREEN_HEIGHT - 40, 15, GRAY);

    // Demo credentials hint
    const char *hint = "Demo: username=demo password=1234 | Or create new account!";
    int hintWidth = MeasureText(hint, 15);
    DrawText(hint, SCREEN_WIDTH / 2 - hintWidth / 2, SCREEN_HEIGHT / 2 - 190, 15, DARKGRAY);
}

void LoginScreen::_renderInputBox(const char *label, const std::string &text, bool isFocused, bool isPassword,
                                  Rectangle bounds) {
    // Label above input
    DrawText(label, bounds.x, bounds.y - 25, 20, DARKGRAY);

    // Input box
    Color boxColor = isFocused ? LIGHTGRAY : WHITE;
    Color borderColor = isFocused ? BLUE : GRAY;

    DrawRectangleRec(bounds, boxColor);
    DrawRectangleLinesEx(bounds, isFocused ? 3 : 2, borderColor);

    // Text content (masked if password)
    std::string displayText = text;
    if (isPassword && !text.empty()) {
        displayText = std::string(text.length(), '*');
    }

    // Add cursor if focused
    if (isFocused) {
        displayText += "_";
    }

    DrawText(displayText.c_str(), bounds.x + 10, bounds.y + 10, 20, BLACK);
}
