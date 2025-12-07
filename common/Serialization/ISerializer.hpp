/*
** EPITECH PROJECT, 2025
** Created by mael on 02/12/2025.
** File description:
** ISerializer.hpp
*/

#pragma once

#include <vector>
#include <memory>

/**
 * @class IMessage
 * @brief Base interface for all serializable messages.
 *
 * IMessage represents a logical unit of data that can be encoded or decoded
 * by an ISerializer. Concrete implementations are defined either by the
 * serialization backend or by the gameplay/domain layer.
 */
class IMessage {
public:
    virtual ~IMessage() = default;
};

/**
 * @class ISerializer
 * @brief Generic binary serialization interface.
 *
 * ISerializer defines the minimal contract to encode and decode messages.
 * The concrete implementation (Cap'n Proto, Protobuf, etc.) is hidden
 * behind this interface so the backend can be swapped without impacting
 * gameplay code.
 */
class ISerializer {
public:
    virtual ~ISerializer() = default;

    /**
     * @brief Serialize a message into a byte buffer.
     *
     * @param message Constant reference to the message to encode.
     * @return Binary buffer representing the encoded message.
     */
    virtual std::vector<uint8_t> encode(const IMessage &message) = 0;

    /**
     * @brief Deserialize a byte buffer into a message.
     *
     * @param data Binary data received from the network, disk, etc.
     * @return Unique pointer to a newly created IMessage instance.
     */
    virtual std::unique_ptr<IMessage> decode(const std::vector<uint8_t> &data) = 0;
};