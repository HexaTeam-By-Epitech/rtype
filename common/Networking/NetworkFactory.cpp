/*
** EPITECH PROJECT, 2025
** Created by IamSwan on 06/12/2025.
** File description:
** NetworkFactory.cpp
*/

#include "NetworkFactory.hpp"
#include <enet/enet.h>
#include "Enet/ENetAddress.hpp"
#include "Enet/ENetHost.hpp"
#include "Enet/ENetPacket.hpp"

bool initializeNetworking() {
    return enet_initialize() == 0;
}

void deinitializeNetworking() {
    enet_deinitialize();
}

std::unique_ptr<IHost> createServerHost(const IAddress &address, size_t maxClients, size_t channelLimit,
                                        uint32_t incomingBandwidth, uint32_t outgoingBandwidth) {
    return std::make_unique<ENetHostWrapper>(address, maxClients, channelLimit, incomingBandwidth,
                                             outgoingBandwidth);
}

std::unique_ptr<IHost> createClientHost(size_t channelLimit, uint32_t incomingBandwidth,
                                        uint32_t outgoingBandwidth) {
    return std::make_unique<ENetHostWrapper>(channelLimit, incomingBandwidth, outgoingBandwidth);
}

std::unique_ptr<IPacket> createPacket(const std::vector<uint8_t> &data, uint32_t flags) {
    return std::make_unique<ENetPacketWrapper>(data, flags);
}

std::unique_ptr<IAddress> createAddress(const std::string &host, uint16_t port) {
    return std::make_unique<ENetAddressWrapper>(host, port);
}
