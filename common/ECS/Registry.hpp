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
        /**
     * @brief Generate a unique Address for a new entity.
     *
     * Reuses freed addresses if available, otherwise generates
     * a new sequential address.
     *
     * @return Address a unique non-zero entity address.
     */
        Address _generateAddress();

        /**
     * @brief Register a component type and allocate a bit in the Signature.
     *
     * If the maximum number of components is reached, an empty (zero)
     * Signature is returned to indicate failure. On success, the returned
     * Signature contains a single bit set for the allocated component slot.
     *
     * @param componentType The ComponentType identifying the component type.
     * @return Signature a Signature with a single bit for the component or
     *         a zero signature on failure.
     */
        Signature _registerComponent(ComponentType componentType);

        /**
     * @brief Map of entity addresses to their component Signatures.
     *
     * Key: Address (entity id), Value: Signature (bitset of attached components).
     */
        std::unordered_map<Address, Signature> _signatures;

        /**
     * @brief Next available sequential address.
     */
        Address _nextAddress;

        /**
     * @brief Pool of freed addresses available for reuse.
     *
     * Uses a priority queue (min-heap) to reuse the smallest freed addresses first,
     * improving cache locality.
     */
        std::priority_queue<Address, std::vector<Address>, std::greater<Address>> _freeAddresses;

        /**
     * @brief Mapping of component type to the Signature bit representing that component.
     *
     * Key: ComponentType identifying the component type.
     * Value: Signature with a single bit set representing the component's position.
     */
        std::unordered_map<ComponentType, Signature> _componentMap;

        /**
     * @brief Storage for component data.
     *
     * First key: ComponentType identifying the component type.
     * Second key: Address (entity id).
     * Value: std::any containing the component data.
     */
        std::unordered_map<ComponentType, std::unordered_map<Address, std::any>> _componentStorage;

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
    };
}  // namespace ecs

#include "Registry.tpp"
