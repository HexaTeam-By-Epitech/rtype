/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** CollisionLayers - Collision layer and mask constants
*/

#pragma once

#include <cstdint>

namespace ecs {
    /**
     * @namespace CollisionLayers
     * @brief Collision layer constants for entity filtering
     * 
     * Each layer is a single bit position (power of 2).
     * Entities have a layer (what they ARE) and a mask (what they can COLLIDE with).
     */
    namespace CollisionLayers {
        // Layer definitions (single bit each)
        constexpr std::uint32_t PLAYER = (1 << 0);             // 0x00000001
        constexpr std::uint32_t ENEMY = (1 << 1);              // 0x00000002
        constexpr std::uint32_t PLAYER_PROJECTILE = (1 << 2);  // 0x00000004
        constexpr std::uint32_t ENEMY_PROJECTILE = (1 << 3);   // 0x00000008
        constexpr std::uint32_t WALL = (1 << 4);               // 0x00000010
        constexpr std::uint32_t COLLECTIBLE = (1 << 5);        // 0x00000020
        constexpr std::uint32_t PLAYER_MODULE = (1 << 6);      // 0x00000040 - Orbital drones

        // Common collision masks (combine multiple layers)
        constexpr std::uint32_t MASK_ALL = 0xFFFFFFFF;
        constexpr std::uint32_t MASK_NONE = 0x00000000;

        // Player collides with: enemies, enemy projectiles, walls, collectibles
        constexpr std::uint32_t MASK_PLAYER = ENEMY | ENEMY_PROJECTILE | WALL | COLLECTIBLE;

        // Enemy collides with: players, player projectiles, player modules
        constexpr std::uint32_t MASK_ENEMY = PLAYER | PLAYER_PROJECTILE | PLAYER_MODULE;

        // Player projectile collides with: enemies, walls
        constexpr std::uint32_t MASK_PLAYER_PROJECTILE = ENEMY | WALL;

        // Enemy projectile collides with: players, player modules, walls
        constexpr std::uint32_t MASK_ENEMY_PROJECTILE = PLAYER | PLAYER_MODULE | WALL;

        // Wall collides with: everything except collectibles
        constexpr std::uint32_t MASK_WALL = PLAYER | ENEMY | PLAYER_PROJECTILE | ENEMY_PROJECTILE;

        // Collectible collides with: players only
        constexpr std::uint32_t MASK_COLLECTIBLE = PLAYER;

        // Player module collides with: enemies and enemy projectiles
        constexpr std::uint32_t MASK_PLAYER_MODULE = ENEMY | ENEMY_PROJECTILE;
    }  // namespace CollisionLayers
}  // namespace ecs
