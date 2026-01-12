/*
** EPITECH PROJECT, 2025
** Created by mael on 02/12/2025.
** File description:
** main.cpp
*/

#include <iostream>
#include <string>
#include "Client/Client.hpp"

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
    std::cout << "R-Type Client" << std::endl;
    std::cout << "Server: " << host << ":" << port << std::endl;
    std::cout << "==================================" << std::endl;
}

int main(int argc, char **argv) {
    std::string host = "127.0.0.1";
    uint16_t port = 4242;

    // Parse command line arguments
    parseCommandLine(argc, argv, host, port);
    printBanner(host, port);

    std::string playerName = "Player";  // Default name, will be updated by login

    // Create and initialize Client
    // The client handles the login phase internally before connecting
    Client client(playerName, host, port);

    if (!client.initialize()) {
        std::cerr << "Failed to initialize client" << std::endl;
        return 1;
    }

    // Run client - this will show login, connect, and start game
    client.run();

    return 0;
}
