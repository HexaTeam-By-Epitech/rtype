/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 08/12/2025.
** File description:
** ENetPeer.cpp
*/

#include "ENetPeer.hpp"
#include <stdexcept>
#include "ENetAddress.hpp"
#include "ENetPacket.hpp"

ENetPeerWrapper::ENetPeerWrapper(ENetPeer *peer) : peer_(peer) {
    if (!peer_) {
        throw std::invalid_argument("ENetPeer cannot be null");
    }
}

bool ENetPeerWrapper::send(std::unique_ptr<IPacket> packet, uint8_t channelID) {
    if (!peer_ || !packet) {
        return false;
    }

    // Cast to ENetPacketWrapper to get native packet
    auto *enetPacket = dynamic_cast<ENetPacketWrapper *>(packet.get());
    if (!enetPacket) {
        throw std::invalid_argument("Packet must be an ENetPacketWrapper");
    }

    ENetPacket *nativePacket = enetPacket->getNativePacket();
    if (enet_peer_send(peer_, channelID, nativePacket) < 0) {
        return false;
    }

    // Release ownership since ENet now owns the packet
    (void)packet.release();
    return true;
}

void ENetPeerWrapper::disconnect(uint32_t data) {
    if (peer_) {
        enet_peer_disconnect(peer_, data);
    }
}

void ENetPeerWrapper::disconnectNow(uint32_t data) {
    if (peer_) {
        enet_peer_disconnect_now(peer_, data);
    }
}

void ENetPeerWrapper::disconnectLater(uint32_t data) {
    if (peer_) {
        enet_peer_disconnect_later(peer_, data);
    }
}

ENetPeer *ENetPeerWrapper::getNativePeer() const {
    return peer_;
}

PeerState ENetPeerWrapper::getState() const {
    if (!peer_) {
        return PeerState::DISCONNECTED;
    }

    switch (peer_->state) {
        case ENET_PEER_STATE_DISCONNECTED:
            return PeerState::DISCONNECTED;
        case ENET_PEER_STATE_CONNECTING:
            return PeerState::CONNECTING;
        case ENET_PEER_STATE_ACKNOWLEDGING_CONNECT:
            return PeerState::ACKNOWLEDGING_CONNECT;
        case ENET_PEER_STATE_CONNECTION_PENDING:
            return PeerState::CONNECTION_PENDING;
        case ENET_PEER_STATE_CONNECTION_SUCCEEDED:
            return PeerState::CONNECTION_SUCCEEDED;
        case ENET_PEER_STATE_CONNECTED:
            return PeerState::CONNECTED;
        case ENET_PEER_STATE_DISCONNECT_LATER:
            return PeerState::DISCONNECT_LATER;
        case ENET_PEER_STATE_DISCONNECTING:
            return PeerState::DISCONNECTING;
        case ENET_PEER_STATE_ACKNOWLEDGING_DISCONNECT:
            return PeerState::ACKNOWLEDGING_DISCONNECT;
        case ENET_PEER_STATE_ZOMBIE:
            return PeerState::ZOMBIE;
        default:
            return PeerState::DISCONNECTED;
    }
}

const IAddress &ENetPeerWrapper::getAddress() const {
    if (!peer_) {
        throw std::runtime_error("Cannot get address of null peer");
    }

    // Create and cache the address wrapper
    static thread_local ENetAddressWrapper addressWrapper(peer_->address);
    addressWrapper = ENetAddressWrapper(peer_->address);
    return addressWrapper;
}

uint32_t ENetPeerWrapper::getID() const {
    if (!peer_) {
        return 0;
    }
    return peer_->connectID;
}

uint32_t ENetPeerWrapper::getRoundTripTime() const {
    if (!peer_) {
        return 0;
    }
    return peer_->roundTripTime;
}

void ENetPeerWrapper::setData(void *data) {
    if (peer_) {
        peer_->data = data;
    }
}

void *ENetPeerWrapper::getData() const {
    if (!peer_) {
        return nullptr;
    }
    return peer_->data;
}
