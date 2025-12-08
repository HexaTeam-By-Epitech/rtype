/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 08/12/2025.
** File description:
** ENetPacket.hpp
*/

#pragma once

#include <enet/enet.h>
#include <cstdint>
#include <vector>
#include "IPacket.hpp"

class ENetPacketWrapper final : public IPacket {
   public:
    explicit ENetPacketWrapper(ENetPacket *packet);
    ENetPacketWrapper(const std::vector<uint8_t> &data, uint32_t flags);
    ~ENetPacketWrapper() override;

    ENetPacketWrapper(const ENetPacketWrapper &) = delete;
    ENetPacketWrapper &operator=(const ENetPacketWrapper &) = delete;

    ENetPacketWrapper(ENetPacketWrapper &&other) noexcept;
    ENetPacketWrapper &operator=(ENetPacketWrapper &&other) noexcept;

    [[nodiscard]] const std::vector<uint8_t> &getData() const override;
    [[nodiscard]] size_t getSize() const override;
    [[nodiscard]] uint32_t getFlags() const override;
    void setData(const std::vector<uint8_t> &data) override;

    [[nodiscard]] ENetPacket *getNativePacket() const;

   private:
    ENetPacket *packet_;
    mutable std::vector<uint8_t> dataCache_;
    mutable bool dataCacheValid_;
};
