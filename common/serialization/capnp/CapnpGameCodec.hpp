/*
** EPITECH PROJECT, 2025
** Created by mael on 02/12/2025.
** File description:
** CapnpGameCodec.hpp
*/

#pragma once

#include "GameCodec.hpp"
#include "CapnpSerializer.hpp"
#include "CapnpFactories.hpp"
#include <capnp/message.h>

/**
 * @class CapnpGameCodec
 * @brief Cap'n Proto-based implementation of IGameCodec.
 *
 * CapnpGameCodec demonstrates how to bridge between high-level gameplay
 * messages (e.g. SpawnEntityMessage) and the Cap'n Proto backend. It keeps
 * all Cap'n Proto-specific code inside the serialization module so that
 * client/server code remains backend-agnostic.
 */
class CapnpGameCodec final : public IGameCodec {
public:
    CapnpGameCodec();
    ~CapnpGameCodec() override = default;

    ISerializer &serializer() noexcept override { return _serializer; }

    std::vector<uint8_t> encodeSpawn(const SpawnEntityMessage &message) override;
    SpawnEntityMessage decodeSpawn(const std::vector<uint8_t> &data) override;

private:
    CapnpSerializer _serializer;
};