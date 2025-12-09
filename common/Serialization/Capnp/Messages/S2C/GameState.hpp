/*
** EPITECH PROJECT, 2025
** Created on 09/12/2025.
** File description:
** GameState.hpp
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <cstdint>
#include <vector>
#include "EntityState.hpp"
#include "schemas/s2c_messages.capnp.h"

namespace RType::Messages::S2C {

    /**
     * @class GameState
     * @brief Complete snapshot of the game world
     * 
     * Contains the authoritative state of all entities in the game.
     * 
     * Usage:
     *   GameState state;
     *   state.serverTick = 42;
     *   state.entities.push_back(entity1);
     *   auto bytes = state.serialize();
     */
    class GameState {
       public:
        uint32_t serverTick;
        std::vector<EntityState> entities{};

        GameState() : serverTick(0) {}

        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::GameState>();

            builder.setServerTick(serverTick);

            auto entitiesBuilder = builder.initEntities(entities.size());
            for (size_t i = 0; i < entities.size(); ++i) {
                entities[i].toCapnp(entitiesBuilder[i]);
            }

            auto bytes = capnp::messageToFlatArray(message);
            auto byteArray = bytes.asBytes();
            return std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        }

        static GameState deserialize(const std::vector<uint8_t> &data) {
            kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(data.data()),
                                                  data.size() / sizeof(capnp::word));

            capnp::FlatArrayMessageReader message(words);
            auto reader = message.getRoot<::GameState>();

            GameState result;
            result.serverTick = reader.getServerTick();

            auto entitiesReader = reader.getEntities();
            result.entities.reserve(entitiesReader.size());
            for (auto entityReader : entitiesReader) {
                result.entities.push_back(EntityState::fromCapnp(entityReader));
            }

            return result;
        }
    };

}  // namespace RType::Messages::S2C