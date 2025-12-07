/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 06/12/2025.
** File description:
** ENetAddress.hpp
*/

#pragma once

#include <enet/enet.h>
#include <cstdint>
#include <string>
#include "IAddress.hpp"

/**
 * @class ENetAddressWrapper
 * @brief Concrete IAddress implementation backed by ENet.
 *
 * ENetAddressWrapper wraps an ENetAddress and provides a clean interface
 * for address manipulation. This class should remain internal to the
 * networking backend.
 */
class ENetAddressWrapper final : public IAddress {
   public:
    /**
     * @brief Construct an address from host and port.
     * @param host Hostname or IP address.
     * @param port Port number.
     */
    explicit ENetAddressWrapper(const std::string &host = "0.0.0.0", uint16_t port = 0);

    /**
     * @brief Construct from an existing ENetAddress.
     * @param address ENet address to wrap.
     */
    explicit ENetAddressWrapper(const ENetAddress &address);

    ~ENetAddressWrapper() override = default;

    [[nodiscard]] std::string getHost() const override;
    [[nodiscard]] uint16_t getPort() const override;
    void setHost(const std::string &host) override;
    void setPort(uint16_t port) override;

    /**
     * @brief Get the underlying ENet address.
     * @return Reference to the ENet address.
     */
    [[nodiscard]] const ENetAddress &getENetAddress() const noexcept { return address_; }

    /**
     * @brief Get mutable reference to the underlying ENet address.
     * @return Reference to the ENet address.
     */
    [[nodiscard]] ENetAddress &getENetAddress() noexcept { return address_; }

   private:
    ENetAddress address_;
};
