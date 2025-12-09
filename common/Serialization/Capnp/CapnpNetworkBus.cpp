/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** CapnpNetworkBus.cpp
*/

#include "CapnpNetworkBus.hpp"

namespace CapnpHelpers {

    std::vector<uint8_t> serialize(capnp::MallocMessageBuilder &builder) {
        // Serialize to flat array of words
        kj::Array<capnp::word> words = capnp::messageToFlatArray(builder);

        // Convert to bytes
        auto bytes = words.asBytes();
        return std::vector<uint8_t>(bytes.begin(), bytes.end());
    }

    capnp::FlatArrayMessageReader deserialize(const std::vector<uint8_t> &data) {
        // Convert bytes to words (Cap'n Proto's unit)
        const capnp::word *wordPtr = reinterpret_cast<const capnp::word *>(data.data());
        size_t wordCount = data.size() / sizeof(capnp::word);

        kj::ArrayPtr<const capnp::word> words(wordPtr, wordCount);

        // Create reader (no copy, references data directly)
        return capnp::FlatArrayMessageReader(words);
    }

}  // namespace CapnpHelpers