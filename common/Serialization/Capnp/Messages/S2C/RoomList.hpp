/*
** EPITECH PROJECT, 2025
** File description:
** RoomList.hpp - List of available rooms
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <cstdint>
#include <string>
#include <vector>
#include "schemas/s2c_messages.capnp.h"

namespace RType::Messages::S2C {

    struct RoomInfoData {
        std::string roomId;
        std::string roomName;
        uint32_t playerCount;
        uint32_t maxPlayers;
        bool isPrivate;
        uint8_t state;  // 0=WAITING, 1=STARTING, 2=IN_PROGRESS, 3=FINISHED
    };

    class RoomList {
       public:
        std::vector<RoomInfoData> rooms;

        RoomList() = default;

        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::RoomList>();
            auto roomsBuilder = builder.initRooms(rooms.size());

            for (size_t i = 0; i < rooms.size(); ++i) {
                auto roomBuilder = roomsBuilder[i];
                roomBuilder.setRoomId(rooms[i].roomId);
                roomBuilder.setRoomName(rooms[i].roomName);
                roomBuilder.setPlayerCount(rooms[i].playerCount);
                roomBuilder.setMaxPlayers(rooms[i].maxPlayers);
                roomBuilder.setIsPrivate(rooms[i].isPrivate);
                roomBuilder.setState(rooms[i].state);
            }

            auto bytes = capnp::messageToFlatArray(message);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        static RoomList deserialize(const std::vector<uint8_t> &data) {
            KJ_REQUIRE(data.size() % sizeof(capnp::word) == 0,
                       "Serialized data size must be a multiple of capnp::word");
            auto aligned = kj::heapArray<uint8_t>(data.size());
            memcpy(aligned.begin(), data.data(), data.size());
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(aligned.begin()),
                                                  aligned.size() / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(words);
            auto msg = reader.getRoot<::RoomList>();

            RoomList result;
            auto roomsList = msg.getRooms();
            for (auto room : roomsList) {
                RoomInfoData info;
                info.roomId = room.getRoomId().cStr();
                info.roomName = room.getRoomName().cStr();
                info.playerCount = room.getPlayerCount();
                info.maxPlayers = room.getMaxPlayers();
                info.isPrivate = room.getIsPrivate();
                info.state = room.getState();
                result.rooms.push_back(info);
            }
            return result;
        }
    };

}  // namespace RType::Messages::S2C
