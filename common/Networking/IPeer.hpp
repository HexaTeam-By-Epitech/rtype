/*
** EPITECH PROJECT, 2025
** Created by IamSwan on 06/12/2025.
** File description:
** IPeer.hpp
*/

#pragma once

#include <cstdint>
#include <memory>
#include "IAddress.hpp"
#include "IPacket.hpp"

/**
 * @enum PeerState
 * @brief Represents the connection state of a peer.
 */
enum class PeerState {
    DISCONNECTED,              ///< Peer is disconnected.
    CONNECTING,                ///< Connection to peer is being established.
    ACKNOWLEDGING_CONNECT,     ///< Connection acknowledgment is being sent.
    CONNECTION_PENDING,        ///< Connection is pending.
    CONNECTION_SUCCEEDED,      ///< Connection has succeeded.
    CONNECTED,                 ///< Peer is connected.
    DISCONNECT_LATER,          ///< Peer is scheduled to disconnect.
    DISCONNECTING,             ///< Disconnection is in progress.
    ACKNOWLEDGING_DISCONNECT,  ///< Disconnect acknowledgment is being sent.
    ZOMBIE                     ///< Peer is in a zombie state.
};

/**
 * @class IPeer
 * @brief Interface representing a remote peer in the network.
 *
 * IPeer represents a connection to another endpoint (client or server).
 * The concrete implementation (ENetPeer, custom connection, etc.) is hidden
 * behind this interface to maintain backend independence.
 */
class IPeer {
   public:
    virtual ~IPeer() = default;

    /**
     * @brief Send a packet to this peer.
     *
     * @param packet The packet to send.
     * @param channelID The channel ID to send the packet on.
     * @return True if the packet was queued successfully, false otherwise.
     */
    virtual bool send(std::unique_ptr<IPacket> packet, uint8_t channelID = 0) = 0;

    /**
     * @brief Disconnect from this peer.
     *
     * @param data Optional user data to send with the disconnect notification.
     */
    virtual void disconnect(uint32_t data = 0) = 0;

    /**
     * @brief Force an immediate disconnect from this peer.
     *
     * @param data Optional user data to send with the disconnect notification.
     */
    virtual void disconnectNow(uint32_t data = 0) = 0;

    /**
     * @brief Disconnect from this peer after all queued packets are sent.
     *
     * @param data Optional user data to send with the disconnect notification.
     */
    virtual void disconnectLater(uint32_t data = 0) = 0;

    /**
     * @brief Get the current state of this peer.
     * @return The peer's state.
     */
    [[nodiscard]] virtual PeerState getState() const = 0;

    /**
     * @brief Get the address of this peer.
     * @return Reference to the peer's address.
     */
    [[nodiscard]] virtual const IAddress &getAddress() const = 0;

    /**
     * @brief Get a unique identifier for this peer.
     * @return Peer ID or connection index.
     */
    [[nodiscard]] virtual uint32_t getID() const = 0;

    /**
     * @brief Get the round-trip time (ping) to this peer.
     * @return RTT in milliseconds.
     */
    [[nodiscard]] virtual uint32_t getRoundTripTime() const = 0;

    /**
     * @brief Set application-specific data associated with this peer.
     * @param data Pointer to user data.
     */
    virtual void setData(void *data) = 0;

    /**
     * @brief Get application-specific data associated with this peer.
     * @return Pointer to user data.
     */
    [[nodiscard]] virtual void *getData() const = 0;
};
