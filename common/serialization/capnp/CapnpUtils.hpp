/*
** EPITECH PROJECT, 2025
** Created by mael on 02/12/2025.
** File description:
** CapnpUtils.hpp
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/io.h>
#include <vector>

// Serialize a Cap'n Proto message builder into a vector<uint8_t>
inline std::vector<uint8_t> serializeCapnpMessage(capnp::MallocMessageBuilder &builder) {
    // Use kj::VectorOutputStream to collect bytes
    kj::VectorOutputStream output;
    capnp::writeMessage(output, builder);
    // Extract the underlying buffer (kj::ArrayPtr<const kj::byte>) -> convert to vector<uint8_t>
    auto arr = output.getArray();
    std::vector<uint8_t> out;
    out.reserve(arr.size());
    for (auto b : arr)
        out.push_back(static_cast<uint8_t>(b));
    return out;
}