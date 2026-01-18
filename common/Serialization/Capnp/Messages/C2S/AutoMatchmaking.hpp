/*
** EPITECH PROJECT, 2026
** rtype
** AutoMatchmaking.hpp - Request automatic matchmaking
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include "schemas/c2s_messages.capnp.h"

#include <vector>

namespace RType::Messages::C2S {
    class AutoMatchmaking {
       public:
        bool enabled;

        // Constructor
        AutoMatchmaking(bool en = true) : enabled(en) {}

        // Serialize to bytes
        std::vector<uint8_t> serialize() const {
            ::capnp::MallocMessageBuilder message;

            auto msg = message.initRoot<::AutoMatchmaking>();
            msg.setEnabled(enabled);

            // Serialize
            kj::VectorOutputStream stream;
            capnp::writeMessage(stream, message);
            auto array = stream.getArray();
            return std::vector<uint8_t>(array.begin(), array.end());
        }

        // Deserialize from bytes
        static AutoMatchmaking deserialize(const std::vector<uint8_t> &data) {
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(data.data()),
                                                  data.size() / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(words);

            auto msg = reader.getRoot<::AutoMatchmaking>();

            AutoMatchmaking result;
            result.enabled = msg.getEnabled();
            return result;
        }
    };
}  // namespace RType::Messages::C2S
