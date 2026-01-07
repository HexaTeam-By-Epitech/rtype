/*
** EPITECH PROJECT, 2025
** File description:
** ListRooms.hpp - Request list of available rooms
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <vector>
#include "schemas/c2s_messages.capnp.h"

namespace RType::Messages::C2S {

    /**
     * @class ListRooms
     * @brief Request list of available rooms
     */
    class ListRooms {
       public:
        ListRooms() = default;

        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            message.initRoot<::ListRooms>();

            auto bytes = capnp::messageToFlatArray(message);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        static ListRooms deserialize(const std::vector<uint8_t> &data) { return ListRooms(); }
    };

}  // namespace RType::Messages::C2S
