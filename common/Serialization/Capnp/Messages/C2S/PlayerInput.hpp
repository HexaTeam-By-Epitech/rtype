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
#include <stdexcept>
#include <vector>
#include "../Shared/SharedTypes.hpp"
#include "schemas/c2s_messages.capnp.h"

namespace RType::Messages::C2S {

    /**
     * @class PlayerInput
     * @brief Player input message sent from client to server (with redundancy)
     * 
     * Contains a history of recent inputs to handle packet loss.
     */
    class PlayerInput {
       public:
        struct InputSnapshot {
            uint32_t sequenceId;
            std::vector<Shared::Action> actions;
        };

        std::vector<InputSnapshot> inputs;

        PlayerInput() = default;

        // Constructor for a single input (legacy support, wraps in list)
        PlayerInput(uint32_t seqId, const std::vector<Shared::Action> &acts) {
            inputs.push_back({seqId, acts});
        }

        // Constructor for full history
        explicit PlayerInput(const std::vector<InputSnapshot> &history) : inputs(history) {}

        /**
         * @brief Serialize to byte vector
         */
        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::PlayerInput>();

            auto inputsBuilder = builder.initInputs(static_cast<unsigned int>(inputs.size()));

            for (size_t i = 0; i < inputs.size(); ++i) {
                auto snapshotBuilder = inputsBuilder[i];
                snapshotBuilder.setSequenceId(inputs[i].sequenceId);

                auto actionsBuilder =
                    snapshotBuilder.initActions(static_cast<unsigned int>(inputs[i].actions.size()));
                for (size_t j = 0; j < inputs[i].actions.size(); ++j) {
                    actionsBuilder.set(static_cast<unsigned int>(j),
                                       Shared::toCapnpAction(inputs[i].actions[j]));
                }
            }

            auto bytes = capnp::messageToFlatArray(message);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        /**
         * @brief Deserialize from byte vector
         */
        static PlayerInput deserialize(const std::vector<uint8_t> &data) {
            KJ_REQUIRE(data.size() % sizeof(capnp::word) == 0,
                       "Serialized data size must be a multiple of capnp::word");
            auto aligned = kj::heapArray<uint8_t>(data.size());
            memcpy(aligned.begin(), data.data(), data.size());
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(aligned.begin()),
                                                  data.size() / sizeof(capnp::word));

            capnp::FlatArrayMessageReader message(words);
            auto reader = message.getRoot<::PlayerInput>();

            PlayerInput result;
            auto inputsReader = reader.getInputs();

            static constexpr size_t MAX_INPUTS_PER_PACKET = 64;  // Safety limit
            if (inputsReader.size() > MAX_INPUTS_PER_PACKET) {
                throw std::runtime_error("Too many inputs in PlayerInput message");
            }

            result.inputs.reserve(inputsReader.size());

            for (auto snapshotReader : inputsReader) {
                InputSnapshot snapshot;
                snapshot.sequenceId = snapshotReader.getSequenceId();

                auto actionsReader = snapshotReader.getActions();
                snapshot.actions.reserve(actionsReader.size());

                for (auto action : actionsReader) {
                    snapshot.actions.push_back(Shared::fromCapnpAction(action));
                }
                result.inputs.push_back(snapshot);
            }

            return result;
        }
    };

}  // namespace RType::Messages::C2S