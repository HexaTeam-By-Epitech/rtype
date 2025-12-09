/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** NetworkMessages.hpp
*/

#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @brief Network protocol with unified message format
 * 
 * Protocol format:
 * [2 bytes: MessageType][4 bytes: payload_length][N bytes: payload]
 * 
 * This is modular, secure, and scalable:
 * - Type checking: Every message has a type identifier
 * - Validation: Size checks prevent buffer overflows
 * - Extensible: Add new message types without changing the protocol
 * 
 * Example:
 *   auto packet = createMessage(MessageType::CONNECT_REQUEST, playerName);
 *   auto type = getMessageType(packet);
 *   auto payload = getPayload(packet);
 */
namespace NetworkMessages {

    /**
     * @enum MessageType
     * @brief All message types in the R-Type protocol
     * 
     * Add new types here as you need them. Each type is 2 bytes (uint16_t).
     */
    enum class MessageType : uint16_t {
        // Connection messages (0x00xx)
        CONNECT_REQUEST = 0x0001,
        CONNECT_RESPONSE = 0x0002,
        DISCONNECT = 0x0003,
        KICK = 0x0004,
        PING = 0x0005,
        PONG = 0x0006,

        // Gameplay messages (0x01xx)
        SPAWN_ENTITY = 0x0100,
        DESTROY_ENTITY = 0x0101,
        UPDATE_ENTITY = 0x0102,
        WORLD_STATE = 0x0103,

        // Player messages (0x02xx)
        PLAYER_INPUT = 0x0200,
        PLAYER_STATE = 0x0201,
        PLAYER_JOINED = 0x0202,
        PLAYER_LEFT = 0x0203,

        UNKNOWN = 0xFFFF
    };

    // ============================================================================
    // LOW-LEVEL PROTOCOL FUNCTIONS (Generic)
    // ============================================================================

    /**
     * @brief Create a message with type and payload
     * @param type Message type identifier
     * @param payload Message data
     * @return Complete packet: [type:2][length:4][payload:N]
     */
    inline std::vector<uint8_t> createMessage(MessageType type, const std::vector<uint8_t> &payload) {
        std::vector<uint8_t> packet;

        // Write message type (2 bytes, little endian)
        uint16_t typeValue = static_cast<uint16_t>(type);
        packet.push_back(static_cast<uint8_t>(typeValue & 0xFF));
        packet.push_back(static_cast<uint8_t>((typeValue >> 8) & 0xFF));

        // Write payload length (4 bytes, little endian)
        uint32_t length = static_cast<uint32_t>(payload.size());
        packet.push_back(static_cast<uint8_t>(length & 0xFF));
        packet.push_back(static_cast<uint8_t>((length >> 8) & 0xFF));
        packet.push_back(static_cast<uint8_t>((length >> 16) & 0xFF));
        packet.push_back(static_cast<uint8_t>((length >> 24) & 0xFF));

        // Write payload
        packet.insert(packet.end(), payload.begin(), payload.end());

        return packet;
    }

    /**
     * @brief Get message type from packet
     * @param packet Complete packet with header
     * @return Message type
     */
    inline MessageType getMessageType(const std::vector<uint8_t> &packet) {
        if (packet.size() < 6) {
            // Minimum: 2 (type) + 4 (length)
            return MessageType::UNKNOWN;
        }

        uint16_t typeValue = packet[0] | (packet[1] << 8);
        return static_cast<MessageType>(typeValue);
    }

