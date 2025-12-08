/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 06/12/2025.
** File description:
** ENetHost.cpp
*/

#include "ENetHost.hpp"
#include <stdexcept>
#include "ENetAddress.hpp"
#include "ENetPacket.hpp"

ENetHostWrapper::ENetHostWrapper(const IAddress &address, size_t maxClients, size_t channelLimit,
                                 uint32_t incomingBandwidth, uint32_t outgoingBandwidth)
    : host_(nullptr) {
    const auto *enetAddr = dynamic_cast<const ENetAddressWrapper *>(&address);
    if (!enetAddr) {
        throw std::invalid_argument("Address must be an ENetAddressWrapper");
    }

    ENetAddress addr = enetAddr->getENetAddress();
    host_ = enet_host_create(&addr, maxClients, channelLimit, incomingBandwidth, outgoingBandwidth);

    if (!host_) {
        throw std::runtime_error("Failed to create ENet server host");
    }

    address_ = std::make_unique<ENetAddressWrapper>(addr);
}

ENetHostWrapper::ENetHostWrapper(size_t channelLimit, uint32_t incomingBandwidth, uint32_t outgoingBandwidth)
    : host_(nullptr) {
    host_ = enet_host_create(nullptr, 1, channelLimit, incomingBandwidth, outgoingBandwidth);

    if (!host_) {
        throw std::runtime_error("Failed to create ENet client host");
    }

    ENetAddress addr;
    addr.host = ENET_HOST_ANY;
    addr.port = 0;
    address_ = std::make_unique<ENetAddressWrapper>(addr);
}

ENetHostWrapper::~ENetHostWrapper() {
    if (host_) {
        enet_host_destroy(host_);
    }
}

IPeer *ENetHostWrapper::connect(const IAddress &address, size_t channelCount, uint32_t data) {
    const auto *enetAddr = dynamic_cast<const ENetAddressWrapper *>(&address);
    if (!enetAddr) {
        throw std::invalid_argument("Address must be an ENetAddressWrapper");
    }

    ENetAddress addr = enetAddr->getENetAddress();
    ENetPeer *peer = enet_host_connect(host_, &addr, channelCount, data);

    if (!peer) {
        return nullptr;
    }

    return getOrCreatePeerWrapper(peer);
}

std::optional<HostNetworkEvent> ENetHostWrapper::service(uint32_t timeout) {
    ENetEvent event;
    int result = enet_host_service(host_, &event, timeout);

    if (result <= 0) {
        return std::nullopt;
    }

    HostNetworkEvent netEvent;
    netEvent.type = convertENetEventType(event.type);
    netEvent.peer = getOrCreatePeerWrapper(event.peer);
    netEvent.channelID = event.channelID;
    netEvent.data = event.data;

    if (event.type == ENET_EVENT_TYPE_RECEIVE) {
        netEvent.packet = std::make_unique<ENetPacketWrapper>(event.packet);
    }

    return netEvent;
}

void ENetHostWrapper::broadcast(std::unique_ptr<IPacket> packet, uint8_t channelID) {
    auto *enetPacketWrapper = dynamic_cast<ENetPacketWrapper *>(packet.get());
    if (!enetPacketWrapper) {
        throw std::invalid_argument("Packet must be an ENetPacketWrapper");
    }

    ENetPacket *enetPacket = enetPacketWrapper->getENetPacket();

    // Release ownership since ENet will manage the packet
    packet.release();

    enet_host_broadcast(host_, channelID, enetPacket);
}

void ENetHostWrapper::flush() {
    enet_host_flush(host_);
}

size_t ENetHostWrapper::getPeerCount() const {
    return host_->connectedPeers;
}

const IAddress &ENetHostWrapper::getAddress() const {
    return *address_;
}

ENetPeerWrapper *ENetHostWrapper::getOrCreatePeerWrapper(ENetPeer *peer) {
    auto it = peers_.find(peer);
    if (it != peers_.end()) {
        return it->second.get();
    }

    auto wrapper = std::make_unique<ENetPeerWrapper>(peer);
    auto *ptr = wrapper.get();
    peers_[peer] = std::move(wrapper);
    return ptr;
}

NetworkEventType ENetHostWrapper::convertENetEventType(ENetEventType type) {
    switch (type) {
        case ENET_EVENT_TYPE_CONNECT:
            return NetworkEventType::CONNECT;
        case ENET_EVENT_TYPE_DISCONNECT:
            return NetworkEventType::DISCONNECT;
        case ENET_EVENT_TYPE_RECEIVE:
            return NetworkEventType::RECEIVE;
        case ENET_EVENT_TYPE_NONE:
        default:
            return NetworkEventType::NONE;
    }
}