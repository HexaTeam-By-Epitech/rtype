/*
** EPITECH PROJECT, 2025
** Created on 13/01/2026.
** File description:
** ChatMessage.hpp
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include "schemas/s2c_messages.capnp.h"

namespace RType::Messages::S2C {

    /**
     * @class S2CChatMessage
     * @brief Chat message sent from server to clients
     * 
     * Contains the chat message with sender information to be displayed to players.
     */
    class S2CChatMessage {
       public:
        uint32_t playerId;
        std::string playerName;
        std::string message;
        uint64_t timestamp;

        S2CChatMessage() = default;
        S2CChatMessage(uint32_t pId, const std::string &pName, const std::string &msg, uint64_t ts)
            : playerId(pId), playerName(pName), message(msg), timestamp(ts) {}

        /**
         * @brief Serialize to byte vector
         */
        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder messageBuilder;
            auto builder = messageBuilder.initRoot<::S2CChatMessage>();

            builder.setPlayerId(playerId);
            builder.setPlayerName(playerName);
            builder.setMessage(message);
            builder.setTimestamp(timestamp);

            auto bytes = capnp::messageToFlatArray(messageBuilder);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        /**
         * @brief Deserialize from byte vector
         */
        static S2CChatMessage deserialize(const std::vector<uint8_t> &data) {
            KJ_REQUIRE(data.size() % sizeof(capnp::word) == 0,
                       "Serialized data size must be a multiple of capnp::word");
            auto aligned = kj::heapArray<uint8_t>(data.size());
            memcpy(aligned.begin(), data.data(), data.size());
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(aligned.begin()),
                                                  data.size() / sizeof(capnp::word));

            capnp::FlatArrayMessageReader messageReader(words);
            auto reader = messageReader.getRoot<::S2CChatMessage>();

            S2CChatMessage result;
            result.playerId = reader.getPlayerId();
            result.playerName = reader.getPlayerName().cStr();
            result.message = reader.getMessage().cStr();
            result.timestamp = reader.getTimestamp();

            return result;
        }
    };

}  // namespace RType::Messages::S2C
