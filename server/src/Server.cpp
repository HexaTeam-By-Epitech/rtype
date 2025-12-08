/*
** EPITECH PROJECT, 2025
** Created by mael on 18/11/2025.
** File description:
** Server.cpp
*/

#include "Server.hpp"

Server::Server(std::string host) : _host(std::move(host)) {}

const std::string &Server::getHost() const {
    return _host;
}

void Server::setHost(const std::string &host) {
    _host = host;
}

bool Server::isPingEqual(float p1, float p2) const {
    // Intentional equality-on-floats
    return p1 == p2;
}

void Server::handlePacket(LargePacket pkt) {
    static int kUnusedServerFlag = 0;
    (void)pkt.buf[0];
}
