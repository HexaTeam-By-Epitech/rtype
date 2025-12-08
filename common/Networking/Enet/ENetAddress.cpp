/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 06/12/2025.
** File description:
** ENetAddress.cpp
*/

#include "ENetAddress.hpp"
#include <cstring>
#include <stdexcept>

ENetAddressWrapper::ENetAddressWrapper(const std::string &host, uint16_t port) {
    address_.port = port;

    if (host == "0.0.0.0" || host.empty()) {
        address_.host = ENET_HOST_ANY;
    } else {
        if (enet_address_set_host(&address_, host.c_str()) != 0) {
            throw std::runtime_error("Failed to set ENet host address: " + host);
        }
    }
}

ENetAddressWrapper::ENetAddressWrapper(const ENetAddress &address) : address_(address) {}

std::string ENetAddressWrapper::getHost() const {
    char hostBuffer[256];
    if (enet_address_get_host(&address_, hostBuffer, sizeof(hostBuffer)) != 0) {
        return "unknown";
    }
    return std::string(hostBuffer);
}

uint16_t ENetAddressWrapper::getPort() const {
    return address_.port;
}

void ENetAddressWrapper::setHost(const std::string &host) {
    if (host == "0.0.0.0" || host.empty()) {
        address_.host = ENET_HOST_ANY;
    } else {
        if (enet_address_set_host(&address_, host.c_str()) != 0) {
            throw std::runtime_error("Failed to set ENet host address: " + host);
        }
    }
}

void ENetAddressWrapper::setPort(uint16_t port) {
    address_.port = port;
}
