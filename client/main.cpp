/*
** EPITECH PROJECT, 2025
** Created by mael on 02/12/2025.
** File description:
** main.cpp
*/

#include "Client/Client.hpp"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <player_name> [host] [port] [--spectator]" << '\n';
        std::cerr << "Example: " << argv[0] << " Alice 127.0.0.1 4242" << '\n';
        std::cerr << "         " << argv[0] << " Bob 127.0.0.1 4242 --spectator" << '\n';
        std::cerr << "         " << argv[0] << " Charlie --spectator" << '\n';
        return 1;
    }

    const std::string playerName = argv[1];

    // Check for spectator mode first
    bool isSpectator = false;
    for (int i = 2; i < argc; ++i) {
        if (std::string(argv[i]) == "--spectator") {
            isSpectator = true;
            break;
        }
    }

    // Parse host and port (skip --spectator if present)
    std::string host = "127.0.0.1";
    uint16_t port = 4242;

    int argIndex = 2;
    if (argc > 2 && std::string(argv[2]) != "--spectator") {
        host = argv[2];
        argIndex = 3;
    }

    if (argc > argIndex && std::string(argv[argIndex]) != "--spectator") {
        port = static_cast<uint16_t>(std::atoi(argv[argIndex]));
    }

    // Create and run client
    Client client(playerName, host, port, isSpectator);

    if (!client.initialize()) {
        std::cerr << "Failed to initialize client" << std::endl;
        return 1;
    }

    client.run();

    return 0;
}
