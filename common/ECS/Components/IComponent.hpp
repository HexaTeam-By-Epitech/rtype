/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** IComponent
*/

#ifndef ICOMPONENT_HPP_
#define ICOMPONENT_HPP_

#include <cstddef>

namespace ecs {
    using ComponentType = std::size_t;

    class IComponent {
       public:
        virtual ~IComponent() = default;

        virtual ComponentType getType() const = 0;
    };

    // Unique ID generator for each component type
    inline ComponentType getUniqueComponentType() {
        static ComponentType lastID = 0;
        return lastID++;
    }

    // Template to get the unique ID for a specific component type T
    template <typename T>
    ComponentType getComponentType() {
        static ComponentType typeID = getUniqueComponentType();
        return typeID;
    }
}

#endif
