/*
** EPITECH PROJECT, 2025
** Created by IamSwan on 06/12/2025.
** File description:
** ENetHost.cpp
*/

#include "ENetHost.hpp"
#include <stdexcept>
#include <string>
#include "ENetAddress.hpp"
#include "ENetPacket.hpp"
#include "ENetPeer.hpp"

// Client constructor
ENetHostWrapper::ENetHostWrapper(size_t maxConnections, size_t maxChannels, uint32_t incomingBandwidth,
                                 uint32_t outgoingBandwidth) {
    _host = enet_host_create(nullptr, maxConnections, maxChannels, incomingBandwidth, outgoingBandwidth);
    if (!_host) {
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
    _host = enet_host_create(&nativeAddr, maxConnections, maxChannels, incomingBandwidth, outgoingBandwidth);
    if (!_host) {
        // ENet doesn't provide specific error codes, but provide helpful context
        uint16_t port = ENET_NET_TO_HOST_16(nativeAddr.port);
        std::string errorMsg =
            "Failed to create ENet server host on port " + std::to_string(port) + ". Possible causes:\n" +
            "  - Port already in use (another server instance running?)\n" +
            "  - Insufficient permissions (try a port > 1024)\n" + "  - Invalid network configuration";
        throw std::runtime_error(errorMsg);
    }
}

ENetHostWrapper::~ENetHostWrapper() {
    if (_host) {
        enet_host_destroy(_host);
    }
}

IPeer *ENetHostWrapper::connect(const IAddress &address, size_t channelCount, uint32_t data) {
    const auto *enetAddr = dynamic_cast<const ENetAddressWrapper *>(&address);
    if (!enetAddr) {
        throw std::invalid_argument("Address must be an ENetAddressWrapper");
    }

    const ENetAddress &nativeAddr = enetAddr->getNativeAddress();
    ENetPeer *peer = enet_host_connect(_host, &nativeAddr, channelCount, data);
    if (!peer) {
        throw std::runtime_error("Failed to connect to host");
    }

    // Store peer wrapper
    auto peerWrapper = std::make_unique<ENetPeerWrapper>(peer);
    IPeer *result = peerWrapper.get();
    _peers[peer] = std::move(peerWrapper);

    return result;
}

std::optional<HostNetworkEvent> ENetHostWrapper::service(uint32_t timeout) {
    ENetEvent event;
    int result = enet_host_service(_host, &event, timeout);

    if (result > 0) {
        HostNetworkEvent netEvent;
        netEvent.channelID = event.channelID;

        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                netEvent.type = NetworkEventType::CONNECT;
                // Create peer wrapper if not exists
                if (_peers.find(event.peer) == _peers.end()) {
                    auto peerWrapper = std::make_unique<ENetPeerWrapper>(event.peer);
                    netEvent.peer = peerWrapper.get();
                    _peers[event.peer] = std::move(peerWrapper);
                } else {
                    netEvent.peer = _peers[event.peer].get();
                }
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                netEvent.type = NetworkEventType::DISCONNECT;
                if (_peers.find(event.peer) != _peers.end()) {
                    netEvent.peer = _peers[event.peer].get();
                }
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                netEvent.type = NetworkEventType::RECEIVE;
                if (_peers.find(event.peer) != _peers.end()) {
                    netEvent.peer = _peers[event.peer].get();
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
    enet_host_broadcast(_host, channelID, nativePacket);

    (void)packet.release();  // ENet owns the packet now
}

void ENetHostWrapper::flush() {
    if (_host) {
        enet_host_flush(_host);
    }
}

size_t ENetHostWrapper::getPeerCount() const {
    return _host ? _host->connectedPeers : 0;
}

const IAddress &ENetHostWrapper::getAddress() const {
    if (!_host) {
        throw std::runtime_error("Host is null");
    }

    // Create and cache the address wrapper in member variable
    // This ensures thread-safety and correct address for this specific host
    if (!_cachedAddress) {
        _cachedAddress = std::make_unique<ENetAddressWrapper>(_host->address);
    }
    return *_cachedAddress;
}
