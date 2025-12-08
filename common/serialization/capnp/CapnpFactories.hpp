/*
** EPITECH PROJECT, 2025
** Created by mael on 02/12/2025.
** File description:
** CapnpFactories.hpp
*/

#pragma once

#include <memory>
#include <vector>
#include "CapnpBuilderMessage.hpp"
#include "CapnpMessage.hpp"
#include "ISerializer.hpp"

// NOTE: This header is intended for internal use inside the Cap'n Proto backend implementation.
// It should *not* be included by client/server code. Only `ISerializer.hpp` should be exposed as
// the public serialization API.

// Factory helper to wrap raw bytes into an IMessage implementation.
// This is mainly useful internally and for low-level code that already has a byte buffer.
inline std::unique_ptr<IMessage> makeCapnpMessageFromBytes(std::vector<uint8_t> bytes) {
    return std::make_unique<CapnpMessage>(std::move(bytes));
}

// Internal helper to create a builder-based message. This leaks Cap'n Proto types through the
// FillerFn type and should therefore *not* be used by high-level client/server code. It is kept
// as an internal utility so that the CapnpSerializer implementation can remain flexible.
inline std::unique_ptr<IMessage> makeCapnpBuilderMessage(CapnpBuilderMessage::FillerFn filler) {
    return std::make_unique<CapnpBuilderMessage>(std::move(filler));
}