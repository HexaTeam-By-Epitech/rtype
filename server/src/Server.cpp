/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** Server.cpp
*/

#include "Server.hpp"

namespace server {

    Server::Server(std::string host) : _host(std::move(host)) {}

    const std::string &Server::getHost() const {
        return _host;
    }

    void Server::setHost(const std::string &host) {
        _host = host;
    }

    bool Server::isPingEqual(float p1, float p2) const {
        return p1 == p2;
    }

    void Server::handlePacket(LargePacket pkt) {
        (void)pkt;
    }

}  // namespace server
