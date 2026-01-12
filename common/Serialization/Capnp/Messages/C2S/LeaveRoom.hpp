/*
** EPITECH PROJECT, 2025
** LeaveRoom.hpp - Request to leave current room
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <vector>
#include "schemas/c2s_messages.capnp.h"

namespace RType::Messages::C2S {

    class LeaveRoom {
       public:
        LeaveRoom() = default;

        std::vector<uint8_t> serialize() const {
            ::capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::LeaveRoom>();

            kj::VectorOutputStream output;
            ::capnp::writeMessage(output, message);
            auto data = output.getArray();
            return std::vector<uint8_t>(data.begin(), data.end());
        }

        static LeaveRoom deserialize(const std::vector<uint8_t> &data) {
            if (data.empty()) {
                throw std::runtime_error("Cannot deserialize LeaveRoom from empty data");
            }

            kj::ArrayPtr<const ::capnp::word> words(reinterpret_cast<const ::capnp::word *>(data.data()),
                                                    data.size() / sizeof(::capnp::word));

            ::capnp::FlatArrayMessageReader reader(words);
            reader.getRoot<::LeaveRoom>();  // Validate

            return LeaveRoom();
        }
    };

}  // namespace RType::Messages::C2S
