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

// Authenticate with server using only Replicator (no Client initialization)
bool authenticateWithServer(const std::string &playerName, const std::string &username,
                            const std::string &password, const std::string &host, uint16_t port) {
    // Create minimal EventBus for Replicator
    EventBus eventBus;
    Replicator replicator(eventBus);

    LOG_INFO("Connecting to ", host, ":", port, "...");

    if (!replicator.connect(host, port)) {
        LOG_ERROR("Failed to initiate connection");
        return false;
    }

    // Wait for connection
    LOG_INFO("Waiting for connection...");
    bool connected = false;
    for (int i = 0; i < 50 && !connected; ++i) {
        replicator.processMessages();
        connected = replicator.isConnected();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!connected) {
        LOG_ERROR("Connection timeout");
        return false;
    }

    LOG_INFO("✓ Connected to server!");

    // Send authentication request
    LOG_INFO("Sending authentication request...");
    if (!replicator.sendConnectRequest(playerName, username, password)) {
        LOG_ERROR("Failed to send connect request");
        return false;
    }

    // Wait for server response
    LOG_INFO("Waiting for server response...");
    for (int i = 0; i < 30; ++i) {
        replicator.processMessages();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Check authentication result
    if (!replicator.isAuthenticated()) {
        LOG_ERROR("Authentication failed - server rejected credentials");
        return false;
    }

    LOG_INFO("✓ Authentication successful!");

    // Disconnect this temporary connection
    replicator.disconnect();
    return true;
}

// Handle async registration check
void handleRegistrationStatus(bool &isRegistering, std::future<bool> &registerFuture,
                              LoginScreen &loginScreen) {
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
}

// Handle login screen user actions
void handleLoginScreenActions(LoginScreen &loginScreen, std::string &username, std::string &password,
                              std::string &playerName, bool &authenticated, bool &isRegistering,
                              std::future<bool> &registerFuture, const std::string &host, uint16_t port) {
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
    } else if (loginScreen.isRegisterSubmitted()) {
        std::string registerUsername = loginScreen.getUsername();
        std::string registerPassword = loginScreen.getPassword();

        if (registerUsername.empty() || registerPassword.empty()) {
            loginScreen.setErrorMessage("Username and password required!");
            loginScreen.reset();
        } else if (!isRegistering) {
            loginScreen.setSuccessMessage("Sending registration...");
            isRegistering = true;
            registerFuture = std::async(std::launch::async, sendRegisterRequest, registerUsername,
                                        registerPassword, host, port);
        }
    }
}

// Run login screen loop
bool runLoginScreen(LoginScreen &loginScreen, std::string &username, std::string &password,
                    std::string &playerName, const std::string &host, uint16_t port) {
    bool authenticated = false;
    std::future<bool> registerFuture;
    bool isRegistering = false;

    while (!WindowShouldClose() && !authenticated) {
        handleRegistrationStatus(isRegistering, registerFuture, loginScreen);
        loginScreen.update();

        BeginDrawing();
        loginScreen.render();
        EndDrawing();

        handleLoginScreenActions(loginScreen, username, password, playerName, authenticated, isRegistering,
                                 registerFuture, host, port);
    }

    return authenticated;
}

// Initialize Raylib window
bool initializeWindow() {
    InitWindow(800, 600, "R-Type - Login");
    SetTargetFPS(60);

    if (!IsWindowReady()) {
        std::cerr << "ERROR: Window failed to initialize!" << std::endl;
        return false;
    }

    return true;
}

// Parse command line arguments
void parseCommandLine(int argc, char **argv, std::string &host, uint16_t &port) {
    if (argc > 1) {
        host = argv[1];
    }
    if (argc > 2) {
        port = static_cast<uint16_t>(std::atoi(argv[2]));
    }
}

// Print welcome banner
void printBanner(const std::string &host, uint16_t port) {
    std::cout << "==================================" << std::endl;
    std::cout << "R-Type Client with Authentication" << std::endl;
    std::cout << "Server: " << host << ":" << port << std::endl;
    std::cout << "==================================" << std::endl;
}

int main(int argc, char **argv) {
    std::string host = "127.0.0.1";
    uint16_t port = 4242;

    // Parse command line arguments
    parseCommandLine(argc, argv, host, port);
    printBanner(host, port);

    // Initialize Raylib window
    if (!initializeWindow()) {
        return 1;
    }

    LoginScreen loginScreen;
    std::string username, password, playerName;

    // Initial login screen
    bool authenticated = runLoginScreen(loginScreen, username, password, playerName, host, port);

    if (!authenticated) {
        CloseWindow();
        return 0;
    }

    // Main game loop with authentication retry
    bool gameRunning = true;
    while (gameRunning && !WindowShouldClose()) {
        // Try authentication before creating full Client
        bool authSuccess = authenticateWithServer(playerName, username, password, host, port);

        if (!authSuccess) {
            std::cerr << "Authentication failed - returning to login screen" << std::endl;
            loginScreen.setErrorMessage("❌ Authentication failed! Check your credentials.");
            loginScreen.reset();

            // Return to login screen for retry
            authenticated = runLoginScreen(loginScreen, username, password, playerName, host, port);

            if (WindowShouldClose() || !authenticated) {
                gameRunning = false;
                continue;
            }
        }

        // Authentication successful - create and run game client
        Client client(playerName, username, password, host, port);

        if (!client.initialize()) {
            std::cerr << "Failed to initialize client" << std::endl;
            gameRunning = false;
            break;
        }

        client.run();
        gameRunning = false;
    }

    CloseWindow();
    return 0;
}
