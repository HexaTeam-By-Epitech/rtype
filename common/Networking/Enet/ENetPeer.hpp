/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 06/12/2025.
** File description:
** ENetPeer.hpp
*/

#pragma once

#include <enet/enet.h>
#include <memory>
#include "ENetAddress.hpp"
#include "IAddress.hpp"
#include "IPeer.hpp"

/**
 * @class ENetPeerWrapper
 * @brief Concrete IPeer implementation backed by ENet.
 *
 * ENetPeerWrapper wraps an ENetPeer and provides a clean interface
 * for peer operations. This class should remain internal to the
 * networking backend and not be exposed to client/server modules.
 */
class ENetPeerWrapper final : public IPeer {
   public:
    /**
     * @brief Construct a peer wrapper from an ENet peer.
     * @param peer ENet peer (does not take ownership).
     */
    explicit ENetPeerWrapper(ENetPeer *peer);

    ~ENetPeerWrapper() override = default;

    bool send(std::unique_ptr<IPacket> packet, uint8_t channelID = 0) override;
    void disconnect(uint32_t data = 0) override;
    void disconnectNow(uint32_t data = 0) override;
    void disconnectLater(uint32_t data = 0) override;
    [[nodiscard]] PeerState getState() const override;
    [[nodiscard]] const IAddress &getAddress() const override;
    [[nodiscard]] uint32_t getID() const override;
    [[nodiscard]] uint32_t getRoundTripTime() const override;
    void setData(void *data) override;
    [[nodiscard]] void *getData() const override;

    /**
     * @brief Get the underlying ENet peer.
     * @return Pointer to the ENet peer.
     */
    [[nodiscard]] ENetPeer *getENetPeer() const noexcept { return peer_; }

   private:
    ENetPeer *peer_;
    mutable std::unique_ptr<ENetAddressWrapper> addressCache_;

    static PeerState convertENetState(ENetPeerState state);
};
