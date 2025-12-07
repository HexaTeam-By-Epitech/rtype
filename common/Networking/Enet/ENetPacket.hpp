/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 06/12/2025.
** File description:
** ENetPacket.hpp
*/

#pragma once

#include <enet/enet.h>
#include <cstdint>
#include <vector>
#include "IPacket.hpp"

/**
 * @class ENetPacketWrapper
 * @brief Concrete IPacket implementation backed by ENet.
 *
 * ENetPacketWrapper wraps an ENetPacket and provides a clean interface
 * for the rest of the application. This class should remain internal
 * to the networking backend and not be exposed to client/server modules.
 */
class ENetPacketWrapper final : public IPacket {
   public:
    /**
     * @brief Construct a packet wrapper from existing ENet packet.
     * @param packet ENet packet (takes ownership).
     */
    explicit ENetPacketWrapper(ENetPacket *packet);

    /**
     * @brief Construct a new packet with given data and flags.
     * @param data The data to include in the packet.
     * @param flags Packet flags.
     */
    ENetPacketWrapper(const std::vector<uint8_t> &data, uint32_t flags);

    ~ENetPacketWrapper() override;

    // Delete copy operations
    ENetPacketWrapper(const ENetPacketWrapper &) = delete;
    ENetPacketWrapper &operator=(const ENetPacketWrapper &) = delete;

    // Allow move operations
    ENetPacketWrapper(ENetPacketWrapper &&other) noexcept;
    ENetPacketWrapper &operator=(ENetPacketWrapper &&other) noexcept;

    [[nodiscard]] const std::vector<uint8_t> &getData() const override;
    [[nodiscard]] size_t getSize() const override;
    [[nodiscard]] uint32_t getFlags() const override;
    void setData(const std::vector<uint8_t> &data) override;

    /**
     * @brief Get the underlying ENet packet.
     * @return Pointer to the ENet packet.
     */
    [[nodiscard]] ENetPacket *getENetPacket() const noexcept { return packet_; }

   private:
    ENetPacket *packet_;
    mutable std::vector<uint8_t> dataCache_;
    mutable bool dataCacheValid_;

    void invalidateCache();
};
