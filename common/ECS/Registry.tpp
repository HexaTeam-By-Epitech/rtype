
#pragma once
#include <iostream>
#include "Registry.hpp"

namespace ecs {

    template <typename T>
    void Registry::addEntityProp(Address address) {
        const std::type_index componentType(typeid(T));
        Signature componentSign = 0;

        if (_componentMap.contains(componentType)) {
            componentSign = _componentMap[componentType];
        } else {
            componentSign = this->_registerComponent(componentType);
        }
        if (componentSign == 0) {
            std::cerr << "CRITICAL: Components limit reached" << std::endl;
            return;
        }
        _signatures[address] |= componentSign;

        std::cout << componentType.name() << ":" << componentSign << std::endl;
    }

};  // namespace ecs
