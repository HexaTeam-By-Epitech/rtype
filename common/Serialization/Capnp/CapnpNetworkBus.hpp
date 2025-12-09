/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** CapnpNetworkBus.hpp
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/array.h>
#include <cstdint>
#include <vector>

/**
 * @namespace CapnpHelpers
 * @brief Helper functions for Cap'n Proto serialization/deserialization.
 *
 * Simple utilities to convert between Cap'n Proto messages and byte vectors.
 * No abstraction, no copying - just direct serialization helpers.
 */
namespace CapnpHelpers {

    /**
     * @brief Serialize a Cap'n Proto message builder to bytes.
     *
     * @param builder The message builder to serialize
     * @return Vector of bytes ready to be sent over network
     */
    std::vector<uint8_t> serialize(capnp::MallocMessageBuilder &builder);

    /**
     * @brief Create a Cap'n Proto reader from received bytes.
     *
     * @param data Byte vector received from network
     * @return Message reader wrapping the data (no copy)
     *
     * @note The data vector must remain valid while using the reader
     */
    capnp::FlatArrayMessageReader deserialize(const std::vector<uint8_t> &data);

}  // namespace CapnpHelpers