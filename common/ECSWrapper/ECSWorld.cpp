/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ECSWorld implementation
*/

#include "ECSWorld.hpp"
#include <algorithm>
#include "../Logger/Logger.hpp"

namespace ecs::wrapper {

    std::string_view systemIdToName(SystemId id) {
        switch (id) {
            case SystemId::Movement:
                return "MovementSystem";
            case SystemId::Collision:
                return "CollisionSystem";
            case SystemId::Health:
                return "HealthSystem";
            case SystemId::Spawn:
                return "SpawnSystem";
            case SystemId::AI:
                return "AISystem";
            case SystemId::Boundary:
                return "BoundarySystem";
            case SystemId::Weapon:
                return "WeaponSystem";
            default:
                return "UnknownSystem";
        }
    }

    // Entity implementation

    Entity::Entity(Address address, Registry *registry) : _address(address), _registry(registry) {}

    Address Entity::getAddress() const {
        return _address;
    }

    bool Entity::isValid() const {
        if (!_registry || _address == 0) {
            return false;
        }

        // Check if entity exists in registry by checking its signature
        try {
            _registry->getSignature(_address);
            return true;
        } catch (...) {
            return false;
        }
    }

    Entity::operator Address() const {
        return _address;
    }

    // ECSWorld implementation

    ECSWorld::ECSWorld() : _registry(std::make_unique<Registry>()) {}

    ECSWorld::~ECSWorld() = default;

    Entity ECSWorld::createEntity() {
        Address address = _registry->newEntity();
        return Entity(address, _registry.get());
    }

    std::vector<Entity> ECSWorld::createEntities(size_t count) {
        std::vector<Entity> entities;
        entities.reserve(count);

        for (size_t i = 0; i < count; ++i) {
            entities.push_back(createEntity());
        }

        return entities;
    }

    Entity ECSWorld::getEntity(Address address) {
        return Entity(address, _registry.get());
    }

    void ECSWorld::destroyEntity(const Entity &entity) {
        if (entity.isValid()) {
            _registry->destroyEntity(entity.getAddress());
        }
    }

    void ECSWorld::destroyEntity(Address address) {
        _registry->destroyEntity(address);
    }

    void ECSWorld::removeSystem(const std::string &name) {
        auto it = _systems.find(name);
        if (it != _systems.end()) {
            _systems.erase(it);

            // Remove from order list
            auto orderIt = std::find(_systemsOrder.begin(), _systemsOrder.end(), name);
            if (orderIt != _systemsOrder.end()) {
                _systemsOrder.erase(orderIt);
            }
        }
    }

    void ECSWorld::removeSystem(SystemId id) {
        removeSystem(std::string(systemIdToName(id)));
    }

    void ECSWorld::update(float deltaTime) {
        for (const auto &systemName : _systemsOrder) {
            auto it = _systems.find(systemName);
            if (it != _systems.end()) {
                try {
                    it->second->update(*_registry, deltaTime);
                } catch (const std::exception &e) {
                    LOG_ERROR("ECSWorld::update - Error in system '", systemName, "': ", e.what());
                }
            }
        }
    }

    bool ECSWorld::updateSystem(const std::string &name, float deltaTime) {
        auto it = _systems.find(name);
        if (it == _systems.end()) {
            return false;
        }

        try {
            it->second->update(*_registry, deltaTime);
            return true;
        } catch (const std::exception &e) {
            LOG_ERROR("ECSWorld::updateSystem - Error in system '", name, "': ", e.what());
            return false;
        }
    }

    bool ECSWorld::updateSystem(SystemId id, float deltaTime) {
        return updateSystem(std::string(systemIdToName(id)), deltaTime);
    }

    Registry &ECSWorld::getRegistry() {
        return *_registry;
    }

    const Registry &ECSWorld::getRegistry() const {
        return *_registry;
    }

    void ECSWorld::clear() {
        // Get all entity addresses and destroy them
        auto allEntities = _registry->view<>();
        for (auto address : allEntities) {
            _registry->destroyEntity(address);
        }
    }

    size_t ECSWorld::getSystemCount() const {
        return _systems.size();
    }

}  // namespace ecs::wrapper
