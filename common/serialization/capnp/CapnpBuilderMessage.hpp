/*
** EPITECH PROJECT, 2025
** Created by mael on 02/12/2025.
** File description:
** CapnpBuilderMessage.hpp
*/

#pragma once

#include "ISerializer.hpp"
#include <functional>
#include <capnp/message.h>

/**
 * @class CapnpBuilderMessage
 * @brief IMessage implementation based on a Cap'n Proto builder.
 *
 * CapnpBuilderMessage encapsulates a filler function (FillerFn) that receives
 * a capnp::MallocMessageBuilder. The Cap'n Proto serializer calls this
 * function to populate the builder before serialization.
 *
 * This class is intended for internal use inside the Cap'n Proto backend and
 * must not be manipulated directly by client/server code.
 */
class CapnpBuilderMessage : public IMessage {
public:
    /**
     * @brief Function type used to fill the Cap'n Proto builder.
     */
    using FillerFn = std::function<void(capnp::MallocMessageBuilder &)>;

    explicit CapnpBuilderMessage(FillerFn filler) : filler_(std::move(filler)) {}

    /**
     * @brief Access the internal filler function.
     */
    [[nodiscard]] const FillerFn &filler() const noexcept { return filler_; }

private:
    FillerFn filler_;
};