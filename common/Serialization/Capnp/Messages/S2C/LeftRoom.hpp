/*
** EPITECH PROJECT, 2025
** Created on 14/01/2026.
** File description:
** LeftRoom.hpp
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
     * @enum LeftRoomReason
     * @brief Reason why a player left a room
     */
    enum class LeftRoomReason : uint8_t {
        VOLUNTARY_LEAVE = 0,  // Player pressed "back" or left voluntarily
        KICKED = 1,           // Player was kicked by host
        ROOM_CLOSED = 2,      // Room was closed/deleted
        SERVER_SHUTDOWN = 3   // Server is shutting down
    };

    /**
     * @class LeftRoom
     * @brief Notification that a player has left a room
     * 
     * Sent from server to:
     * - The player who left (to update their UI)
     * - All other players in the room (to update their player list)
     */
    class LeftRoom {
       public:
        uint32_t playerId;
        LeftRoomReason reason;
        std::string message;

        LeftRoom() = default;
        LeftRoom(uint32_t pId, LeftRoomReason r, const std::string &msg = "")
            : playerId(pId), reason(r), message(msg) {}

        /**
         * @brief Serialize to byte vector
         */
        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder messageBuilder;
            auto builder = messageBuilder.initRoot<::LeftRoom>();

            builder.setPlayerId(playerId);

            // Convert enum to Cap'n Proto enum
            ::LeftRoomReason capnpReason;
            switch (reason) {
                case LeftRoomReason::VOLUNTARY_LEAVE:
                    capnpReason = ::LeftRoomReason::VOLUNTARY_LEAVE;
                    break;
                case LeftRoomReason::KICKED:
                    capnpReason = ::LeftRoomReason::KICKED;
                    break;
                case LeftRoomReason::ROOM_CLOSED:
                    capnpReason = ::LeftRoomReason::ROOM_CLOSED;
                    break;
                case LeftRoomReason::SERVER_SHUTDOWN:
                    capnpReason = ::LeftRoomReason::SERVER_SHUTDOWN;
                    break;
                default:
                    capnpReason = ::LeftRoomReason::VOLUNTARY_LEAVE;
                    break;
            }
            builder.setReason(capnpReason);
            builder.setMessage(message);

            auto bytes = capnp::messageToFlatArray(messageBuilder);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        /**
         * @brief Deserialize from byte vector
         */
        static LeftRoom deserialize(const std::vector<uint8_t> &data) {
            KJ_REQUIRE(data.size() % sizeof(capnp::word) == 0,
                       "Serialized data size must be a multiple of capnp::word");
            auto aligned = kj::heapArray<uint8_t>(data.size());
            memcpy(aligned.begin(), data.data(), data.size());
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(aligned.begin()),
                                                  data.size() / sizeof(capnp::word));

            capnp::FlatArrayMessageReader messageReader(words);
            auto reader = messageReader.getRoot<::LeftRoom>();

            LeftRoom result;
            result.playerId = reader.getPlayerId();

            // Convert Cap'n Proto enum to C++ enum
            auto capnpReason = reader.getReason();
            switch (capnpReason) {
                case ::LeftRoomReason::VOLUNTARY_LEAVE:
                    result.reason = LeftRoomReason::VOLUNTARY_LEAVE;
                    break;
                case ::LeftRoomReason::KICKED:
                    result.reason = LeftRoomReason::KICKED;
                    break;
                case ::LeftRoomReason::ROOM_CLOSED:
                    result.reason = LeftRoomReason::ROOM_CLOSED;
                    break;
                case ::LeftRoomReason::SERVER_SHUTDOWN:
                    result.reason = LeftRoomReason::SERVER_SHUTDOWN;
                    break;
            }

            result.message = reader.getMessage().cStr();

            return result;
        }
    };

}  // namespace RType::Messages::S2C
