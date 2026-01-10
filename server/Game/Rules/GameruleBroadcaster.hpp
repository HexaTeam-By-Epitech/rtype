/*
** EPITECH PROJECT, 2025
** Created on 07/01/2026.
** File description:
** GameruleBroadcaster.hpp
*/

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include "Capnp/Messages/S2C/GamerulePacket.hpp"
#include "Capnp/NetworkMessages.hpp"
#include "GameruleKeys.hpp"
#include "common/Networking/IPacket.hpp"
#include "common/Networking/IPeer.hpp"
#include "common/Networking/NetworkFactory.hpp"
#include "server/Game/Rules/GameRules.hpp"

namespace server {

    /**
     * @class GameruleBroadcaster
     * @brief Utility class to send gamerule updates to clients
     * 
     * This class helps the server synchronize game constants with clients.
     * It can send individual gamerules or batch multiple updates together.
     * 
     * The class provides type-safe methods using GameruleKey enum to prevent typos
     * and string-based methods for flexibility with custom/dynamic keys.
     * 
     * Usage:
     *   // Type-safe with enum (recommended)
     *   GameruleBroadcaster::sendGamerule(peer, GameruleKey::PLAYER_SPEED, 250.0f);
     *   GameruleBroadcaster::broadcastGamerule(peers, GameruleKey::PLAYER_HEALTH, 150.0f);
     *   
     *   // String-based (for custom keys)
     *   GameruleBroadcaster::sendGamerule(peer, "custom.value", 42.0f);
     */
    class GameruleBroadcaster {
       public:
        GameruleBroadcaster() = default;
        ~GameruleBroadcaster() = default;

        /**
         * @brief Send all gamerules to a single client
         * @param peer The peer to send to
         * @param rules The game rules to send
         * 
         * This is typically called when a player first connects to ensure
         * they have all the correct game constants.
         */
        static void sendAllGamerules(IPeer *peer, const GameRules &rules) {
            if (!peer)
                return;

            RType::Messages::S2C::GamerulePacket packet;

            // Add all gamerules using type-safe enum
            packet.addGamerule(GameruleKeys::toString(GameruleKey::PLAYER_HEALTH),
                               static_cast<float>(rules.getDefaultPlayerHealth()));
            packet.addGamerule(GameruleKeys::toString(GameruleKey::PLAYER_SPEED),
                               static_cast<float>(rules.getDefaultPlayerSpeed()));
            packet.addGamerule(GameruleKeys::toString(GameruleKey::PLAYER_SPAWN_X),
                               static_cast<float>(rules.getPlayerSpawnX()));
            packet.addGamerule(GameruleKeys::toString(GameruleKey::PLAYER_SPAWN_Y),
                               static_cast<float>(rules.getPlayerSpawnY()));
            packet.addGamerule(GameruleKeys::toString(GameruleKey::PLAYER_FIRE_RATE),
                               rules.getDefaultPlayerFireRate());
            packet.addGamerule(GameruleKeys::toString(GameruleKey::PLAYER_DAMAGE),
                               static_cast<float>(rules.getDefaultPlayerDamage()));

            sendGamerulePacket(peer, packet);
        }

        /**
         * @brief Send a single gamerule update to a single client (type-safe)
         * @param peer The peer to send to
         * @param key The gamerule key enum
         * @param value The gamerule value
         */
        static void sendGamerule(IPeer *peer, GameruleKey key, float value) {
            sendGamerule(peer, GameruleKeys::toString(key), value);
        }

        /**
         * @brief Send a single gamerule update to a single client (string version)
         * @param peer The peer to send to
         * @param key The gamerule key string
         * @param value The gamerule value
         */
        static void sendGamerule(IPeer *peer, const std::string &key, float value) {
            if (!peer)
                return;

            RType::Messages::S2C::GamerulePacket packet;
            packet.addGamerule(key, value);
            sendGamerulePacket(peer, packet);
        }

        /**
         * @brief Broadcast a single gamerule update to multiple clients (type-safe)
         * @param peers The peers to send to
         * @param key The gamerule key enum
         * @param value The gamerule value
         */
        static void broadcastGamerule(const std::vector<IPeer *> &peers, GameruleKey key, float value) {
            broadcastGamerule(peers, GameruleKeys::toString(key), value);
        }

        /**
         * @brief Broadcast a single gamerule update to multiple clients (string version)
         * @param peers The peers to send to
         * @param key The gamerule key string
         * @param value The gamerule value
         */
        static void broadcastGamerule(const std::vector<IPeer *> &peers, const std::string &key,
                                      float value) {
            RType::Messages::S2C::GamerulePacket packet;
            packet.addGamerule(key, value);

            for (auto *peer : peers) {
                if (peer) {
                    sendGamerulePacket(peer, packet);
                }
            }
        }

        /**
         * @brief Broadcast multiple gamerule updates to multiple clients (type-safe)
         * @param peers The peers to send to
         * @param gamerules Map of gamerule enum-value pairs
         */
        static void broadcastGamerules(const std::vector<IPeer *> &peers,
                                       const std::unordered_map<GameruleKey, float> &gamerules) {
            RType::Messages::S2C::GamerulePacket packet;

            for (const auto &[key, value] : gamerules) {
                packet.addGamerule(GameruleKeys::toString(key), value);
            }

            for (auto *peer : peers) {
                if (peer) {
                    sendGamerulePacket(peer, packet);
                }
            }
        }

        /**
         * @brief Broadcast multiple gamerule updates to multiple clients (string version)
         * @param peers The peers to send to
         * @param gamerules Map of gamerule string-value pairs
         */
        static void broadcastGamerules(const std::vector<IPeer *> &peers,
                                       const std::unordered_map<std::string, float> &gamerules) {
            RType::Messages::S2C::GamerulePacket packet;

            for (const auto &[key, value] : gamerules) {
                packet.addGamerule(key, value);
            }

            for (auto *peer : peers) {
                if (peer) {
                    sendGamerulePacket(peer, packet);
                }
            }
        }

        /**
         * @brief Send a pre-built gamerule packet to a single client
         * @param peer The peer to send to
         * @param packet The gamerule packet to send
         */
        static void sendGamerulePacket(IPeer *peer, const RType::Messages::S2C::GamerulePacket &packet) {
            if (!peer)
                return;

            // Serialize the packet
            std::vector<uint8_t> payload = packet.serialize();
            std::vector<uint8_t> message =
                NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_GAMERULE_UPDATE, payload);

            // Create network packet and send reliably
            std::unique_ptr<IPacket> netPacket =
                createPacket(message, static_cast<int>(PacketFlag::RELIABLE));
            peer->send(std::move(netPacket), 0);
        }
    };

}  // namespace server