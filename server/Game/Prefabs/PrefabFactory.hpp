/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** PrefabFactory.hpp
*/

#pragma once

#include <memory>

namespace server {

/**
 * @class PrefabFactory
 * @brief Factory for creating game entity prefabs
 *
 * Used to instantiate entities with predefined components and settings.
 */
class PrefabFactory {
public:
    PrefabFactory() = default;
    ~PrefabFactory() = default;

    // Prototypes
    int createPlayer() const;
    int createEnemy() const;
    int createProjectile() const;
};

} // namespace server

