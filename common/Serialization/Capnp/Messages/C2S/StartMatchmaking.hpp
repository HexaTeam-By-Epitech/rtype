/*
** EPITECH PROJECT, 2025
** File description:
** StartMatchmaking.hpp - Start matchmaking
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <vector>
#include "schemas/c2s_messages.capnp.h"

namespace RType::Messages::C2S {

    class StartMatchmaking {
       public:
        StartMatchmaking() = default;

        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            message.initRoot<::StartMatchmaking>();

            auto bytes = capnp::messageToFlatArray(message);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        static StartMatchmaking deserialize(const std::vector<uint8_t> &data) { return StartMatchmaking(); }
    };

}  // namespace RType::Messages::C2S
