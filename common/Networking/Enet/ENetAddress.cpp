/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 08/12/2025.
** File description:
** ENetAddress.cpp
*/

#include "ENetAddress.hpp"
#include <cstring>
#include <stdexcept>

ENetAddressWrapper::ENetAddressWrapper(const std::string &hostname, uint16_t port) {
    address_.port = port;

    if (enet_address_set_host(&address_, hostname.c_str()) != 0) {
        throw std::runtime_error("Failed to set ENet host address: " + hostname);
    }
}

ENetAddressWrapper::ENetAddressWrapper(const ENetAddress &address) : address_(address) {}

ENetAddressWrapper::ENetAddressWrapper(const ENetAddressWrapper &other) : address_(other.address_) {}

ENetAddressWrapper &ENetAddressWrapper::operator=(const ENetAddressWrapper &other) {
    if (this != &other) {
        address_ = other.address_;
    }
    return *this;
}

std::string ENetAddressWrapper::getHost() const {
    char buffer[256];
    if (enet_address_get_host(&address_, buffer, sizeof(buffer)) == 0) {
        return std::string(buffer);
    }
    return "";
}

uint16_t ENetAddressWrapper::getPort() const {
    return address_.port;
}

const ENetAddress &ENetAddressWrapper::getNativeAddress() const {
    return address_;
}

ENetAddress &ENetAddressWrapper::getNativeAddress() {
    return address_;
}

void ENetAddressWrapper::setHost(const std::string &hostname) {
    if (enet_address_set_host(&address_, hostname.c_str()) != 0) {
        throw std::runtime_error("Failed to set ENet host address: " + hostname);
    }
}

void ENetAddressWrapper::setPort(uint16_t port) {
    address_.port = port;
}
