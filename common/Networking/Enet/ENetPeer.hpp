/*
** EPITECH PROJECT, 2025
** Created by IamSwan on 06/12/2025.
** File description:
** ENetPeer.hpp
*/

#pragma once

#include <enet/enet.h>
#include <memory>
#include "ENetAddress.hpp"
#include "IPeer.hpp"

class ENetPeerWrapper final : public IPeer {
   public:
    explicit ENetPeerWrapper(ENetPeer *peer);

    bool send(std::unique_ptr<IPacket> packet, uint8_t channelID) override;
    void disconnect(uint32_t data) override;
    void disconnectNow(uint32_t data) override;
    void disconnectLater(uint32_t data) override;

    [[nodiscard]] PeerState getState() const override;
    [[nodiscard]] const IAddress &getAddress() const override;
    [[nodiscard]] uint32_t getID() const override;
    [[nodiscard]] uint32_t getRoundTripTime() const override;
    void setData(void *data) override;
    [[nodiscard]] void *getData() const override;

    [[nodiscard]] ENetPeer *getNativePeer() const;

   private:
    ENetPeer *peer_;
    mutable std::unique_ptr<ENetAddressWrapper> cachedAddress_;
};
