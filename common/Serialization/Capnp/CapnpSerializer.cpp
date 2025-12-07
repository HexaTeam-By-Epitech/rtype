/*
** EPITECH PROJECT, 2025
** Created by mael on 02/12/2025.
** File description:
** CapnpSerializer.cpp
*/

#include "CapnpSerializer.hpp"
#include "CapnpBuilderMessage.hpp"
#include "CapnpUtils.hpp"
#include <capnp/message.h>
#include <kj/io.h>
#include <stdexcept>

CapnpSerializer::CapnpSerializer() = default;
CapnpSerializer::~CapnpSerializer() = default;

std::vector<uint8_t> CapnpSerializer::encode(const IMessage &message) {
    // If the message is already a CapnpMessage, just return the stored bytes.
    if (const auto capnpMsg = dynamic_cast<const CapnpMessage *>(&message)) {
        return capnpMsg->data();
    }

    // If the message is a builder wrapper, invoke its filler and serialize
    if (const auto builderMsg = dynamic_cast<const CapnpBuilderMessage *>(&message)) {
        capnp::MallocMessageBuilder builder;
        // call the filler to populate the builder
        builderMsg->filler()(builder);
        return serializeCapnpMessage(builder);
    }

    // Otherwise, we don't know how to convert the IMessage to a Cap'n Proto bytes blob
    throw std::invalid_argument(
        "CapnpSerializer::encode expects a CapnpMessage or CapnpBuilderMessage instance");
}

std::unique_ptr<IMessage> CapnpSerializer::decode(const std::vector<uint8_t> &data) {
    // Wrap raw bytes into a CapnpMessage instance. Consumers can then interpret them with the
    // appropriate generated schema reader (capnp::FlatArrayMessageReader) externally.
    return std::make_unique<CapnpMessage>(data);
}