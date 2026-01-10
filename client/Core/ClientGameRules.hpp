/*
** EPITECH PROJECT, 2025
** Created on 07/01/2026.
** File description:
** ClientGameRules.hpp
*/

#pragma once

#include <mutex>
#include <string>
#include <unordered_map>
#include "GameruleKeys.hpp"

namespace client {

    /**
     * @class ClientGameRules
     * @brief Client-side storage for game rules synchronized from server
     * 
     * This class stores game constants that are received from the server.
     * These values are used for client-side prediction and other gameplay features.
     * 
     * The class is thread-safe to allow updates from the network thread
     * while the game loop reads values.
     * 
     * Usage:
     *   auto& rules = ClientGameRules::getInstance();
     *   
     *   // Type-safe with enum (recommended)
     *   float speed = rules.get(GameruleKey::PLAYER_SPEED, 200.0f);
     *   rules.update(GameruleKey::PLAYER_SPEED, 250.0f);
     *   
     *   // String-based (for custom keys)
     *   float customValue = rules.get("custom.value", 0.0f);
     */
    class ClientGameRules {
       public:
        /**
         * @brief Get the singleton instance
         * @return Reference to the singleton instance
         */
        static ClientGameRules &getInstance() {
            static ClientGameRules instance;
            return instance;
        }

        // Delete copy constructor and assignment operator
        ClientGameRules(const ClientGameRules &) = delete;
        ClientGameRules &operator=(const ClientGameRules &) = delete;

        /**
         * @brief Get a gamerule value (type-safe with enum)
         * @param key The gamerule key enum
         * @param defaultValue Value to return if key doesn't exist
         * @return The gamerule value or defaultValue
         */
        float get(GameruleKey key, float defaultValue = 0.0f) const {
            return get(GameruleKeys::toString(key), defaultValue);
        }

        /**
         * @brief Get a gamerule value (string version)
         * @param key The gamerule key (e.g., "player.speed")
         * @param defaultValue Value to return if key doesn't exist
         * @return The gamerule value or defaultValue
         */
        float get(const std::string &key, float defaultValue = 0.0f) const {
            std::scoped_lock lock(mutex_);
            auto it = rules_.find(key);
            return (it != rules_.end()) ? it->second : defaultValue;
        }

        /**
         * @brief Update a gamerule value (type-safe with enum)
         * @param key The gamerule key enum
         * @param value The new value
         */
        void update(GameruleKey key, float value) { update(GameruleKeys::toString(key), value); }

        /**
         * @brief Update a gamerule value (string version)
         * @param key The gamerule key
         * @param value The new value
         */
        void update(const std::string &key, float value) {
            std::scoped_lock lock(mutex_);
            rules_[key] = value;
        }

        /**
         * @brief Update multiple gamerules at once
         * @param gamerules Map of key-value pairs to update
         */
        void updateMultiple(const std::unordered_map<std::string, float> &gamerules) {
            std::scoped_lock lock(mutex_);
            for (const auto &[key, value] : gamerules) {
                rules_[key] = value;
            }
        }

        /**
         * @brief Check if a gamerule exists (type-safe with enum)
         * @param key The gamerule key enum
         * @return True if the gamerule exists
         */
        bool has(GameruleKey key) const { return has(GameruleKeys::toString(key)); }

        /**
         * @brief Check if a gamerule exists (string version)
         * @param key The gamerule key
         * @return True if the gamerule exists
         */
        bool has(const std::string &key) const {
            std::scoped_lock lock(mutex_);
            return rules_.find(key) != rules_.end();
        }

        /**
         * @brief Clear all gamerules
         */
        void clear() {
            std::scoped_lock lock(mutex_);
            rules_.clear();
        }

        /**
         * @brief Get the number of stored gamerules
         * @return Number of gamerules
         */
        size_t size() const {
            std::scoped_lock lock(mutex_);
            return rules_.size();
        }

       private:
        ClientGameRules() = default;
        ~ClientGameRules() = default;

        mutable std::mutex mutex_;
        std::unordered_map<std::string, float> rules_;
    };

}  // namespace client