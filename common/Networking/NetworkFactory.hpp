/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 06/12/2025.
** File description:
** NetworkFactory.hpp
*/

#pragma once

#include <cstdint>
#include <memory>
#include "IAddress.hpp"
#include "IHost.hpp"
#include "IPacket.hpp"

/**
 * @brief Create a host for server-side networking.
 *
 * This function creates a host that listens for incoming connections on the
 * specified address. The concrete implementation (ENet, custom backend) is
 * hidden from the caller.
 *
 * @param address Address to bind the server to.
 * @param maxClients Maximum number of clients that can connect.
 * @param channelLimit Maximum number of channels per connection.
 * @param incomingBandwidth Incoming bandwidth limit (0 = unlimited).
 * @param outgoingBandwidth Outgoing bandwidth limit (0 = unlimited).
 * @return Unique pointer to an IHost instance configured as a server.
 */
std::unique_ptr<IHost> createServerHost(const IAddress &address, size_t maxClients = 32,
                                        size_t channelLimit = 2, uint32_t incomingBandwidth = 0,
                                        uint32_t outgoingBandwidth = 0);

/**
 * @brief Create a host for client-side networking.
 *
 * This function creates a host that can initiate connections to servers.
 * The concrete implementation (ENet, custom backend) is hidden from the caller.
 *
 * @param channelLimit Maximum number of channels per connection.
 * @param incomingBandwidth Incoming bandwidth limit (0 = unlimited).
 * @param outgoingBandwidth Outgoing bandwidth limit (0 = unlimited).
 * @return Unique pointer to an IHost instance configured as a client.
 */
std::unique_ptr<IHost> createClientHost(size_t channelLimit = 2, uint32_t incomingBandwidth = 0,
                                        uint32_t outgoingBandwidth = 0);

/**
 * @brief Create a network packet with the given data and flags.
 *
 * @param data The data to include in the packet.
 * @param flags Packet flags controlling delivery guarantees.
 * @return Unique pointer to an IPacket instance.
 */
std::unique_ptr<IPacket> createPacket(const std::vector<uint8_t> &data,
                                      uint32_t flags = static_cast<uint32_t>(PacketFlag::RELIABLE));

/**
 * @brief Create a network address.
 *
 * @param host The hostname or IP address.
 * @param port The port number.
 * @return Unique pointer to an IAddress instance.
 */
std::unique_ptr<IAddress> createAddress(const std::string &host = "0.0.0.0", uint16_t port = 0);

/**
 * @brief Initialize the networking subsystem.
 *
 * Must be called before using any networking functions.
 * @return True if initialization succeeded, false otherwise.
 */
bool initializeNetworking();

/**
 * @brief Cleanup the networking subsystem.
 *
 * Should be called when networking is no longer needed.
 */
void deinitializeNetworking();
