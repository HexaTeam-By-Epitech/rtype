/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** ConnectionMessages.hpp
*/

#pragma once

#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief Helper functions for connection protocol messages
 * 
 * Handles handshake, disconnect, kick, and ping/pong messages.
 * Format: Simple binary [length:4][data:N].
 */
namespace ConnectionMessages {

    // ============================================================================
    // HANDSHAKE
    // ============================================================================

    struct HandshakeRequestData {
        std::string clientVersion;
        std::string playerName;
        uint64_t timestamp;
    };

    struct HandshakeResponseData {
        bool accepted;
        std::string sessionId;
        std::string serverId;
        std::string message;
        std::string serverVersion;
    };

    inline std::vector<uint8_t> createHandshakeRequest(const HandshakeRequestData &data) {
        // Format: [4:version_len][version][4:name_len][name][8:timestamp]
        std::vector<uint8_t> packet;

        // Client version
        uint32_t versionLen = static_cast<uint32_t>(data.clientVersion.size());
        packet.push_back(static_cast<uint8_t>(versionLen & 0xFF));
        packet.push_back(static_cast<uint8_t>((versionLen >> 8) & 0xFF));
        packet.push_back(static_cast<uint8_t>((versionLen >> 16) & 0xFF));
        packet.push_back(static_cast<uint8_t>((versionLen >> 24) & 0xFF));
        packet.insert(packet.end(), data.clientVersion.begin(), data.clientVersion.end());

        // Player name
        uint32_t nameLen = static_cast<uint32_t>(data.playerName.size());
        packet.push_back(static_cast<uint8_t>(nameLen & 0xFF));
        packet.push_back(static_cast<uint8_t>((nameLen >> 8) & 0xFF));
        packet.push_back(static_cast<uint8_t>((nameLen >> 16) & 0xFF));
        packet.push_back(static_cast<uint8_t>((nameLen >> 24) & 0xFF));
        packet.insert(packet.end(), data.playerName.begin(), data.playerName.end());

        // Timestamp
        for (int i = 0; i < 8; ++i) {
            packet.push_back(static_cast<uint8_t>((data.timestamp >> (i * 8)) & 0xFF));
        }

        return packet;
    }

    inline HandshakeRequestData parseHandshakeRequest(const std::vector<uint8_t> &data) {
        HandshakeRequestData result;
        size_t offset = 0;

        if (data.size() < 4)
            return result;

        // Read version
        uint32_t versionLen =
            data[offset] | (data[offset + 1] << 8) | (data[offset + 2] << 16) | (data[offset + 3] << 24);
        offset += 4;
        if (data.size() < offset + versionLen)
            return result;
        result.clientVersion = std::string(data.begin() + offset, data.begin() + offset + versionLen);
        offset += versionLen;

        // Read name
        if (data.size() < offset + 4)
            return result;
        uint32_t nameLen =
            data[offset] | (data[offset + 1] << 8) | (data[offset + 2] << 16) | (data[offset + 3] << 24);
        offset += 4;
        if (data.size() < offset + nameLen)
            return result;
        result.playerName = std::string(data.begin() + offset, data.begin() + offset + nameLen);
        offset += nameLen;

        // Read timestamp
        if (data.size() < offset + 8)
            return result;
        result.timestamp = 0;
        for (int i = 0; i < 8; ++i) {
            result.timestamp |= static_cast<uint64_t>(data[offset + i]) << (i * 8);
        }

        return result;
    }

