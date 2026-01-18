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
#include <string>
#include <vector>
#include "GameState.hpp"
#include "schemas/s2c_messages.capnp.h"

namespace RType::Messages::S2C {

    /**
     * @struct MapConfig
     * @brief Map background configuration sent to client
     */
    struct MapConfig {
        std::string background;            ///< Path to main background texture
        std::string parallaxBackground;    ///< Path to parallax layer texture (empty = none)
        float scrollSpeed = 50.0f;         ///< Background scroll speed in pixels/second
        float parallaxSpeedFactor = 0.3f;  ///< Parallax layer speed factor
    };

    /**
     * @class GameStart
     * @brief Game start notification from server to client
     *
     * Provides the client with their entity ID, initial game state, and map configuration.
     *
     * Usage:
     *   GameStart start;
     *   start.yourEntityId = 123;
     *   start.initialState = gameState;
     *   start.mapConfig.background = "backgrounds/bg-full.png";
     *   auto bytes = start.serialize();
     */
    class GameStart {
       public:
        uint32_t yourEntityId;
        GameState initialState;
        MapConfig mapConfig;

        GameStart() : yourEntityId(0) {}

        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::GameStart>();

            builder.setYourEntityId(yourEntityId);

            // Build initial state inline
            auto stateBuilder = builder.initInitialState();
            stateBuilder.setServerTick(initialState.serverTick);

            auto entitiesBuilder =
                stateBuilder.initEntities(static_cast<unsigned int>(initialState.entities.size()));
            for (size_t i = 0; i < initialState.entities.size(); ++i) {
                initialState.entities[i].toCapnp(entitiesBuilder[static_cast<unsigned int>(i)]);
            }

            // Build map config
            auto mapConfigBuilder = builder.initMapConfig();
            mapConfigBuilder.setBackground(mapConfig.background);
            mapConfigBuilder.setParallaxBackground(mapConfig.parallaxBackground);
            mapConfigBuilder.setScrollSpeed(mapConfig.scrollSpeed);
            mapConfigBuilder.setParallaxSpeedFactor(mapConfig.parallaxSpeedFactor);

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

            // Deserialize map config
            if (reader.hasMapConfig()) {
                auto mapConfigReader = reader.getMapConfig();
                result.mapConfig.background = mapConfigReader.getBackground();
                result.mapConfig.parallaxBackground = mapConfigReader.getParallaxBackground();
                result.mapConfig.scrollSpeed = mapConfigReader.getScrollSpeed();
                result.mapConfig.parallaxSpeedFactor = mapConfigReader.getParallaxSpeedFactor();
            }

            return result;
        }
    };

}  // namespace RType::Messages::S2C