/*
** EPITECH PROJECT, 2025
** File description:
** JoinedRoom.hpp - Room join response
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <vector>
#include "schemas/s2c_messages.capnp.h"

namespace RType::Messages::S2C {

    class JoinedRoom {
       public:
        std::string roomId;
        bool success;
        std::string errorMessage;
        bool isSpectator{false};

        JoinedRoom() = default;
        JoinedRoom(const std::string &id, bool succ, const std::string &err = "", bool spectator = false)
            : roomId(id), success(succ), errorMessage(err), isSpectator(spectator) {}

        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::JoinedRoom>();
            builder.setRoomId(roomId);
            builder.setSuccess(success);
            builder.setErrorMessage(errorMessage);
            builder.setIsSpectator(isSpectator);

            auto bytes = capnp::messageToFlatArray(message);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        static JoinedRoom deserialize(const std::vector<uint8_t> &data) {
            KJ_REQUIRE(data.size() % sizeof(capnp::word) == 0,
                       "Serialized data size must be a multiple of capnp::word");
            auto aligned = kj::heapArray<uint8_t>(data.size());
            memcpy(aligned.begin(), data.data(), data.size());
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(aligned.begin()),
                                                  aligned.size() / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(words);
            auto msg = reader.getRoot<::JoinedRoom>();

            JoinedRoom result;
            result.roomId = msg.getRoomId().cStr();
            result.success = msg.getSuccess();
            result.errorMessage = msg.getErrorMessage().cStr();
            result.isSpectator = msg.getIsSpectator();
            return result;
        }
    };

}  // namespace RType::Messages::S2C
