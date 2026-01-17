/*
** EPITECH PROJECT, 2025
** Created on 13/01/2026.
** File description:
** ChatMessage.hpp
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include "schemas/c2s_messages.capnp.h"

namespace RType::Messages::C2S {

    /**
     * @class C2SChatMessage
     * @brief Chat message sent from client to server
     * 
     * Contains the text message to be broadcast to other players in the room.
     * Messages starting with "/" are treated as commands and not broadcast.
     */
    class C2SChatMessage {
       public:
        std::string message;

        C2SChatMessage() = default;
        explicit C2SChatMessage(const std::string &msg) : message(msg) {}

        /**
         * @brief Serialize to byte vector
         */
        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder messageBuilder;
            auto builder = messageBuilder.initRoot<::C2SChatMessage>();

            builder.setMessage(message);

            auto bytes = capnp::messageToFlatArray(messageBuilder);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        /**
         * @brief Deserialize from byte vector
         */
        static C2SChatMessage deserialize(const std::vector<uint8_t> &data) {
            KJ_REQUIRE(data.size() % sizeof(capnp::word) == 0,
                       "Serialized data size must be a multiple of capnp::word");
            auto aligned = kj::heapArray<uint8_t>(data.size());
            memcpy(aligned.begin(), data.data(), data.size());
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(aligned.begin()),
                                                  data.size() / sizeof(capnp::word));

            capnp::FlatArrayMessageReader messageReader(words);
            auto reader = messageReader.getRoot<::C2SChatMessage>();

            C2SChatMessage result;
            result.message = reader.getMessage().cStr();

            return result;
        }
    };

}  // namespace RType::Messages::C2S
