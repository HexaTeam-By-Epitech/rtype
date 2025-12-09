/*
** EPITECH PROJECT, 2025
** Created by IamSwan on 06/12/2025.
** File description:
** ENetPeer.cpp
*/

#include "ENetPeer.hpp"
#include <stdexcept>
#include "ENetAddress.hpp"
#include "ENetPacket.hpp"

ENetPeerWrapper::ENetPeerWrapper(ENetPeer *peer) : _peer(peer) {
    if (!_peer) {
        throw std::invalid_argument("ENetPeer cannot be null");
    }
}

bool ENetPeerWrapper::send(std::unique_ptr<IPacket> packet, uint8_t channelID) {
    if (!_peer || !packet) {
        return false;
    }

    // Cast to ENetPacketWrapper to get native packet
    auto *enetPacket = dynamic_cast<ENetPacketWrapper *>(packet.get());
    if (!enetPacket) {
        throw std::invalid_argument("Packet must be an ENetPacketWrapper");
    }

    ENetPacket *nativePacket = enetPacket->getNativePacket();
    if (enet_peer_send(_peer, channelID, nativePacket) < 0) {
        return false;
    }

    // Release ownership since ENet now owns the packet
    (void)packet.release();
    return true;
}

void ENetPeerWrapper::disconnect(uint32_t data) {
    if (_peer) {
        enet_peer_disconnect(_peer, data);
    }
}

void ENetPeerWrapper::disconnectNow(uint32_t data) {
    if (_peer) {
        enet_peer_disconnect_now(_peer, data);
    }
}

void ENetPeerWrapper::disconnectLater(uint32_t data) {
    if (_peer) {
        enet_peer_disconnect_later(_peer, data);
    }
}

ENetPeer *ENetPeerWrapper::getNativePeer() const {
    return _peer;
}

PeerState ENetPeerWrapper::getState() const {
    if (!_peer) {
        return PeerState::DISCONNECTED;
    }

    switch (_peer->state) {
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
    if (!_peer) {
        throw std::runtime_error("Cannot get address of null peer");
    }

    // Create and cache the address wrapper in member variable
    // This ensures thread-safety and correct address for this specific peer
    if (!_cachedAddress) {
        _cachedAddress = std::make_unique<ENetAddressWrapper>(_peer->address);
    }
    return *_cachedAddress;
}

uint32_t ENetPeerWrapper::getID() const {
    if (!_peer) {
        return 0;
    }
    return _peer->connectID;
}

uint32_t ENetPeerWrapper::getRoundTripTime() const {
    if (!_peer) {
        return 0;
    }
    return _peer->roundTripTime;
}

void ENetPeerWrapper::setData(void *data) {
    if (_peer) {
        _peer->data = data;
    }
}

void *ENetPeerWrapper::getData() const {
    if (!_peer) {
        return nullptr;
    }
    return _peer->data;
}
