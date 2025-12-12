/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** World.cpp
*/

#include "World.hpp"
#include <algorithm>
#include "../../../common/Logger/Logger.hpp"

namespace server {

    World::World(std::shared_ptr<ecs::Registry> registry) : _registry(registry) {
        if (!_registry) {
            throw std::invalid_argument("World: registry cannot be null");
        }
        LOG_DEBUG("World created with registry");
    }

    void World::addEntity(int entityID) {
        _entities.push_back(entityID);
        LOG_DEBUG("World: Entity ", entityID, " added to world tracking (total: ", _entities.size(), ")");
    }

    void World::removeEntity(int entityID) {
        auto it = std::find(_entities.begin(), _entities.end(), entityID);
        if (it != _entities.end()) {
            _entities.erase(it);

            // Also remove from registry
            if (_registry) {
                _registry->destroyEntity(entityID);
            }

            LOG_DEBUG("World: Entity ", entityID, " removed from world (remaining: ", _entities.size(), ")");
        }
    }

    void World::update(float dt) {
        // World-level update logic
        // Currently systems are called externally, but could add:
        // - Global world effects (gravity, environment)
        // - Entity lifecycle management
        // - Spatial partitioning updates
    }

}  // namespace server
