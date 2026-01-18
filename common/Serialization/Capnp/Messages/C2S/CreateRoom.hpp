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
        float gameSpeedMultiplier;  ///< Game speed multiplier (0.25 to 1.0, default 1.0)

        CreateRoom() : maxPlayers(4), isPrivate(false), gameSpeedMultiplier(1.0f) {}
        CreateRoom(const std::string &name, uint32_t max, bool priv, float speedMultiplier = 1.0f)
            : roomName(name), maxPlayers(max), isPrivate(priv), gameSpeedMultiplier(speedMultiplier) {}

        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::CreateRoom>();
            builder.setRoomName(roomName);
            builder.setMaxPlayers(maxPlayers);
            builder.setIsPrivate(isPrivate);
            builder.setGameSpeedMultiplier(gameSpeedMultiplier);

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
            result.gameSpeedMultiplier = msg.getGameSpeedMultiplier();
            // Clamp to valid range and default if not set (0.0f means unset in capnp)
            if (result.gameSpeedMultiplier <= 0.0f || result.gameSpeedMultiplier > 1.0f) {
                result.gameSpeedMultiplier = 1.0f;
            }
            return result;
        }
    };

}  // namespace RType::Messages::C2S
