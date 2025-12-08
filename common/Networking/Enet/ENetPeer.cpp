/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 06/12/2025.
** File description:
** ENetPeer.cpp
*/

#include "ENetPeer.hpp"
#include <stdexcept>
#include "ENetPacket.hpp"

ENetPeerWrapper::ENetPeerWrapper(ENetPeer *peer) : peer_(peer) {
    if (!peer_) {
        throw std::invalid_argument("ENetPeer cannot be null");
    }
}

bool ENetPeerWrapper::send(std::unique_ptr<IPacket> packet, uint8_t channelID) {
    auto *enetPacketWrapper = dynamic_cast<ENetPacketWrapper *>(packet.get());
    if (!enetPacketWrapper) {
        throw std::invalid_argument("Packet must be an ENetPacketWrapper");
    }

    ENetPacket *enetPacket = enetPacketWrapper->getENetPacket();

    // Release ownership from wrapper since ENet will manage the packet now
    packet.release();

    int result = enet_peer_send(peer_, channelID, enetPacket);
    return result == 0;
}

void ENetPeerWrapper::disconnect(uint32_t data) {
    enet_peer_disconnect(peer_, data);
}

void ENetPeerWrapper::disconnectNow(uint32_t data) {
    enet_peer_disconnect_now(peer_, data);
}

void ENetPeerWrapper::disconnectLater(uint32_t data) {
    enet_peer_disconnect_later(peer_, data);
}

PeerState ENetPeerWrapper::getState() const {
    return convertENetState(peer_->state);
}

const IAddress &ENetPeerWrapper::getAddress() const {
    if (!addressCache_) {
        addressCache_ = std::make_unique<ENetAddressWrapper>(peer_->address);
    }
    return *addressCache_;
}

uint32_t ENetPeerWrapper::getID() const {
    return peer_->connectID;
}

uint32_t ENetPeerWrapper::getRoundTripTime() const {
    return peer_->roundTripTime;
}

void ENetPeerWrapper::setData(void *data) {
    peer_->data = data;
}

void *ENetPeerWrapper::getData() const {
    return peer_->data;
}

PeerState ENetPeerWrapper::convertENetState(ENetPeerState state) {
    switch (state) {
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
