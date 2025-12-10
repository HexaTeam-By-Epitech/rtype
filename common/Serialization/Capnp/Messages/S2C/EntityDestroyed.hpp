/*
** EPITECH PROJECT, 2025
** Created on 09/12/2025.
** File description:
** EntityDestroyed.hpp 
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <cstdint>
#include <vector>
#include "../Shared/SharedTypes.hpp"
#include "schemas/s2c_messages.capnp.h"

namespace RType::Messages::S2C {

    /**
     * @class EntityDestroyed
     * @brief Entity destruction notification
     * 
     * Sent when an entity is removed from the game world.
     */
    class EntityDestroyed {
       public:
        uint32_t entityId;
        Shared::DestroyReason reason;

        EntityDestroyed() : entityId(0), reason(Shared::DestroyReason::OutOfBounds) {}
        EntityDestroyed(uint32_t id, Shared::DestroyReason r) : entityId(id), reason(r) {}

        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::EntityDestroyed>();

            builder.setEntityId(entityId);
            builder.setReason(Shared::toCapnpDestroyReason(reason));

            auto bytes = capnp::messageToFlatArray(message);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        static EntityDestroyed deserialize(const std::vector<uint8_t> &data) {
            // Ensure buffer is word-aligned for Cap'n Proto (undefined behavior if not)
            KJ_REQUIRE(data.size() % sizeof(capnp::word) == 0,
                       "Serialized data size must be a multiple of capnp::word");
            auto aligned = kj::heapArray<uint8_t>(data.size());
            memcpy(aligned.begin(), data.data(), data.size());
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(aligned.begin()),
                                                  data.size() / sizeof(capnp::word));

            capnp::FlatArrayMessageReader message(words);
            auto reader = message.getRoot<::EntityDestroyed>();

            EntityDestroyed result;
            result.entityId = reader.getEntityId();
            result.reason = Shared::fromCapnpDestroyReason(reader.getReason());

            return result;
        }
    };

}  // namespace RType::Messages::S2C