    inline std::vector<uint8_t> createHandshakeResponse(const HandshakeResponseData &data) {
        std::vector<uint8_t> packet;

        // Accepted flag
        packet.push_back(data.accepted ? 1 : 0);

        // Session ID
        uint32_t sessionLen = static_cast<uint32_t>(data.sessionId.size());
        packet.push_back(static_cast<uint8_t>(sessionLen & 0xFF));
        packet.push_back(static_cast<uint8_t>((sessionLen >> 8) & 0xFF));
        packet.push_back(static_cast<uint8_t>((sessionLen >> 16) & 0xFF));
        packet.push_back(static_cast<uint8_t>((sessionLen >> 24) & 0xFF));
        packet.insert(packet.end(), data.sessionId.begin(), data.sessionId.end());

        // Server ID
        uint32_t serverLen = static_cast<uint32_t>(data.serverId.size());
        packet.push_back(static_cast<uint8_t>(serverLen & 0xFF));
        packet.push_back(static_cast<uint8_t>((serverLen >> 8) & 0xFF));
        packet.push_back(static_cast<uint8_t>((serverLen >> 16) & 0xFF));
        packet.push_back(static_cast<uint8_t>((serverLen >> 24) & 0xFF));
        packet.insert(packet.end(), data.serverId.begin(), data.serverId.end());

        // Message
        uint32_t msgLen = static_cast<uint32_t>(data.message.size());
        packet.push_back(static_cast<uint8_t>(msgLen & 0xFF));
        packet.push_back(static_cast<uint8_t>((msgLen >> 8) & 0xFF));
        packet.push_back(static_cast<uint8_t>((msgLen >> 16) & 0xFF));
        packet.push_back(static_cast<uint8_t>((msgLen >> 24) & 0xFF));
        packet.insert(packet.end(), data.message.begin(), data.message.end());

        // Server version
        uint32_t versionLen = static_cast<uint32_t>(data.serverVersion.size());
        packet.push_back(static_cast<uint8_t>(versionLen & 0xFF));
        packet.push_back(static_cast<uint8_t>((versionLen >> 8) & 0xFF));
        packet.push_back(static_cast<uint8_t>((versionLen >> 16) & 0xFF));
        packet.push_back(static_cast<uint8_t>((versionLen >> 24) & 0xFF));
        packet.insert(packet.end(), data.serverVersion.begin(), data.serverVersion.end());

        return packet;
    }

    // ============================================================================
    // DISCONNECT
    // ============================================================================

    enum class DisconnectReason : uint8_t {
        CLIENT_REQUEST = 0,
        SERVER_SHUTDOWN = 1,
        TIMEOUT = 2,
        KICKED = 3,
        ERROR = 4
    };

    struct DisconnectData {
        DisconnectReason reason;
        std::string message;
        uint64_t timestamp;
    };

    inline std::vector<uint8_t> createDisconnect(const DisconnectData &data) {
        std::vector<uint8_t> packet;

        // Reason
        packet.push_back(static_cast<uint8_t>(data.reason));

        // Message
        uint32_t msgLen = static_cast<uint32_t>(data.message.size());
        packet.push_back(static_cast<uint8_t>(msgLen & 0xFF));
        packet.push_back(static_cast<uint8_t>((msgLen >> 8) & 0xFF));
        packet.push_back(static_cast<uint8_t>((msgLen >> 16) & 0xFF));
        packet.push_back(static_cast<uint8_t>((msgLen >> 24) & 0xFF));
        packet.insert(packet.end(), data.message.begin(), data.message.end());

        // Timestamp
        for (int i = 0; i < 8; ++i) {
            packet.push_back(static_cast<uint8_t>((data.timestamp >> (i * 8)) & 0xFF));
        }

        return packet;
    }

    // ============================================================================
    // KICK
    // ============================================================================

    struct KickData {
        std::string reason;
        uint64_t duration;  // 0 = permanent
        uint64_t timestamp;
    };

    inline std::vector<uint8_t> createKick(const KickData &data) {
        std::vector<uint8_t> packet;

        // Reason
        uint32_t reasonLen = static_cast<uint32_t>(data.reason.size());
        packet.push_back(static_cast<uint8_t>(reasonLen & 0xFF));
        packet.push_back(static_cast<uint8_t>((reasonLen >> 8) & 0xFF));
        packet.push_back(static_cast<uint8_t>((reasonLen >> 16) & 0xFF));
        packet.push_back(static_cast<uint8_t>((reasonLen >> 24) & 0xFF));
        packet.insert(packet.end(), data.reason.begin(), data.reason.end());

        // Duration
        for (int i = 0; i < 8; ++i) {
            packet.push_back(static_cast<uint8_t>((data.duration >> (i * 8)) & 0xFF));
        }

        // Timestamp
        for (int i = 0; i < 8; ++i) {
            packet.push_back(static_cast<uint8_t>((data.timestamp >> (i * 8)) & 0xFF));
        }

        return packet;
    }

    // ============================================================================
    // PING/PONG
    // ============================================================================

    struct PingData {
        uint64_t timestamp;
        uint32_t sequenceNumber;
    };

    inline std::vector<uint8_t> createPing(const PingData &data) {
        std::vector<uint8_t> packet;

        // Timestamp
        for (int i = 0; i < 8; ++i) {
            packet.push_back(static_cast<uint8_t>((data.timestamp >> (i * 8)) & 0xFF));
        }

        // Sequence number
        for (int i = 0; i < 4; ++i) {
            packet.push_back(static_cast<uint8_t>((data.sequenceNumber >> (i * 8)) & 0xFF));
        }

        return packet;
    }

}  // namespace ConnectionMessages