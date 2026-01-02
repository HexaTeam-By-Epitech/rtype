/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Registry
*/

/**
* @file Registry.hpp
* @brief Registry for the ECS (Entity-Component System).
*
* Provides facilities to create/destroy entities, assign components
* to entities via signatures (bitsets), and register component types.
*/

#pragma once

#include <any>
#include <bitset>
#include <queue>
#include <unordered_map>
#include <vector>

#include "Components/IComponent.hpp"

#define N_MAX_COMPONENTS 32
/**
 * @brief Maximum number of distinct component types supported by the Registry.
 *
 * This value determines the size of the Signature bitset. Each registered
 * component type is assigned a unique bit position in the Signature.
 */

namespace ecs {

    /**
 * @brief Bitset representing the set of components attached to an entity.
 *
 * Each bit corresponds to a component type; a set bit indicates the presence
 * of that component on an entity. The number of bits is N_MAX_COMPONENTS.
 */
    typedef std::bitset<N_MAX_COMPONENTS> Signature;

    /**
 * @brief Type used to represent an entity address/ID.
 *
 * Addresses are 32-bit unsigned integers (non-zero).
 */
    typedef uint32_t Address;

    /**
 * @class Registry
 * @brief Manages entities, their signatures and component type registrations.
 *
 * Responsibilities:
 * - Generate unique sequential addresses for new entities, reusing freed addresses when possible.
 * - Maintain a mapping from Address -> Signature (which components an entity has).
 * - Maintain a mapping from component type (std::type_index) -> Signature bit.
 *
 * Notes:
 * - The number of distinct component types is limited by N_MAX_COMPONENTS.
 * - Signatures are implemented as std::bitset and each registered component
 *   occupies a single bit.
 */
    class Registry {

       private:
        std::unordered_map<Address, Signature> _signatures;

        Address _nextAddress;

        Address _generateAddress();

        Signature _registerComponent(ComponentType componentType);

        std::priority_queue<Address, std::vector<Address>, std::greater<Address>> _freeAddresses = {};
        std::unordered_map<ComponentType, Signature> _componentMap = {};
        std::unordered_map<ComponentType, std::unordered_map<Address, std::any>> _componentStorage = {};

       public:
        /**
     * @brief Construct a new Registry object.
     *
     * Initializes internal maps. Addresses are generated sequentially and
     * freed addresses are reused from a pool.
     */
        Registry();

        /**
     * @brief Destroy the Registry object and clear internal containers.
     */
        ~Registry();

        /**
     * @brief Create and register a new entity, returning its Address.
     *
     * A new unique Address is generated and an empty Signature (no components)
     * is associated with it.
     *
     * @return Address the new entity's address.
     */
        Address newEntity();

        /**
     * @brief Set/add a component to an entity with its data.
     *
     * If the component type is not yet registered, it will be registered automatically.
     * Updates the entity's signature and stores the component data.
     *
     * @tparam T The component type to set.
     * @param address The entity Address.
     * @param component The component data to store.
     * @throws std::runtime_error if component limit is reached or entity doesn't exist.
     */
        template <typename T>
        void setComponent(Address address, const T &component);

        /**
     * @brief Get a component from an entity.
     *
     * @tparam T The component type to retrieve.
     * @param address The entity Address.
     * @return T& Reference to the component data.
     * @throws std::runtime_error if entity doesn't have the component.
     */
        template <typename T>
        T &getComponent(Address address);

        /**
     * @brief Check if an entity has a specific component.
     *
     * @tparam T The component type to check.
     * @param address The entity Address.
     * @return bool true if the entity has the component, false otherwise.
     */
        template <typename T>
        bool hasComponent(Address address);

        /**
     * @brief Remove a component from an entity.
     *
     * Removes the component data and updates the entity's signature.
     *
     * @tparam T The component type to remove.
     * @param address The entity Address.
     */
        template <typename T>
        void removeComponent(Address address);

        /**
     * @brief Attach a component type T to an entity (set the component bit).
     *
     * @deprecated Use setComponent() instead to store component data.
     * This method only sets the signature bit without storing data.
     *
     * If the component type T is not yet registered, it will be registered
     * automatically. If the component registration fails (component limit reached),
     * no change is made.
     *
     * @tparam T The component type to add.
     * @param address The entity Address to which the component is added.
     */
        template <typename T>
        void addEntityProp(Address address);

        /**
     * @brief Remove an entity and its Signature from the registry.
     *
     * @param address The Address of the entity to destroy.
     */
        void destroyEntity(Address address);

        /**
     * @brief Retrieve the Signature for a given entity address.
     *
     * If the address is not present, a zero (empty) Signature is returned.
     *
     * @param address The entity Address to query.
     * @return Signature The Signature of the entity or zero if not found.
     */
        Signature getSignature(Address address);

        /**
     * @brief Get all entities that have a specific set of components.
     *
     * Returns a vector of entity addresses that possess all the specified
     * component types. This allows efficient iteration over entities matching
     * a specific archetype.
     *
     * @tparam Components The component types to filter by.
     * @return std::vector<Address> Vector of entity addresses with all specified components.
     *
     * @code
     * // Get all entities with both Transform and Velocity
     * auto entities = registry.view<Transform, Velocity>();
     * for (auto entity : entities) {
     *     auto& transform = registry.getComponent<Transform>(entity);
     *     auto& velocity = registry.getComponent<Velocity>(entity);
     *     // process...
     * }
     * @endcode
     */
        template <typename... Components>
        std::vector<Address> view();

        /**
     * @brief Get all entities matching a specific component mask.
     *
     * Returns a vector of entity addresses that have all the components
     * specified by the bitmask. This is the low-level filtering method
     * used by systems for efficient entity queries.
     *
     * @param requiredMask Bitmask of required components (from ComponentMask)
     * @return std::vector<Address> Vector of entity addresses matching the mask.
     *
     * @code
     * // Get entities matching a specific mask
     * ComponentMask mask = (1ULL << getComponentType<Transform>()) | 
     *                      (1ULL << getComponentType<Velocity>());
     * auto entities = registry.getEntitiesWithMask(mask);
     * @endcode
     */
        std::vector<Address> getEntitiesWithMask(Signature requiredMask);
    };
}  // namespace ecs

#include "Registry.tpp"
