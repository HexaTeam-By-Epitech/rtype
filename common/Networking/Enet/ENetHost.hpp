/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 06/12/2025.
** File description:
** ENetHost.hpp
*/

#pragma once

#include <enet/enet.h>
#include <memory>
#include <optional>
#include <unordered_map>
#include "ENetPeer.hpp"
#include "IHost.hpp"
#include "IPeer.hpp"

/**
 * @class ENetHostWrapper
 * @brief Concrete IHost implementation backed by ENet.
 *
 * ENetHostWrapper wraps an ENetHost and provides a clean interface
 * for host operations. This class should remain internal to the
 * networking backend and not be exposed to client/server modules.
 */
class ENetHostWrapper final : public IHost {
   public:
    /**
     * @brief Construct a server host.
     * @param address Address to bind to.
     * @param maxClients Maximum number of clients.
     * @param channelLimit Maximum number of channels.
     * @param incomingBandwidth Incoming bandwidth limit.
     * @param outgoingBandwidth Outgoing bandwidth limit.
     */
    ENetHostWrapper(const IAddress &address, size_t maxClients, size_t channelLimit,
                    uint32_t incomingBandwidth, uint32_t outgoingBandwidth);

    /**
     * @brief Construct a client host.
     * @param channelLimit Maximum number of channels.
     * @param incomingBandwidth Incoming bandwidth limit.
     * @param outgoingBandwidth Outgoing bandwidth limit.
     */
    ENetHostWrapper(size_t channelLimit, uint32_t incomingBandwidth, uint32_t outgoingBandwidth);

    ~ENetHostWrapper() override;

    // Delete copy operations
    ENetHostWrapper(const ENetHostWrapper &) = delete;
    ENetHostWrapper &operator=(const ENetHostWrapper &) = delete;

    IPeer *connect(const IAddress &address, size_t channelCount = 1, uint32_t data = 0) override;
    std::optional<NetworkEvent> service(uint32_t timeout = 0) override;
    void broadcast(std::unique_ptr<IPacket> packet, uint8_t channelID = 0) override;
    void flush() override;
    [[nodiscard]] size_t getPeerCount() const override;
    [[nodiscard]] const IAddress &getAddress() const override;

   private:
    ENetHost *host_;
    std::unique_ptr<IAddress> address_;
    std::unordered_map<ENetPeer *, std::unique_ptr<ENetPeerWrapper>> peers_;

    ENetPeerWrapper *getOrCreatePeerWrapper(ENetPeer *peer);
    static EventType convertENetEventType(ENetEventType type);
};
