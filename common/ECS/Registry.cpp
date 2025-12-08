/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Registry
*/

#include "Registry.hpp"

#include <iostream>
#include <limits>

namespace ecs {
    Registry::Registry() {
        _signatures = {};
        _componentMap = {};
        _initRandomizer();
    }

    Registry::~Registry() {
        _signatures.clear();
        _componentMap.clear();
    }

    void Registry::_initRandomizer() {
        std::random_device rd;
        _rng = std::mt19937(rd());
        _addressGenerator = std::uniform_int_distribution<Address>(1, std::numeric_limits<Address>::max());
    }

    Address Registry::_generateRandomAddress() {
        Address addr = 0;
        do {
            addr = _addressGenerator(_rng);
        } while (!addr || _signatures.contains(addr));
        return addr;
    }

    Signature Registry::_registerComponent(const std::type_index componentType) {
        int offset = _componentMap.size();
        Signature sign = 0;

        if (_componentMap.contains(componentType))
            return sign;

        if (offset >= N_MAX_COMPONENTS) {
            return sign;
        }
        sign = 1 << offset;
        _componentMap[componentType] = sign;

        return sign;
    }

    Address Registry::newEntity() {
        const Address addr = this->_generateRandomAddress();
        Signature signature;

        _signatures[addr] = signature;
        return addr;
    }

    void Registry::destroyEntity(Address addr) {
        _signatures.erase(addr);
    }

    Signature Registry::getSignature(Address address) {
        Signature sign = 0u;
        if (_signatures.contains(address)) {
            sign = _signatures[address];
        }
        return sign;
    }
}  // namespace ecs
