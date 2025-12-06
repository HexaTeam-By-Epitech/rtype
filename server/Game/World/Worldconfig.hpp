/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** Worldconfig.hpp
*/

#pragma once

namespace server {

/**
 * @struct WorldConfig
 * @brief Configuration settings for the game world
 *
 * Defines default parameters like map size, physics settings, and tick rates.
 */
struct WorldConfig {
    int width = 100;          ///< Width of the world
    int height = 100;         ///< Height of the world
    float tickRate = 0.016f;  ///< Time per simulation tick (in seconds)
    bool enablePhysics = true;///< Toggle physics simulation
};

} // namespace server

