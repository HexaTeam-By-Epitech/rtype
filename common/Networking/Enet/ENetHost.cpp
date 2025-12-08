/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 08/12/2025.
** File description:
** ENetHost.cpp
*/

#include "ENetHost.hpp"
#include <stdexcept>
#include "ENetAddress.hpp"
#include "ENetPacket.hpp"
#include "ENetPeer.hpp"

// Client constructor
ENetHostWrapper::ENetHostWrapper(size_t maxConnections, size_t maxChannels, uint32_t incomingBandwidth,
                                 uint32_t outgoingBandwidth) {
    host_ = enet_host_create(nullptr, maxConnections, maxChannels, incomingBandwidth, outgoingBandwidth);
    if (!host_) {
        throw std::runtime_error("Failed to create ENet client host");
    }
}

// Server constructor
ENetHostWrapper::ENetHostWrapper(const IAddress &address, size_t maxConnections, size_t maxChannels,
                                 uint32_t incomingBandwidth, uint32_t outgoingBandwidth) {
    const auto *enetAddr = dynamic_cast<const ENetAddressWrapper *>(&address);
    if (!enetAddr) {
        throw std::invalid_argument("Address must be an ENetAddressWrapper");
    }

    const ENetAddress &nativeAddr = enetAddr->getNativeAddress();
    host_ = enet_host_create(&nativeAddr, maxConnections, maxChannels, incomingBandwidth, outgoingBandwidth);
    if (!host_) {
        throw std::runtime_error("Failed to create ENet server host");
    }
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

    const ENetAddress &nativeAddr = enetAddr->getNativeAddress();
    ENetPeer *peer = enet_host_connect(host_, &nativeAddr, channelCount, data);
    if (!peer) {
        throw std::runtime_error("Failed to connect to host");
    }

    // Store peer wrapper
    auto peerWrapper = std::make_unique<ENetPeerWrapper>(peer);
    IPeer *result = peerWrapper.get();
    peers_[peer] = std::move(peerWrapper);

    return result;
}

std::optional<HostNetworkEvent> ENetHostWrapper::service(uint32_t timeout) {
    ENetEvent event;
    int result = enet_host_service(host_, &event, timeout);

    if (result > 0) {
        HostNetworkEvent netEvent;
        netEvent.channelID = event.channelID;

        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                netEvent.type = NetworkEventType::CONNECT;
                // Create peer wrapper if not exists
                if (peers_.find(event.peer) == peers_.end()) {
                    auto peerWrapper = std::make_unique<ENetPeerWrapper>(event.peer);
                    netEvent.peer = peerWrapper.get();
                    peers_[event.peer] = std::move(peerWrapper);
                } else {
                    netEvent.peer = peers_[event.peer].get();
                }
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                netEvent.type = NetworkEventType::DISCONNECT;
                if (peers_.find(event.peer) != peers_.end()) {
                    netEvent.peer = peers_[event.peer].get();
                }
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                netEvent.type = NetworkEventType::RECEIVE;
                if (peers_.find(event.peer) != peers_.end()) {
                    netEvent.peer = peers_[event.peer].get();
                }
                netEvent.packet = std::make_unique<ENetPacketWrapper>(event.packet);
                break;

            default:
                return std::nullopt;
        }

        return netEvent;
    }

    return std::nullopt;
}

void ENetHostWrapper::broadcast(std::unique_ptr<IPacket> packet, uint8_t channelID) {
    if (!packet) {
        return;
    }

    auto *enetPacket = dynamic_cast<ENetPacketWrapper *>(packet.get());
    if (!enetPacket) {
        throw std::invalid_argument("Packet must be an ENetPacketWrapper");
    }

    ENetPacket *nativePacket = enetPacket->getNativePacket();
    enet_host_broadcast(host_, channelID, nativePacket);

    packet.release();  // ENet owns the packet now
}

void ENetHostWrapper::flush() {
    if (host_) {
        enet_host_flush(host_);
    }
}

size_t ENetHostWrapper::getPeerCount() const {
    return host_ ? host_->connectedPeers : 0;
}

const IAddress &ENetHostWrapper::getAddress() const {
    if (!host_) {
        throw std::runtime_error("Host is null");
    }

    // Create address wrapper from host's address using the direct constructor
    static thread_local ENetAddressWrapper cachedAddress(host_->address);
    // Update the cached address with current host address
    cachedAddress = ENetAddressWrapper(host_->address);
    return cachedAddress;
}
