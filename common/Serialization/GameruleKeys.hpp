/*
** EPITECH PROJECT, 2025
** Created on 07/01/2026.
** File description:
** GameruleKeys.hpp - Shared gamerule key constants
*/

#pragma once

#include <string>
#include <unordered_map>

/**
 * @enum GameruleKey
 * @brief Type-safe enum for gamerule identifiers
 * 
 * Using an enum instead of raw strings prevents typos and makes the code more maintainable.
 */
enum class GameruleKey {
    PLAYER_SPEED,
    PLAYER_HEALTH,
    PLAYER_FIRE_RATE,
    PLAYER_DAMAGE,
    PLAYER_SPAWN_X,
    PLAYER_SPAWN_Y
};

namespace GameruleKeys {

    /**
     * @brief Convert GameruleKey enum to string
     * @param key The enum key
     * @return String representation of the key
     */
    inline const char *toString(GameruleKey key) {
        switch (key) {
            case GameruleKey::PLAYER_SPEED:
                return "player.speed";
            case GameruleKey::PLAYER_HEALTH:
                return "player.health";
            case GameruleKey::PLAYER_FIRE_RATE:
                return "player.fireRate";
            case GameruleKey::PLAYER_DAMAGE:
                return "player.damage";
            case GameruleKey::PLAYER_SPAWN_X:
                return "player.spawnX";
            case GameruleKey::PLAYER_SPAWN_Y:
                return "player.spawnY";
            default:
                return "unknown";
        }
    }

    /**
     * @brief Convert string to GameruleKey enum (optional, for deserialization)
     * @param str The string key
     * @return Optional GameruleKey if valid, nullopt otherwise
     */
    inline std::optional<GameruleKey> fromString(const std::string &str) {
        static const std::unordered_map<std::string, GameruleKey> map = {
            {"player.speed", GameruleKey::PLAYER_SPEED},
            {"player.health", GameruleKey::PLAYER_HEALTH},
            {"player.fireRate", GameruleKey::PLAYER_FIRE_RATE},
            {"player.damage", GameruleKey::PLAYER_DAMAGE},
            {"player.spawnX", GameruleKey::PLAYER_SPAWN_X},
            {"player.spawnY", GameruleKey::PLAYER_SPAWN_Y}};

        auto it = map.find(str);
        if (it != map.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    // Legacy constants for backward compatibility (deprecated)
    [[deprecated("Use GameruleKey::PLAYER_SPEED with GameruleKeys::toString() instead")]]
    static constexpr const char *PLAYER_SPEED = "player.speed";

    [[deprecated("Use GameruleKey::PLAYER_HEALTH with GameruleKeys::toString() instead")]]
    static constexpr const char *PLAYER_HEALTH = "player.health";

    [[deprecated("Use GameruleKey::PLAYER_FIRE_RATE with GameruleKeys::toString() instead")]]
    static constexpr const char *PLAYER_FIRE_RATE = "player.fireRate";

    [[deprecated("Use GameruleKey::PLAYER_DAMAGE with GameruleKeys::toString() instead")]]
    static constexpr const char *PLAYER_DAMAGE = "player.damage";

    [[deprecated("Use GameruleKey::PLAYER_SPAWN_X with GameruleKeys::toString() instead")]]
    static constexpr const char *PLAYER_SPAWN_X = "player.spawnX";

    [[deprecated("Use GameruleKey::PLAYER_SPAWN_Y with GameruleKeys::toString() instead")]]
    static constexpr const char *PLAYER_SPAWN_Y = "player.spawnY";

}  // namespace GameruleKeys