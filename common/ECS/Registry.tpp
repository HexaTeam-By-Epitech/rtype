
#pragma once
#include <iostream>
#include <mutex>

namespace ecs {
    template <typename T>
    void Registry::addEntityProp(Address address) {
        std::unique_lock lock(entityLock_);
        const std::type_index componentType(typeid(T));
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
        if (_signatures.contains(address)) {
            _signatures[address] |= componentSign;
        } else {
            std::cerr
                << "[ecs::Registry::addEntityProp] Warning: given address does not exists in the Registery ("
                << address << ")" << std::endl;
        }
    }

};  // namespace ecs
