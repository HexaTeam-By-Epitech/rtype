/*
** EPITECH PROJECT, 2025
** Created on 09/12/2025.
** File description:
** GameOver.hpp
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <cstdint>
#include <string>
#include <vector>
#include "schemas/s2c_messages.capnp.h"

namespace RType::Messages::S2C {

    /**
     * @class GameOver
     * @brief Game over notification
     * 
     * Sent when the game session terminates.
     */
    class GameOver {
       public:
        std::string reason;

        GameOver() = default;
        explicit GameOver(const std::string &r) : reason(r) {}

        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::GameOver>();
            builder.setReason(reason);

            auto bytes = capnp::messageToFlatArray(message);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        static GameOver deserialize(const std::vector<uint8_t> &data) {
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(data.data()),
                                                  data.size() / sizeof(capnp::word));

            capnp::FlatArrayMessageReader message(words);
            auto reader = message.getRoot<::GameOver>();

            return GameOver(reader.getReason().cStr());
        }
    };

}  // namespace RType::Messages::S2C