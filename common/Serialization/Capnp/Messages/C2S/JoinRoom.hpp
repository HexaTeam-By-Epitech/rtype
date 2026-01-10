/*
** EPITECH PROJECT, 2025
** File description:
** JoinRoom.hpp - Join a specific room
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <vector>
#include "schemas/c2s_messages.capnp.h"

namespace RType::Messages::C2S {

    class JoinRoom {
       public:
        std::string roomId;

        JoinRoom() = default;
        explicit JoinRoom(const std::string &id) : roomId(id) {}

        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::JoinRoom>();
            builder.setRoomId(roomId);

            auto bytes = capnp::messageToFlatArray(message);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        static JoinRoom deserialize(const std::vector<uint8_t> &data) {
            KJ_REQUIRE(data.size() % sizeof(capnp::word) == 0,
                       "Serialized data size must be a multiple of capnp::word");
            auto aligned = kj::heapArray<uint8_t>(data.size());
            memcpy(aligned.begin(), data.data(), data.size());
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(aligned.begin()),
                                                  aligned.size() / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(words);
            auto msg = reader.getRoot<::JoinRoom>();

            return JoinRoom(msg.getRoomId().cStr());
        }
    };

}  // namespace RType::Messages::C2S
