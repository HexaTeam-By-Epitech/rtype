/*
** EPITECH PROJECT, 2025
** File description:
** CreateRoom.hpp - Create a new room
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <cstdint>
#include <string>
#include <vector>
#include "schemas/c2s_messages.capnp.h"

namespace RType::Messages::C2S {

    class CreateRoom {
       public:
        std::string roomName;
        uint32_t maxPlayers;
        bool isPrivate;

        CreateRoom() = default;
        CreateRoom(const std::string &name, uint32_t max, bool priv)
            : roomName(name), maxPlayers(max), isPrivate(priv) {}

        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::CreateRoom>();
            builder.setRoomName(roomName);
            builder.setMaxPlayers(maxPlayers);
            builder.setIsPrivate(isPrivate);

            auto bytes = capnp::messageToFlatArray(message);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        static CreateRoom deserialize(const std::vector<uint8_t> &data) {
            KJ_REQUIRE(data.size() % sizeof(capnp::word) == 0,
                       "Serialized data size must be a multiple of capnp::word");
            auto aligned = kj::heapArray<uint8_t>(data.size());
            memcpy(aligned.begin(), data.data(), data.size());
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(aligned.begin()),
                                                  aligned.size() / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(words);
            auto msg = reader.getRoot<::CreateRoom>();

            CreateRoom result;
            result.roomName = msg.getRoomName().cStr();
            result.maxPlayers = msg.getMaxPlayers();
            result.isPrivate = msg.getIsPrivate();
            return result;
        }
    };

}  // namespace RType::Messages::C2S
