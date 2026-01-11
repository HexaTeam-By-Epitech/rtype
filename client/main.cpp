/*
** EPITECH PROJECT, 2025
** Created by mael on 02/12/2025.
** File description:
** main.cpp
*/

#include <atomic>
#include <chrono>
#include <ctime>
#include <future>
#include <thread>
#include "../common/Networking/NetworkFactory.hpp"
#include "../common/Serialization/Capnp/ConnectionMessages.hpp"
#include "../common/Serialization/Capnp/NetworkMessages.hpp"
#include "Client/Client.hpp"
#include "UI/LoginScreen.hpp"
#include "raylib.h"

// Helper function to send registration request
bool sendRegisterRequest(const std::string &username, const std::string &password, const std::string &host,
                         uint16_t port) {
    using namespace RType::Messages;
    using namespace ConnectionMessages;

    // Create temporary network connection
    auto clientHost = createClientHost(2);
    auto address = createAddress(host, port);

    // Connect to server
    auto peer = clientHost->connect(*address, 2, 0);
    if (!peer) {
        return false;
    }

    // Wait for connection
    auto start = std::chrono::steady_clock::now();
    bool connected = false;
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(3)) {
        auto event = clientHost->service(100);
        if (event && event->type == NetworkEventType::CONNECT) {
            connected = true;
            break;
        }
    }

    if (!connected) {
        return false;
    }

    // Send REGISTER_REQUEST
    RegisterRequestData registerData;
    registerData.username = username;
    registerData.password = password;

    std::vector<uint8_t> payload = createRegisterRequest(registerData);
    std::vector<uint8_t> message =
        NetworkMessages::createMessage(NetworkMessages::MessageType::REGISTER_REQUEST, payload);
    auto packet = createPacket(message, static_cast<int>(PacketFlag::RELIABLE));
    peer->send(std::move(packet), 0);

    // Wait for response
    bool gotResponse = false;
    bool success = false;
    start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(3)) {
        auto event = clientHost->service(100);
        if (event && event->type == NetworkEventType::RECEIVE && event->packet) {
            NetworkMessages::MessageType msgType = NetworkMessages::getMessageType(event->packet->getData());
            if (msgType == NetworkMessages::MessageType::REGISTER_RESPONSE) {
                std::vector<uint8_t> responsePayload = NetworkMessages::getPayload(event->packet->getData());
                RegisterResponseData response = parseRegisterResponse(responsePayload);
                success = response.success;
                gotResponse = true;
                break;
            }
        }
    }

    peer->disconnect();
    return gotResponse && success;
}

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

    // Initialize Raylib window for login screen (will be reused for game)
    InitWindow(800, 600, "R-Type - Login");
    SetTargetFPS(60);

    if (!IsWindowReady()) {
        std::cerr << "ERROR: Window failed to initialize!" << std::endl;
        return 1;
    }

    std::cout << "Login screen initialized, entering loop..." << std::endl;

    LoginScreen loginScreen;
    std::string username, password, playerName;
    bool authenticated = false;

    // For async registration
    std::future<bool> registerFuture;
    bool isRegistering = false;
    std::string registerUsername, registerPassword;

    std::cout << "WindowShouldClose: " << WindowShouldClose() << ", authenticated: " << authenticated
              << std::endl;

    int loopCount = 0;
    while (!WindowShouldClose() && !authenticated) {
        if (loopCount == 0) {
            std::cout << "Entering loop iteration " << loopCount << std::endl;
        }
        loopCount++;

        // Check if registration is complete
        if (isRegistering && registerFuture.valid()) {
            if (registerFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                bool success = registerFuture.get();
                isRegistering = false;

                if (success) {
                    loginScreen.setSuccessMessage("✓ Account created! You can now login.");
                } else {
                    loginScreen.setErrorMessage("❌ Registration failed! Username already exists.");
                }
                loginScreen.reset();
            }
        }

        loginScreen.update();

        BeginDrawing();
        loginScreen.render();
        EndDrawing();

        if (loginScreen.isLoginSubmitted()) {
            username = loginScreen.getUsername();
            password = loginScreen.getPassword();
            playerName = username;
            authenticated = true;
        } else if (loginScreen.isGuestSubmitted()) {
            // Connect as guest with default credentials
            username = "guest";
            password = "guest";
            playerName = "Guest_" + std::to_string(std::time(nullptr) % 10000);
            authenticated = true;
            std::cout << "Connecting as guest: " << playerName << std::endl;
        } else if (loginScreen.isRegisterSubmitted()) {
            registerUsername = loginScreen.getUsername();
            registerPassword = loginScreen.getPassword();

            if (registerUsername.empty() || registerPassword.empty()) {
                loginScreen.setErrorMessage("Username and password required!");
                loginScreen.reset();
            } else if (!isRegistering) {
                loginScreen.setSuccessMessage("Sending registration...");

                // Launch async registration
                isRegistering = true;
                registerFuture = std::async(std::launch::async, sendRegisterRequest, registerUsername,
                                            registerPassword, host, port);
            }
        }
    }

    // Don't close the window - GLFW doesn't support closing and reopening
    // The game will reuse the same window instance

    if (!authenticated) {
        CloseWindow();  // Only close if user cancelled
        return 0;       // User closed window
    }

    // Create and run client with authentication
    // The window will be reused and reconfigured by the game
    Client client(playerName, username, password, host, port);

    if (!client.initialize()) {
        std::cerr << "Failed to initialize client" << std::endl;
        return 1;
    }

    client.run();

    return 0;
}
