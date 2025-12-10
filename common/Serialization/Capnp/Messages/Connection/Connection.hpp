/*
** EPITECH PROJECT, 2025
** Created on 09/12/2025.
** File description:
** Connection.hpp
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <cstdint>
#include <string>
#include <vector>
#include "schemas/connection.capnp.h"

namespace RType::Messages::Connection {

    /**
     * @class HandshakeRequest
     * @brief Client initiates connection with server
     */
    class HandshakeRequest {
       public:
        std::string _clientVersion;
        std::string _playerName;
        uint64_t _timestamp;

        HandshakeRequest() : _timestamp(0) {}

        HandshakeRequest(const std::string &version, const std::string &name, uint64_t ts)
            : _clientVersion(version), _playerName(name), _timestamp(ts) {}

        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::HandshakeRequest>();
            builder.setClientVersion(_clientVersion);
            builder.setPlayerName(_playerName);
            builder.setTimestamp(_timestamp);

            auto bytes = capnp::messageToFlatArray(message);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        static HandshakeRequest deserialize(const std::vector<uint8_t> &data) {
            // Ensure buffer is word-aligned for Cap'n Proto (undefined behavior if not)
            KJ_REQUIRE(data.size() % sizeof(capnp::word) == 0,
                       "Serialized data size must be a multiple of capnp::word");
            auto aligned = kj::heapArray<uint8_t>(data.size());
            memcpy(aligned.begin(), data.data(), data.size());
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(aligned.begin()),
                                                  data.size() / sizeof(capnp::word));
            capnp::FlatArrayMessageReader message(words);
            auto reader = message.getRoot<::HandshakeRequest>();

            return HandshakeRequest(reader.getClientVersion().cStr(), reader.getPlayerName().cStr(),
                                    reader.getTimestamp());
        }
    };

    /**
     * @class HandshakeResponse
     * @brief Server responds to handshake
     */
    class HandshakeResponse {
       public:
        bool accepted{false};
        std::string sessionId;
        std::string serverId;
        std::string message;
        std::string serverVersion;

        HandshakeResponse() = default;

        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder msg;
            auto builder = msg.initRoot<::HandshakeResponse>();
            builder.setAccepted(accepted);
            builder.setSessionId(sessionId);
            builder.setServerId(serverId);
            builder.setMessage(message);
            builder.setServerVersion(serverVersion);

            auto bytes = capnp::messageToFlatArray(msg);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        static HandshakeResponse deserialize(const std::vector<uint8_t> &data) {
            // Ensure buffer is word-aligned for Cap'n Proto (undefined behavior if not)
            KJ_REQUIRE(data.size() % sizeof(capnp::word) == 0,
                       "Serialized data size must be a multiple of capnp::word");
            auto aligned = kj::heapArray<uint8_t>(data.size());
            memcpy(aligned.begin(), data.data(), data.size());
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(aligned.begin()),
                                                  data.size() / sizeof(capnp::word));
            capnp::FlatArrayMessageReader msg(words);
            auto reader = msg.getRoot<::HandshakeResponse>();

            HandshakeResponse result;
            result.accepted = reader.getAccepted();
            result.sessionId = reader.getSessionId().cStr();
            result.serverId = reader.getServerId().cStr();
            result.message = reader.getMessage().cStr();
            result.serverVersion = reader.getServerVersion().cStr();
            return result;
        }
    };

    /**
     * @class PingMessage
     * @brief Client or server sends ping
     */
    class PingMessage {
       public:
        uint64_t timestamp;
        uint32_t sequenceNumber;

        PingMessage() : timestamp(0), sequenceNumber(0) {}
        PingMessage(uint64_t ts, uint32_t seq) : timestamp(ts), sequenceNumber(seq) {}

        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::PingMessage>();
            builder.setTimestamp(timestamp);
            builder.setSequenceNumber(sequenceNumber);

            auto bytes = capnp::messageToFlatArray(message);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        static PingMessage deserialize(const std::vector<uint8_t> &data) {
            // Ensure buffer is word-aligned for Cap'n Proto (undefined behavior if not)
            KJ_REQUIRE(data.size() % sizeof(capnp::word) == 0,
                       "Serialized data size must be a multiple of capnp::word");
            auto aligned = kj::heapArray<uint8_t>(data.size());
            memcpy(aligned.begin(), data.data(), data.size());
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(aligned.begin()),
                                                  data.size() / sizeof(capnp::word));
            capnp::FlatArrayMessageReader message(words);
            auto reader = message.getRoot<::PingMessage>();

            return PingMessage(reader.getTimestamp(), reader.getSequenceNumber());
        }
    };

    /**
     * @class PongMessage
     * @brief Response to ping
     */
    class PongMessage {
       public:
        uint64_t clientTimestamp;
        uint64_t serverTimestamp;
        uint32_t sequenceNumber;

        PongMessage() : clientTimestamp(0), serverTimestamp(0), sequenceNumber(0) {}

        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::PongMessage>();
            builder.setClientTimestamp(clientTimestamp);
            builder.setServerTimestamp(serverTimestamp);
            builder.setSequenceNumber(sequenceNumber);

            auto bytes = capnp::messageToFlatArray(message);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        static PongMessage deserialize(const std::vector<uint8_t> &data) {
            // Ensure buffer is word-aligned for Cap'n Proto (undefined behavior if not)
            KJ_REQUIRE(data.size() % sizeof(capnp::word) == 0,
                       "Serialized data size must be a multiple of capnp::word");
            auto aligned = kj::heapArray<uint8_t>(data.size());
            memcpy(aligned.begin(), data.data(), data.size());
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(aligned.begin()),
                                                  data.size() / sizeof(capnp::word));
            capnp::FlatArrayMessageReader message(words);
            auto reader = message.getRoot<::PongMessage>();

            PongMessage result;
            result.clientTimestamp = reader.getClientTimestamp();
            result.serverTimestamp = reader.getServerTimestamp();
            result.sequenceNumber = reader.getSequenceNumber();
            return result;
        }
    };

}  // namespace RType::Messages::Connection
