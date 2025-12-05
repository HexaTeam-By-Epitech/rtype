/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Registry
*/

#include "Registry.hpp"
#include <iostream>
#include <limits>
#include "IComponent.hpp"

namespace ecs {
    Registry::Registry() {
        _signatures = {};
        _componentMap = {};
        _initRandomizer();
        _entityLock = 0;
        _componentLock = 0;
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
        while (_componentLock)
            ;
        _componentLock = true;

        int offset = _componentMap.size();
        std::cout << "Size of map is " << offset << std::endl;
        Signature sign = 0;

        if (offset >= N_MAX_COMPONENTS)
            return sign;
        sign = 1;
        while (offset > 0) {
            sign <<= 1;
            offset -= 1;
        }
        _componentMap[componentType] = sign;

        _componentLock = false;
        return sign;
    }

    Address Registry::newEntity() {
        while (_entityLock)
            ;
        _entityLock = true;
        const Address addr = this->_generateRandomAddress();
        Signature signature;
        signature.reset();

        _signatures[addr] = signature;
        _entityLock = false;
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
