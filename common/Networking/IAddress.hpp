/*
** EPITECH PROJECT, 2025
** Created by IamSwan on 06/12/2025.
** File description:
** IAddress.hpp
*/

#pragma once

#include <cstdint>
#include <string>

/**
 * @class IAddress
 * @brief Interface representing a network address (IP + port).
 *
 * IAddress abstracts away the underlying network address representation,
 * allowing the backend (ENet, ASIO, etc.) to be swapped without impacting
 * client or server code.
 */
class IAddress {
   public:
    virtual ~IAddress() = default;

    /**
     * @brief Get the hostname or IP address as a string.
     * @return The host address.
     */
    [[nodiscard]] virtual std::string getHost() const = 0;

    /**
     * @brief Get the port number.
     * @return The port number.
     */
    [[nodiscard]] virtual uint16_t getPort() const = 0;

    /**
     * @brief Set the hostname or IP address.
     * @param host The hostname or IP address string.
     */
    virtual void setHost(const std::string &host) = 0;

    /**
     * @brief Set the port number.
     * @param port The port number.
     */
    virtual void setPort(uint16_t port) = 0;
};
