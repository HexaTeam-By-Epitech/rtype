/*
** EPITECH PROJECT, 2025
** RoomState.hpp - Room state with player list
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <vector>
#include "schemas/s2c_messages.capnp.h"

namespace RType::Messages::S2C {

    struct PlayerData {
        uint32_t playerId;
        std::string playerName;
        bool isHost;
        bool isSpectator;

        PlayerData() = default;
        PlayerData(uint32_t id, const std::string &name, bool host, bool spectator)
            : playerId(id), playerName(name), isHost(host), isSpectator(spectator) {}
    };

    class RoomState {
       public:
        std::string roomId;
        std::string roomName;
        uint32_t currentPlayers;
        uint32_t maxPlayers;
        std::vector<PlayerData> players;
        uint8_t state;

        RoomState() = default;

        std::vector<uint8_t> serialize() const {
            ::capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::RoomState>();

            builder.setRoomId(roomId);
            builder.setRoomName(roomName);
            builder.setCurrentPlayers(currentPlayers);
            builder.setMaxPlayers(maxPlayers);
            builder.setState(state);

            auto playersList = builder.initPlayers(players.size());
            for (size_t i = 0; i < players.size(); ++i) {
                auto playerBuilder = playersList[i];
                playerBuilder.setPlayerId(players[i].playerId);
                playerBuilder.setPlayerName(players[i].playerName);
                playerBuilder.setIsHost(players[i].isHost);
                playerBuilder.setIsSpectator(players[i].isSpectator);
            }

            kj::VectorOutputStream output;
            ::capnp::writeMessage(output, message);
            auto data = output.getArray();
            return std::vector<uint8_t>(data.begin(), data.end());
        }

        static RoomState deserialize(const std::vector<uint8_t> &data) {
            if (data.empty()) {
                throw std::runtime_error("Cannot deserialize RoomState from empty data");
            }

            kj::ArrayPtr<const ::capnp::word> words(reinterpret_cast<const ::capnp::word *>(data.data()),
                                                    data.size() / sizeof(::capnp::word));

            ::capnp::FlatArrayMessageReader reader(words);
            auto msg = reader.getRoot<::RoomState>();

            RoomState result;
            result.roomId = msg.getRoomId().cStr();
            result.roomName = msg.getRoomName().cStr();
            result.currentPlayers = msg.getCurrentPlayers();
            result.maxPlayers = msg.getMaxPlayers();
            result.state = msg.getState();

            auto playersList = msg.getPlayers();
            result.players.reserve(playersList.size());
            for (auto playerMsg : playersList) {
                PlayerData player;
                player.playerId = playerMsg.getPlayerId();
                player.playerName = playerMsg.getPlayerName().cStr();
                player.isHost = playerMsg.getIsHost();
                player.isSpectator = playerMsg.getIsSpectator();
                result.players.push_back(player);
            }

            return result;
        }
    };

}  // namespace RType::Messages::S2C
