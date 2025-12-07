/*
** EPITECH PROJECT, 2025
** Created by mael on 02/12/2025.
** File description:
** GameCodec.hpp
*/

#pragma once

#include "ISerializer.hpp"
#include "GameMessages.hpp"
#include <vector>
#include <memory>

/**
 * @class IGameCodec
 * @brief High-level encoding/decoding interface for gameplay messages.
 *
 * IGameCodec sits on top of ISerializer. It knows how to convert
 * domain-specific IMessage implementations (such as SpawnEntityMessage)
 * to/from raw byte buffers suitable for transport. The concrete
 * implementation is free to use Cap'n Proto, another binary format, or
 * any other backend.
 */
class IGameCodec {
public:
    virtual ~IGameCodec() = default;

    /**
     * @brief Access the underlying low-level serializer.
     *
     * This allows systems that need raw IMessage / byte access to still use
     * the same backend, while most gameplay code interacts only with
     * gameplay-centric methods like encodeSpawn/decodeSpawn.
     */
    virtual ISerializer &serializer() noexcept = 0;

    /**
     * @brief Encode a SpawnEntityMessage into a binary buffer.
     *
     * @param message High-level spawn message containing gameplay data.
     * @return Binary buffer ready to be sent over the network.
     */
    virtual std::vector<uint8_t> encodeSpawn(const SpawnEntityMessage &message) = 0;

    /**
     * @brief Decode a binary buffer into a SpawnEntityMessage DTO.
     *
     * @param data Binary data received from the network.
     * @return Concrete SpawnEntityMessage instance populated with decoded data.
     */
    virtual SpawnEntityMessage decodeSpawn(const std::vector<uint8_t> &data) = 0;
};

/**
 * @brief Factory function to create the default game codec.
 *
 * This is the main entry point for gameplay code that wants to work with
 * high-level messages only. The returned codec will be backed by the
 * default serializer (currently Cap'n Proto).
 */
std::unique_ptr<IGameCodec> createDefaultGameCodec();