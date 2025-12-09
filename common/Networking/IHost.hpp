/*
** EPITECH PROJECT, 2025
** Created by IamSwan on 06/12/2025.
** File description:
** IHost.hpp
*/

#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include "IAddress.hpp"
#include "IPacket.hpp"
#include "IPeer.hpp"

/**
 * @enum NetworkEventType
 * @brief Type of network event.
 */
enum class NetworkEventType {
    NONE,        ///< No event occurred.
    CONNECT,     ///< A peer has connected.
    DISCONNECT,  ///< A peer has disconnected.
    RECEIVE      ///< A packet was received.
};

/**
 * @struct HostNetworkEvent
 * @brief Represents a network event (connection, disconnection, or received data).
 */
struct HostNetworkEvent {
    NetworkEventType type = NetworkEventType::NONE;  ///< Type of the event.
    IPeer *peer = nullptr;                           ///< Peer associated with the event.
    std::unique_ptr<IPacket> packet;                 ///< Packet received (only for RECEIVE events).
    uint8_t channelID = 0;                           ///< Channel on which the event occurred.
    uint32_t data = 0;                               ///< Additional data (e.g., disconnect reason).
};

/**
 * @class IHost
 * @brief Interface representing a network host (server or client endpoint).
 *
 * IHost manages connections and network communication. It can act as a server
 * accepting connections or as a client connecting to a server. The concrete
 * implementation (ENetHost, custom socket manager, etc.) is hidden behind
 * this interface to allow backend swapping.
 */
class IHost {
   public:
    virtual ~IHost() = default;

    /**
     * @brief Connect to a remote host.
     *
     * @param address The address to connect to.
     * @param channelCount Number of channels to allocate (default: 1).
     * @param data User data to send with the connection request.
     * @return Pointer to the peer object representing the connection, or nullptr on failure.
     */
    virtual IPeer *connect(const IAddress &address, size_t channelCount = 1, uint32_t data = 0) = 0;

    /**
     * @brief Service the host, processing network events.
     *
     * @param timeout Maximum time to wait for events in milliseconds.
     * @return Optional HostNetworkEvent if an event occurred, std::nullopt otherwise.
     */
    virtual std::optional<HostNetworkEvent> service(uint32_t timeout = 0) = 0;

    /**
     * @brief Broadcast a packet to all connected peers.
     *
     * @param packet The packet to broadcast.
     * @param channelID The channel to broadcast on.
     */
    virtual void broadcast(std::unique_ptr<IPacket> packet, uint8_t channelID = 0) = 0;

    /**
     * @brief Send all queued packets immediately.
     */
    virtual void flush() = 0;

    /**
     * @brief Get the number of connected peers.
     * @return Number of connected peers.
     */
    [[nodiscard]] virtual size_t getPeerCount() const = 0;

    /**
     * @brief Get the address this host is bound to.
     * @return Reference to the host's address.
     */
    [[nodiscard]] virtual const IAddress &getAddress() const = 0;
};