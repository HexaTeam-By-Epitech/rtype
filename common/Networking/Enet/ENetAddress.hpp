/*
** EPITECH PROJECT, 2025
** Created by IamSwan on 06/12/2025.
** File description:
** ENetAddress.hpp
*/

#pragma once

#include <enet/enet.h>
#include <string>
#include "IAddress.hpp"

class ENetAddressWrapper final : public IAddress {
   public:
    explicit ENetAddressWrapper(const std::string &hostname, uint16_t port);
    explicit ENetAddressWrapper(const ENetAddress &address);
    ENetAddressWrapper(const ENetAddressWrapper &other);
    ENetAddressWrapper &operator=(const ENetAddressWrapper &other);

    [[nodiscard]] std::string getHost() const override;
    [[nodiscard]] uint16_t getPort() const override;
    void setHost(const std::string &hostname) override;
    void setPort(uint16_t port) override;

    [[nodiscard]] const ENetAddress &getNativeAddress() const;
    [[nodiscard]] ENetAddress &getNativeAddress();

   private:
    ENetAddress _address;
};
