/*
** EPITECH PROJECT, 2025
** Created by mael on 02/12/2025.
** File description:
** CapnpSerializer.hpp
*/

#pragma once

#include "CapnpBuilderMessage.hpp"
#include "CapnpMessage.hpp"
#include "ISerializer.hpp"

/**
 * @class CapnpSerializer
 * @brief ISerializer implementation backed by Cap'n Proto.
 *
 * CapnpSerializer knows how to encode and decode IMessage instances using
 * Cap'n Proto primitives. Concrete message types (CapnpMessage,
 * CapnpBuilderMessage) are kept inside the backend.
 */
class CapnpSerializer final : public ISerializer {
   public:
    CapnpSerializer();
    ~CapnpSerializer() override;

    /**
     * @brief Encode an IMessage into a Cap'n Proto binary stream.
     *
     * Supports internal implementations CapnpMessage and CapnpBuilderMessage.
     * Any other IMessage type will cause an exception to be thrown.
     */
    std::vector<uint8_t> encode(const IMessage &message) override;

    /**
     * @brief Decode a binary stream into an IMessage instance.
     *
     * Returns a CapnpMessage object holding the received bytes.
     */
    std::unique_ptr<IMessage> decode(const std::vector<uint8_t> &data) override;
};