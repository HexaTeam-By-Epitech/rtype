/*
** EPITECH PROJECT, 2025
** Created by mael on 02/12/2025.
** File description:
** CapnpMessage.hpp
*/

#pragma once

#include "ISerializer.hpp"
#include <vector>
#include <cstdint>

/**
 * @class CapnpMessage
 * @brief Concrete IMessage implementation storing Cap'n Proto bytes.
 *
 * This class wraps a binary buffer already serialized using Cap'n Proto.
 * It is mainly used internally by the Cap'n Proto backend and should not be
 * exposed to gameplay modules.
 */
class CapnpMessage final : public IMessage {
public:
    CapnpMessage() = default;
    explicit CapnpMessage(std::vector<uint8_t> data) : data_(std::move(data)) {}

    /**
     * @brief Read-only access to the underlying binary buffer.
     */
    [[nodiscard]] const std::vector<uint8_t> &data() const noexcept { return data_; }

    /**
     * @brief Mutable access to the underlying binary buffer.
     */
    std::vector<uint8_t> &data() noexcept { return data_; }

private:
    std::vector<uint8_t> data_;
};