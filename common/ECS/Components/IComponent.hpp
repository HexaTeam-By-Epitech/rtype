/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** IComponent
*/

#pragma once

#include <atomic>
#include <cstddef>

namespace ecs {
    /**
     * @brief Type alias for component identification.
     * 
     * Used to uniquely identify each component type in the ECS system.
     */
    using ComponentType = std::size_t;

    /**
     * @class IComponent
     * @brief Base interface for all ECS components.
     * 
     * All components in the Entity-Component-System must inherit from this interface.
     * Each component type is assigned a unique ID for identification and bitmask operations.
     * 
     * @note Components should be data-only structures (POD when possible).
     * @note Logic should be implemented in Systems, not in Components.
     */
    class IComponent {
       public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IComponent() = default;

        /**
         * @brief Get the unique type identifier for this component.
         * @return ComponentType The unique ID of this component type.
         */
        virtual ComponentType getType() const = 0;
    };

    /**
     * @brief Generates a unique ID for each component type.
     * 
     * This function uses a static counter to ensure each call returns
     * a different ID. It's used internally by getComponentType<T>().
     * 
     * @return ComponentType A new unique component type ID.
     * @note This function is thread-safe.
     */
    inline ComponentType getUniqueComponentType() {
        static std::atomic<ComponentType> lastID{0};
        return lastID.fetch_add(1);
    }

    /**
     * @brief Get the unique type ID for a specific component type.
     * 
     * Template function that returns a unique ID for each component type T.
     * The same type will always return the same ID (singleton pattern).
     * 
     * @tparam T The component type (must inherit from IComponent).
     * @return ComponentType The unique ID for type T.
     * 
     * @code
     * auto transformID = getComponentType<Transform>();
     * auto velocityID = getComponentType<Velocity>();
     * // transformID != velocityID
     * @endcode
     */
    template <typename T>
    ComponentType getComponentType() {
        static ComponentType typeID = getUniqueComponentType();
        return typeID;
    }
}  // namespace ecs
