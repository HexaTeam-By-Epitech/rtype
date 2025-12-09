/*
** EPITECH PROJECT, 2025
** Created by IamSwan on 06/12/2025.
** File description:
** ENetPacket.cpp
*/

#include "ENetPacket.hpp"
#include <cstring>
#include <stdexcept>

ENetPacketWrapper::ENetPacketWrapper(ENetPacket *packet) : packet_(packet), dataCacheValid_(false) {
    if (!packet_) {
        throw std::invalid_argument("ENetPacket cannot be null");
    }
}

ENetPacketWrapper::ENetPacketWrapper(const std::vector<uint8_t> &data, uint32_t flags)
    : packet_(nullptr), dataCacheValid_(false) {
    packet_ = enet_packet_create(data.data(), data.size(), flags);
    if (!packet_) {
        throw std::runtime_error("Failed to create ENet packet");
    }
}

ENetPacketWrapper::~ENetPacketWrapper() {
    if (packet_) {
        enet_packet_destroy(packet_);
    }
}

ENetPacketWrapper::ENetPacketWrapper(ENetPacketWrapper &&other) noexcept
    : packet_(other.packet_),
      dataCache_(std::move(other.dataCache_)),
      dataCacheValid_(other.dataCacheValid_) {
    other.packet_ = nullptr;
    other.dataCacheValid_ = false;
}

ENetPacketWrapper &ENetPacketWrapper::operator=(ENetPacketWrapper &&other) noexcept {
    if (this != &other) {
        if (packet_) {
            enet_packet_destroy(packet_);
        }
        packet_ = other.packet_;
        dataCache_ = std::move(other.dataCache_);
        dataCacheValid_ = other.dataCacheValid_;

        other.packet_ = nullptr;
        other.dataCacheValid_ = false;
    }
    return *this;
}

const std::vector<uint8_t> &ENetPacketWrapper::getData() const {
    if (!dataCacheValid_ && packet_) {
        dataCache_.assign(packet_->data, packet_->data + packet_->dataLength);
        dataCacheValid_ = true;
    }
    return dataCache_;
}

size_t ENetPacketWrapper::getSize() const {
    return packet_ ? packet_->dataLength : 0;
}

uint32_t ENetPacketWrapper::getFlags() const {
    return packet_ ? packet_->flags : 0;
}

void ENetPacketWrapper::setData(const std::vector<uint8_t> &data) {
    if (packet_) {
        enet_packet_destroy(packet_);
    }
    packet_ = enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE);
    if (!packet_) {
        throw std::runtime_error("Failed to create ENet packet");
    }
    dataCacheValid_ = false;
}

ENetPacket *ENetPacketWrapper::getNativePacket() const {
    return packet_;
}
