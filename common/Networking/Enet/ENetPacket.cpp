/*
** EPITECH PROJECT, 2025
** Created by IamSwan on 06/12/2025.
** File description:
** ENetPacket.cpp
*/

#include "ENetPacket.hpp"
#include <cstring>
#include <stdexcept>

ENetPacketWrapper::ENetPacketWrapper(ENetPacket *packet) : _packet(packet), _dataCacheValid(false) {
    if (!_packet) {
        throw std::invalid_argument("ENetPacket cannot be null");
    }
}

ENetPacketWrapper::ENetPacketWrapper(const std::vector<uint8_t> &data, uint32_t flags)
    : _packet(nullptr), _dataCacheValid(false) {
    _packet = enet_packet_create(data.data(), data.size(), flags);
    if (!_packet) {
        throw std::runtime_error("Failed to create ENet packet");
    }
}

ENetPacketWrapper::~ENetPacketWrapper() {
    if (_packet) {
        enet_packet_destroy(_packet);
    }
}

ENetPacketWrapper::ENetPacketWrapper(ENetPacketWrapper &&other) noexcept
    : _packet(other._packet),
      _dataCache(std::move(other._dataCache)),
      _dataCacheValid(other._dataCacheValid) {
    other._packet = nullptr;
    other._dataCacheValid = false;
}

ENetPacketWrapper &ENetPacketWrapper::operator=(ENetPacketWrapper &&other) noexcept {
    if (this != &other) {
        if (_packet) {
            enet_packet_destroy(_packet);
        }
        _packet = other._packet;
        _dataCache = std::move(other._dataCache);
        _dataCacheValid = other._dataCacheValid;

        other._packet = nullptr;
        other._dataCacheValid = false;
    }
    return *this;
}

const std::vector<uint8_t> &ENetPacketWrapper::getData() const {
    if (!_dataCacheValid && _packet) {
        _dataCache.assign(_packet->data, _packet->data + _packet->dataLength);
        _dataCacheValid = true;
    }
    return _dataCache;
}

size_t ENetPacketWrapper::getSize() const {
    return _packet ? _packet->dataLength : 0;
}

uint32_t ENetPacketWrapper::getFlags() const {
    return _packet ? _packet->flags : 0;
}

void ENetPacketWrapper::setData(const std::vector<uint8_t> &data) {
    if (_packet) {
        enet_packet_destroy(_packet);
    }
    _packet = enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE);
    if (!_packet) {
        throw std::runtime_error("Failed to create ENet packet");
    }
    _dataCacheValid = false;
}

ENetPacket *ENetPacketWrapper::getNativePacket() const {
    return _packet;
}
