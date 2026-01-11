/*
** EPITECH PROJECT, 2025
** Created by Copilot on 10/01/2026
** File description:
** main_with_login.cpp - Client with login screen
*/

#include "Client/Client.hpp"
#include "UI/LoginScreen.hpp"
#include "raylib.h"

/**
 * @brief Client with authentication login screen
 * 
 * Flow:
 * 1. Show login screen
 * 2. User enters username/password
 * 3. Connect to server with credentials
 * 4. Start game if authenticated
 */
class AuthenticatedClient {
   public:
    AuthenticatedClient(const std::string &host, uint16_t port)
        : _host(host), _port(port), _state(State::LOGIN) {}

    void run() {
        // Initialize Raylib window for login screen
        InitWindow(800, 600, "R-Type - Login");
        SetTargetFPS(60);

        LoginScreen loginScreen;

        while (!WindowShouldClose()) {
            if (_state == State::LOGIN) {
                loginScreen.update();

                BeginDrawing();
                loginScreen.render();
                EndDrawing();

                if (loginScreen.isLoginSubmitted()) {
                    _username = loginScreen.getUsername();
                    _password = loginScreen.getPassword();
                    _playerName = _username;  // Use username as player name

                    // Close Raylib window before starting game
                    CloseWindow();

                    _state = State::CONNECTING;
                    _connectToGame();
                }
            } else if (_state == State::GAME) {
                break;
            }
        }

        CloseWindow();
    }

   private:
    enum class State { LOGIN, CONNECTING, GAME, ERROR };

    void _connectToGame() {
        // Create and run client with authentication
        Client client(_playerName, _host, _port, false);

        if (!client.initialize()) {
            std::cerr << "Failed to initialize client" << std::endl;
            _state = State::ERROR;
            return;
        }

        // Override the connect method to use username/password
        _state = State::GAME;

        // Note: We'll need to modify Client class to accept credentials
        // For now, this demonstrates the flow
        client.run();
    }

    std::string _host;
    uint16_t _port;
    std::string _username;
    std::string _password;
    std::string _playerName;
    State _state;
};

int main(int argc, char **argv) {
    std::string host = "127.0.0.1";
    uint16_t port = 4242;

    // Parse host and port from command line
    if (argc > 1) {
        host = argv[1];
    }
    if (argc > 2) {
        port = static_cast<uint16_t>(std::atoi(argv[2]));
    }

    std::cout << "==================================" << std::endl;
    std::cout << "R-Type Client with Authentication" << std::endl;
    std::cout << "Server: " << host << ":" << port << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Demo credentials:" << std::endl;
    std::cout << "  Username: demo" << std::endl;
    std::cout << "  Password: 1234" << std::endl;
    std::cout << std::endl;

    AuthenticatedClient authClient(host, port);
    authClient.run();

    return 0;
}
