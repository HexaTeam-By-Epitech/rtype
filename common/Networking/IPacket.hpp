/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 06/12/2025.
** File description:
** IPacket.hpp
*/

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

/**
 * @enum PacketFlag
 * @brief Flags that control packet behavior and delivery guarantees.
 */
enum class PacketFlag : uint32_t {
    RELIABLE = 1 << 0,             ///< Packet must be received by the target peer and resent if dropped.
    UNSEQUENCED = 1 << 1,          ///< Packet will not be sequenced with other packets.
    NO_ALLOCATE = 1 << 2,          ///< Packet will not allocate data, and user must supply it.
    UNRELIABLE_FRAGMENT = 1 << 3,  ///< Packet will be fragmented using unreliable (default) sends.
    SENT = 1 << 8                  ///< Flag indicating packet has been sent from peer.
};

/**
 * @class IPacket
 * @brief Interface representing a network packet.
 *
 * IPacket encapsulates data to be sent or received over the network.
 * The concrete implementation (ENet, custom UDP, etc.) is hidden
 * behind this interface so the backend can be swapped without impacting
 * game code.
 */
class IPacket {
   public:
    virtual ~IPacket() = default;

    /**
     * @brief Get the packet data as a byte buffer.
     * @return Const reference to the packet's data.
     */
    [[nodiscard]] virtual const std::vector<uint8_t> &getData() const = 0;

    /**
     * @brief Get the size of the packet data in bytes.
     * @return Size of the packet.
     */
    [[nodiscard]] virtual size_t getSize() const = 0;

    /**
     * @brief Get the flags associated with this packet.
     * @return Packet flags.
     */
    [[nodiscard]] virtual uint32_t getFlags() const = 0;

    /**
     * @brief Set new data for this packet.
     * @param data The data to set.
     */
    virtual void setData(const std::vector<uint8_t> &data) = 0;
};
