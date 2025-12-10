/*
** EPITECH PROJECT, 2025
** Created on 09/12/2025.
** File description:
** JoinGame.hpp 
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <cstdint>
#include <string>
#include <vector>
#include "schemas/c2s_messages.capnp.h"

namespace RType::Messages::C2S {

    /**
     * @class JoinGame
     * @brief Join game request sent from client to server
     * 
     * Usage:
     *   JoinGame join("PlayerOne");
     *   auto bytes = join.serialize();
     */
    class JoinGame {
       public:
        std::string playerName;

        JoinGame() = default;
        explicit JoinGame(const std::string &name) : playerName(name) {}

        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::JoinGame>();
            builder.setPlayerName(playerName);

            auto bytes = capnp::messageToFlatArray(message);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        static JoinGame deserialize(const std::vector<uint8_t> &data) {
            // Ensure buffer is word-aligned for Cap'n Proto (undefined behavior if not)
            KJ_REQUIRE(data.size() % sizeof(capnp::word) == 0,
                       "Serialized data size must be a multiple of capnp::word");
            auto aligned = kj::heapArray<uint8_t>(data.size());
            memcpy(aligned.begin(), data.data(), data.size());
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(aligned.begin()),
                                                  data.size() / sizeof(capnp::word));

            capnp::FlatArrayMessageReader message(words);
            auto reader = message.getRoot<::JoinGame>();

            return JoinGame(reader.getPlayerName().cStr());
        }
    };

}  // namespace RType::Messages::C2S