/*
** EPITECH PROJECT, 2025
** Created by IamSwan on 06/12/2025.
** File description:
** ENetAddress.cpp
*/

#include "ENetAddress.hpp"
#include <cstring>
#include <stdexcept>

ENetAddressWrapper::ENetAddressWrapper(const std::string &hostname, uint16_t port) {
    _address.port = port;

    if (enet_address_set_host(&_address, hostname.c_str()) != 0) {
        throw std::runtime_error("Failed to set ENet host address: " + hostname);
    }
}

ENetAddressWrapper::ENetAddressWrapper(const ENetAddress &address) : _address(address) {}

ENetAddressWrapper::ENetAddressWrapper(const ENetAddressWrapper &other) : _address(other._address) {}

ENetAddressWrapper &ENetAddressWrapper::operator=(const ENetAddressWrapper &other) {
    if (this != &other) {
        _address = other._address;
    }
    return *this;
}

std::string ENetAddressWrapper::getHost() const {
    char buffer[256];
    if (enet_address_get_host(&_address, buffer, sizeof(buffer)) == 0) {
        return std::string(buffer);
    }
    return "";
}

uint16_t ENetAddressWrapper::getPort() const {
    return _address.port;
}

const ENetAddress &ENetAddressWrapper::getNativeAddress() const {
    return _address;
}

ENetAddress &ENetAddressWrapper::getNativeAddress() {
    return _address;
}

void ENetAddressWrapper::setHost(const std::string &hostname) {
    if (enet_address_set_host(&_address, hostname.c_str()) != 0) {
        throw std::runtime_error("Failed to set ENet host address: " + hostname);
    }
}

void ENetAddressWrapper::setPort(uint16_t port) {
    _address.port = port;
}
