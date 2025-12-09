/*
** EPITECH PROJECT, 2025
** Created by IamSwan on 06/12/2025.
** File description:
** ENetHost.hpp
*/

#pragma once

#include <enet/enet.h>
#include <map>
#include <memory>
#include "IHost.hpp"

class ENetPeerWrapper;

class ENetHostWrapper final : public IHost {
   public:
    // Client constructor
    explicit ENetHostWrapper(size_t maxConnections = 1, size_t maxChannels = 2,
                             uint32_t incomingBandwidth = 0, uint32_t outgoingBandwidth = 0);

    // Server constructor
    ENetHostWrapper(const IAddress &address, size_t maxConnections, size_t maxChannels = 2,
                    uint32_t incomingBandwidth = 0, uint32_t outgoingBandwidth = 0);

    ~ENetHostWrapper() override;

    ENetHostWrapper(const ENetHostWrapper &) = delete;
    ENetHostWrapper &operator=(const ENetHostWrapper &) = delete;

    IPeer *connect(const IAddress &address, size_t channelCount, uint32_t data) override;
    std::optional<HostNetworkEvent> service(uint32_t timeout) override;
    void broadcast(std::unique_ptr<IPacket> packet, uint8_t channelID) override;
    void flush() override;

    [[nodiscard]] size_t getPeerCount() const override;
    [[nodiscard]] const IAddress &getAddress() const override;

   private:
    ENetHost *host_;
    std::map<ENetPeer *, std::unique_ptr<ENetPeerWrapper>> peers_;
};
