/*
** EPITECH PROJECT, 2025
** Created on 07/01/2026.
** File description:
** GamerulePacket.hpp
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include "schemas/s2c_messages.capnp.h"

namespace RType::Messages::S2C {

    /**
     * @struct Gamerule
     * @brief A single gamerule key-value pair
     * 
     * Represents a game constant that can be synchronized from server to client.
     * Examples: "player.speed", "player.health", "player.fireRate"
     */
    struct Gamerule {
        std::string key;
        float value;

        Gamerule() : value(0.0f) {}
        Gamerule(const std::string &k, float v) : key(k), value(v) {}
    };

    /**
     * @class GamerulePacket
     * @brief Packet containing gamerule updates from server to client
     * 
     * This packet is used to synchronize game constants between server and client.
     * The client needs these values for functions such as prediction.
     * 
     * The packet is designed to be lightweight:
     * - Can contain multiple gamerules to update at once
     * - Can contain a single gamerule for targeted updates
     * - Uses a map-like structure for efficient lookups
     * 
     * Usage on server:
     *   GamerulePacket packet;
     *   packet.addGamerule("player.speed", 200.0f);
     *   packet.addGamerule("player.health", 100.0f);
     *   auto bytes = packet.serialize();
     * 
     * Usage on client:
     *   auto packet = GamerulePacket::deserialize(data);
     *   for (const auto& [key, value] : packet.getGamerules()) {
     *       // Update local gamerule storage
     *   }
     */
    class GamerulePacket {
       public:
        GamerulePacket() = default;

        /**
         * @brief Add a gamerule to the packet
         * @param key The gamerule identifier (e.g., "player.speed")
         * @param value The gamerule value
         */
        void addGamerule(const std::string &key, float value) { gamerules[key] = value; }

        /**
         * @brief Add a gamerule struct to the packet
         * @param gamerule The gamerule to add
         */
        void addGamerule(const Gamerule &gamerule) { gamerules[gamerule.key] = gamerule.value; }

        /**
         * @brief Get all gamerules
         * @return Map of gamerule key-value pairs
         */
        [[nodiscard]] const std::unordered_map<std::string, float> &getGamerules() const { return gamerules; }

        /**
         * @brief Check if packet contains a specific gamerule
         * @param key The gamerule key
         * @return True if the gamerule exists
         */
        [[nodiscard]] bool hasGamerule(const std::string &key) const {
            return gamerules.find(key) != gamerules.end();
        }

        /**
         * @brief Get a specific gamerule value
         * @param key The gamerule key
         * @param defaultValue Value to return if key doesn't exist
         * @return The gamerule value or defaultValue
         */
        [[nodiscard]] float getGamerule(const std::string &key, float defaultValue = 0.0f) const {
            auto it = gamerules.find(key);
            return (it != gamerules.end()) ? it->second : defaultValue;
        }

        /**
         * @brief Clear all gamerules from the packet
         */
        void clear() { gamerules.clear(); }

        /**
         * @brief Get the number of gamerules in the packet
         * @return Number of gamerules
         */
        [[nodiscard]] size_t size() const { return gamerules.size(); }

        /**
         * @brief Serialize the packet to bytes using Cap'n Proto
         * @return Serialized packet data
         */
        [[nodiscard]] std::vector<uint8_t> serialize() const {
            capnp::MallocMessageBuilder message;
            auto builder = message.initRoot<::GamerulePacket>();

            // Build gamerules list
            auto gamerulesBuilder = builder.initGamerules(static_cast<unsigned int>(gamerules.size()));
            unsigned int i = 0;
            for (const auto &[key, value] : gamerules) {
                auto gameruleBuilder = gamerulesBuilder[i];
                gameruleBuilder.setKey(key);
                gameruleBuilder.setValue(value);
                ++i;
            }

            auto bytes = capnp::messageToFlatArray(message);
            auto byteArray = bytes.asBytes();
            return std::vector(byteArray.begin(), byteArray.end());
        }

        /**
         * @brief Deserialize a packet from bytes using Cap'n Proto
         * @param data Serialized packet data
         * @return Deserialized GamerulePacket
         */
        static GamerulePacket deserialize(const std::vector<uint8_t> &data) {
            // Ensure buffer is word-aligned for Cap'n Proto (undefined behavior if not)
            KJ_REQUIRE(data.size() % sizeof(capnp::word) == 0,
                       "Serialized data size must be a multiple of capnp::word");
            auto aligned = kj::heapArray<uint8_t>(data.size());
            memcpy(aligned.begin(), data.data(), data.size());
            kj::ArrayPtr words(reinterpret_cast<const capnp::word *>(aligned.begin()),
                               data.size() / sizeof(capnp::word));

            capnp::FlatArrayMessageReader message(words);
            auto reader = message.getRoot<::GamerulePacket>();

            GamerulePacket result;

            // Deserialize gamerules list
            auto gamerarulesReader = reader.getGamerules();
            for (auto gameruleReader : gamerarulesReader) {
                std::string key = gameruleReader.getKey().cStr();
                float value = gameruleReader.getValue();
                result.addGamerule(key, value);
            }

            return result;
        }

       private:
        std::unordered_map<std::string, float> gamerules;
    };

}  // namespace RType::Messages::S2C