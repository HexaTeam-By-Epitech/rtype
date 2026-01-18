
#pragma once
#include <iostream>
#include <stdexcept>
#include <string>
#include <mutex>

namespace ecs {
    template <typename T>
    void Registry::setComponent(Address address, const T &component) {
        std::unique_lock lock(_mutex);
        const ComponentType componentType = getComponentType<T>();
        Signature componentSign = 0;

        if (_componentMap.contains(componentType)) {
            componentSign = _componentMap[componentType];
        } else {
            componentSign = this->_registerComponent(componentType);
        }
        if (componentSign == 0) {
            const std::string errorMsg =
                "[ecs::Registry::setComponent] CRITICAL: Components limit reached (" +
                std::to_string(N_MAX_COMPONENTS) + ")";
            std::cerr << errorMsg << std::endl;
            throw std::runtime_error(errorMsg);
        }
        if (!_signatures.contains(address)) {
            const std::string errorMsg =
                "[ecs::Registry::setComponent] ERROR: Entity address does not exist (" +
                std::to_string(address) + ")";
            std::cerr << errorMsg << std::endl;
            throw std::runtime_error(errorMsg);
        }

        // Update signature
        _signatures[address] |= componentSign;

        // Store component data
        _componentStorage[componentType][address] = component;
    }

    template <typename T>
    T &Registry::getComponent(Address address) {
        std::shared_lock lock(_mutex);
        const ComponentType componentType = getComponentType<T>();

        if (!_componentStorage.contains(componentType) ||
            !_componentStorage[componentType].contains(address)) {
            const std::string errorMsg = "[ecs::Registry::getComponent] ERROR: Entity " +
                                         std::to_string(address) + " does not have component type " +
                                         std::to_string(componentType);
            std::cerr << errorMsg << std::endl;
            throw std::runtime_error(errorMsg);
        }

        return std::any_cast<T &>(_componentStorage[componentType][address]);
    }

    template <typename T>
    bool Registry::hasComponent(Address address) {
        std::shared_lock lock(_mutex);
        const ComponentType componentType = getComponentType<T>();

        if (!_componentMap.contains(componentType)) {
            return false;
        }

        Signature componentSign = _componentMap[componentType];
        if (!_signatures.contains(address)) {
            return false;
        }

        return (_signatures[address] & componentSign) == componentSign;
    }

    template <typename T>
    void Registry::removeComponent(Address address) {
        std::unique_lock lock(_mutex);
        const ComponentType componentType = getComponentType<T>();

        if (!_componentMap.contains(componentType)) {
            return;
        }

        // Remove from signature
        Signature componentSign = _componentMap[componentType];
        if (_signatures.contains(address)) {
            _signatures[address] &= ~componentSign;
        }

        // Remove component data
        if (_componentStorage.contains(componentType)) {
            _componentStorage[componentType].erase(address);
        }
    }

    template <typename T>
    void Registry::addEntityProp(Address address) {
        std::unique_lock lock(_mutex);
        const ComponentType componentType = getComponentType<T>();
        Signature componentSign = 0;

        if (_componentMap.contains(componentType)) {
            componentSign = _componentMap[componentType];
        } else {
            componentSign = this->_registerComponent(componentType);
        }
        if (componentSign == 0) {
            const std::string errorMsg =
                "[ecs::Registry::addEntityProp] CRITICAL: Components limit reached (" +
                std::to_string(N_MAX_COMPONENTS) + ")";
            std::cerr << errorMsg << std::endl;
            throw std::runtime_error(errorMsg);
        }
        if (!_signatures.contains(address)) {
            return;
        }

        _signatures[address] |= componentSign;
    }

    template <typename... Components>
    std::vector<Address> Registry::view() {
        std::shared_lock lock(_mutex);
        std::vector<Address> result;

        // Build the required signature (bitwise OR of all component signatures)
        Signature requiredSignature = 0;
        (
            [&]() {
                ComponentType componentType = getComponentType<Components>();
                if (_componentMap.contains(componentType)) {
                    requiredSignature |= _componentMap[componentType];
                }
            }(),
            ...);

        // If no components registered, return empty
        if (requiredSignature == 0) {
            return result;
        }

        // Iterate through all entities and check if they match the signature
        for (const auto &[address, signature] : _signatures) {
            if ((signature & requiredSignature) == requiredSignature) {
                result.push_back(address);
            }
        }

        return result;
    }

};  // namespace ecs
