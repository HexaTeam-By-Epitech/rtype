/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Registry
*/

#include "Registry.hpp"

namespace ecs {
    Registry::Registry() : _nextAddress(1) {
        _signatures = {};
        _componentMap = {};
        _componentStorage = {};
    }

    Registry::~Registry() {
        _signatures.clear();
        _componentMap.clear();
        _componentStorage.clear();
    }

    Address Registry::_generateAddress() {
        // Reuse a freed address if available
        if (!_freeAddresses.empty()) {
            Address addr = _freeAddresses.top();
            _freeAddresses.pop();
            return addr;
        }

        // Otherwise, generate a new sequential address
        return _nextAddress++;
    }

    Signature Registry::_registerComponent(const ComponentType componentType) {
        int offset = _componentMap.size();
        Signature sign = 0;

        if (_componentMap.contains(componentType))
            return sign;

        if (offset >= N_MAX_COMPONENTS) {
            return sign;
        }
        sign = 1ULL << offset;
        _componentMap[componentType] = sign;

        return sign;
    }

    Address Registry::newEntity() {
        const Address addr = this->_generateAddress();
        Signature signature;

        _signatures[addr] = signature;
        return addr;
    }

    void Registry::destroyEntity(Address addr) {
        // Remove from signatures
        _signatures.erase(addr);

        // Remove all components for this entity
        for (auto &[componentType, storage] : _componentStorage) {
            storage.erase(addr);
        }

        // Add address to the pool for reuse
        _freeAddresses.push(addr);
    }

    Signature Registry::getSignature(Address address) {
        Signature sign = 0u;
        if (_signatures.contains(address)) {
            sign = _signatures[address];
        }
        return sign;
    }

    std::vector<Address> Registry::getEntitiesWithMask(Signature requiredMask) {
        std::vector<Address> result;

        if (requiredMask == 0) {
            return result;
        }

        for (const auto &[address, signature] : _signatures) {
            if ((signature & requiredMask) == requiredMask) {
                result.push_back(address);
            }
        }

        return result;
    }
}  // namespace ecs
