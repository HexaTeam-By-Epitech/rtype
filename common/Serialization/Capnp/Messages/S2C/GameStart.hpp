/*
** EPITECH PROJECT, 2025
** Created on 09/12/2025.
** File description:
** GameStart.hpp
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <cstdint>
#include <vector>
#include "GameState.hpp"
#include "schemas/s2c_messages.capnp.h"

namespace RType::Messages::S2C {

    /**
     * @class GameStart
     * @brief Game start notification from server to client
     * 
     * Provides the client with their entity ID and initial game state.
     * 
     * Usage:
     *   GameStart start;
     *   start.yourEntityId = 123;
     *   start.initialState = gameState;
     *   auto bytes = start.serialize();
     */
    class GameStart {
       public:
        uint32_t yourEntityId;
        GameState initialState;

        GameStart() : yourEntityId(0) {}

        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::GameStart>();

            builder.setYourEntityId(yourEntityId);

            // Build initial state inline
            auto stateBuilder = builder.initInitialState();
            stateBuilder.setServerTick(initialState.serverTick);

            auto entitiesBuilder = stateBuilder.initEntities(initialState.entities.size());
            for (size_t i = 0; i < initialState.entities.size(); ++i) {
                initialState.entities[i].toCapnp(entitiesBuilder[i]);
            }

            auto bytes = capnp::messageToFlatArray(message);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        static GameStart deserialize(const std::vector<uint8_t> &data) {
            // Ensure buffer is word-aligned for Cap'n Proto (undefined behavior if not)
            KJ_REQUIRE(data.size() % sizeof(capnp::word) == 0,
                       "Serialized data size must be a multiple of capnp::word");
            auto aligned = kj::heapArray<uint8_t>(data.size());
            memcpy(aligned.begin(), data.data(), data.size());
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(aligned.begin()),
                                                  data.size() / sizeof(capnp::word));

            capnp::FlatArrayMessageReader message(words);
            auto reader = message.getRoot<::GameStart>();

            GameStart result;
            result.yourEntityId = reader.getYourEntityId();

            // Deserialize initial state inline
            auto stateReader = reader.getInitialState();
            result.initialState.serverTick = stateReader.getServerTick();

            auto entitiesReader = stateReader.getEntities();
            result.initialState.entities.reserve(entitiesReader.size());
            for (auto entityReader : entitiesReader) {
                result.initialState.entities.push_back(EntityState::fromCapnp(entityReader));
            }

            return result;
        }
    };

}  // namespace RType::Messages::S2C