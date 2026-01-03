/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ECSWorld template implementations
*/

#pragma once

#include "ECSWorld.hpp"
#include <iostream>

namespace ecs::wrapper {

    // Entity template methods

    template <typename T>
    Entity& Entity::with(const T& component) {
        if (_registry && _address != 0) {
            _registry->setComponent(_address, component);
        }
        return *this;
    }

    template <typename T>
    T& Entity::get() {
        if (!_registry || _address == 0) {
            throw std::runtime_error("Entity::get() - Invalid entity");
        }
        return _registry->getComponent<T>(_address);
    }

    template <typename T>
    const T& Entity::get() const {
        if (!_registry || _address == 0) {
            throw std::runtime_error("Entity::get() - Invalid entity");
        }
        return _registry->getComponent<T>(_address);
    }

    template <typename T>
    bool Entity::has() const {
        if (!_registry || _address == 0) {
            return false;
        }
        return _registry->hasComponent<T>(_address);
    }

    template <typename T>
    Entity& Entity::remove() {
        if (_registry && _address != 0) {
            _registry->removeComponent<T>(_address);
        }
        return *this;
    }

    // ECSWorld template methods

    template <typename... Components>
    std::vector<Entity> ECSWorld::query() {
        std::vector<Entity> result;
        auto addresses = _registry->view<Components...>();
        
        result.reserve(addresses.size());
        for (auto address : addresses) {
            result.emplace_back(address, _registry.get());
        }
        
        return result;
    }

    template <typename... Components>
    void ECSWorld::forEach(std::function<void(Entity, Components&...)> callback) {
        auto entities = query<Components...>();
        
        for (auto& entity : entities) {
            try {
                callback(entity, entity.get<Components>()...);
            } catch (const std::exception& e) {
                std::cerr << "[ECSWorld::forEach] Error processing entity " 
                          << entity.getAddress() << ": " << e.what() << std::endl;
            }
        }
    }

    template <typename T>
    void ECSWorld::registerSystem(const std::string& name, std::unique_ptr<T> system) {
        static_assert(std::is_base_of<ISystem, T>::value, 
                      "System must inherit from ISystem");
        
        if (_systems.find(name) != _systems.end()) {
            std::cerr << "[ECSWorld::registerSystem] Warning: System '" << name 
                      << "' already exists. Replacing." << std::endl;
            
            // Remove from order list
            auto it = std::find(_systemsOrder.begin(), _systemsOrder.end(), name);
            if (it != _systemsOrder.end()) {
                _systemsOrder.erase(it);
            }
        }
        
        _systems[name] = std::move(system);
        _systemsOrder.push_back(name);
    }

    template <typename T, typename... Args>
    void ECSWorld::createSystem(const std::string& name, Args&&... args) {
        static_assert(std::is_base_of<ISystem, T>::value, 
                      "System must inherit from ISystem");
        
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        registerSystem(name, std::move(system));
    }

    template <typename T>
    T* ECSWorld::getSystem(const std::string& name) {
        static_assert(std::is_base_of<ISystem, T>::value, 
                      "System must inherit from ISystem");
        
        auto it = _systems.find(name);
        if (it == _systems.end()) {
            return nullptr;
        }
        
        return dynamic_cast<T*>(it->second.get());
    }

}  // namespace ecs::wrapper