    /**
     * @brief Get payload from packet (without header)
     * @param packet Complete packet with header
     * @return Payload data (or empty if invalid)
     */
    inline std::vector<uint8_t> getPayload(const std::vector<uint8_t> &packet) {
        if (packet.size() < 6) {
            return {};
        }

        // Read payload length
        uint32_t length = packet[2] | (packet[3] << 8) | (packet[4] << 16) | (packet[5] << 24);

        // Validate length is reasonable (max 10MB to prevent DoS attacks)
        constexpr uint32_t MAX_PAYLOAD_SIZE = 10 * 1024 * 1024;
        if (length > MAX_PAYLOAD_SIZE) {
            return {};
        }

        // Validate the declared length doesn't exceed actual packet size
        if (packet.size() < 6 + length) {
            return {};
        }

        // Additional sanity check: ensure no integer overflow when adding 6 + length
        if (length > packet.size() - 6) {
            return {};
        }

        return std::vector<uint8_t>(packet.begin() + 6, packet.begin() + 6 + length);
    }

    // ============================================================================
    // HIGH-LEVEL HELPER FUNCTIONS (Specific messages)
    // ============================================================================

    /**
     * @brief Serialize a string to bytes
     */
    inline std::vector<uint8_t> serializeString(const std::string &str) {
        std::vector<uint8_t> bytes;
        uint32_t length = static_cast<uint32_t>(str.size());

        // Write length
        bytes.push_back(static_cast<uint8_t>(length & 0xFF));
        bytes.push_back(static_cast<uint8_t>((length >> 8) & 0xFF));
        bytes.push_back(static_cast<uint8_t>((length >> 16) & 0xFF));
        bytes.push_back(static_cast<uint8_t>((length >> 24) & 0xFF));

        // Write string data
        bytes.insert(bytes.end(), str.begin(), str.end());

        return bytes;
    }

    /**
     * @brief Deserialize bytes to string
     */
    inline std::string deserializeString(const std::vector<uint8_t> &bytes, size_t &offset) {
        if (bytes.size() < offset + 4) {
            return "";
        }

        // Read length
        uint32_t length =
            bytes[offset] | (bytes[offset + 1] << 8) | (bytes[offset + 2] << 16) | (bytes[offset + 3] << 24);

        // Validate length is reasonable (max 1MB for strings to prevent DoS)
        constexpr uint32_t MAX_STRING_SIZE = 1024 * 1024;
        if (length > MAX_STRING_SIZE) {
            return "";
        }

        offset += 4;

        // Validate the declared length doesn't exceed actual buffer size
        if (bytes.size() < offset + length) {
            return "";
        }

        // Additional sanity check: ensure no integer overflow
        if (length > bytes.size() - offset) {
            return "";
        }

        std::string result(bytes.begin() + offset, bytes.begin() + offset + length);
        offset += length;

        return result;
    }

    // ============================================================================
    // CONNECTION MESSAGES
    // ============================================================================

    /**
     * @brief Create CONNECT_REQUEST message
     * @param playerName Player's name
     */
    inline std::vector<uint8_t> createConnectRequest(const std::string &playerName) {
        auto payload = serializeString(playerName);
        return createMessage(MessageType::CONNECT_REQUEST, payload);
    }

    /**
     * @brief Parse CONNECT_REQUEST message
     * @param packet Complete packet
     * @return Player name (empty if invalid or wrong type)
     */
    inline std::string parseConnectRequest(const std::vector<uint8_t> &packet) {
        if (getMessageType(packet) != MessageType::CONNECT_REQUEST) {
            return "";
        }

        auto payload = getPayload(packet);
        size_t offset = 0;
        return deserializeString(payload, offset);
    }

    /**
     * @brief Create CONNECT_RESPONSE message
     * @param message Welcome message
     */
    inline std::vector<uint8_t> createConnectResponse(const std::string &message) {
        auto payload = serializeString(message);
        return createMessage(MessageType::CONNECT_RESPONSE, payload);
    }

    /**
     * @brief Parse CONNECT_RESPONSE message
     * @param packet Complete packet
     * @return Welcome message (empty if invalid or wrong type)
     */
    inline std::string parseConnectResponse(const std::vector<uint8_t> &packet) {
        if (getMessageType(packet) != MessageType::CONNECT_RESPONSE) {
            return "";
        }

        auto payload = getPayload(packet);
        size_t offset = 0;
        return deserializeString(payload, offset);
    }

}  // namespace NetworkMessages