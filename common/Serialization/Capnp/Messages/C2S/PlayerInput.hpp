/*
** EPITECH PROJECT, 2025
** Created on 09/12/2025.
** File description:
** PlayerInput.hpp
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <cstdint>
#include <vector>
#include "../Shared/SharedTypes.hpp"
#include "schemas/c2s_messages.capnp.h"

namespace RType::Messages::C2S {

    /**
     * @class PlayerInput
     * @brief Player input message sent from client to server
     * 
     * Contains the sequence ID for input ordering and the list of actions.
     * 
     * Usage:
     *   PlayerInput input(123, {Action::MoveUp, Action::Shoot});
     *   auto bytes = input.serialize();
     *   // Send bytes over network...
     */
    class PlayerInput {
       public:
        uint32_t _sequenceId;
        std::vector<Shared::Action> actions{};

        PlayerInput() : _sequenceId(0) {}

        PlayerInput(uint32_t seqId, const std::vector<Shared::Action> &acts)
            : _sequenceId(seqId), actions(acts) {}

        /**
         * @brief Serialize to byte vector
         */
        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::PlayerInput>();

            builder.setSequenceId(_sequenceId);

            auto actionsBuilder = builder.initActions(actions.size());
            for (size_t i = 0; i < actions.size(); ++i) {
                actionsBuilder.set(i, Shared::toCapnpAction(actions[i]));
            }

            auto bytes = capnp::messageToFlatArray(message);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        /**
         * @brief Deserialize from byte vector
         */
        static PlayerInput deserialize(const std::vector<uint8_t> &data) {
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(data.data()),
                                                  data.size() / sizeof(capnp::word));

            capnp::FlatArrayMessageReader message(words);
            auto reader = message.getRoot<::PlayerInput>();

            PlayerInput result;
            result._sequenceId = reader.getSequenceId();

            auto actionsReader = reader.getActions();
            result.actions.reserve(actionsReader.size());
            for (auto action : actionsReader) {
                result.actions.push_back(Shared::fromCapnpAction(action));
            }

            return result;
        }
    };

}  // namespace RType::Messages::C2S