/*
** EPITECH PROJECT, 2025
** Created by mael on 02/12/2025.
** File description:
** main.cpp
*/

#include <iostream>
#include "Client/Client.hpp"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <player_name> [host] [port]" << std::endl;
        std::cerr << "Example: " << argv[0] << " Alice 127.0.0.1 4242" << std::endl;
        return 1;
    }

    std::string playerName = argv[1];
    std::string host = (argc > 2) ? argv[2] : "127.0.0.1";
    uint16_t port = (argc > 3) ? static_cast<uint16_t>(std::atoi(argv[3])) : 4242;

    // Create and run client
    Client client(playerName, host, port);

    if (!client.initialize()) {
        std::cerr << "Failed to initialize client" << std::endl;
        return 1;
    }

    client.run();

    return 0;
}