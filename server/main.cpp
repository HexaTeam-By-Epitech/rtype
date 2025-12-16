/*
** EPITECH PROJECT, 2025
** Created by mael on 17/11/2025.
** File description:
** main.cpp
*/

#include <iostream>
#include "server/Server/Server.hpp"

int main(int argc, char **argv) {
    uint16_t port = 4242;
    if (argc > 1) {
        port = static_cast<uint16_t>(std::atoi(argv[1]));
    }

    // Create and run server
    Server server(port);

    if (!server.initialize()) {
        std::cerr << "Failed to initialize server" << std::endl;
        return 1;
    }

    server.run();

    return 0;
}