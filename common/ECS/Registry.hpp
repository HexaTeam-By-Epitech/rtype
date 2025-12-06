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

#include <atomic>
#include <bitset>
#include <random>
#include <shared_mutex>
#include <typeindex>
#include <unordered_map>

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
 * - Generate unique random addresses for new entities.
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
     * @brief Generate a unique random Address not currently in use.
     *
     * Uses the internal random number generator and distribution
     * to produce a non-zero Address that does not already exist in
     * the _signatures map.
     *
     * @return Address a unique non-zero entity address.
     */
        Address _generateRandomAddress();

        /**
     * @brief Initialize the internal random number generator and distribution.
     *
     * Seeds the RNG (std::mt19937) and sets up the _addressGenerator
     * uniform distribution to span valid Address values.
     */
        void _initRandomizer();

        /**
         * @brief Thread-safe lock for registering entities
         */
        std::shared_mutex entityLock_;

        /**
         * @brief Thread-safe lock for registering components
         * Permits a safer implementation of ecs::getComponentType<>()
         */
        std::shared_mutex componentLock_;

        /**
     * @brief Register a component type and allocate a bit in the Signature.
     *
     * If the maximum number of components is reached, an empty (zero)
     * Signature is returned to indicate failure. On success, the returned
     * Signature contains a single bit set for the allocated component slot.
     *
     * @param componentType The std::type_index identifying the component type.
     * @return Signature a Signature with a single bit for the component or
     *         a zero signature on failure.
     */
        Signature _registerComponent(std::type_index componentType);

        /**
     * @brief Map of entity addresses to their component Signatures.
     *
     * Key: Address (entity id), Value: Signature (bitset of attached components).
     */
        std::unordered_map<Address, Signature> _signatures;

        /**
     * @brief Pseudo-random number generator used for address generation.
     */
        std::mt19937 _rng;

        /**
     * @brief Distribution used to produce random Address values.
     *
     * Produces uniformly distributed Address values in the valid range.
     */
        std::uniform_int_distribution<Address> _addressGenerator;

        /**
     * @brief Mapping of component type to the Signature bit representing that component.
     *
     * Key: std::type_index identifying the component type.
     * Value: Signature with a single bit set representing the component's position.
     */
        std::unordered_map<std::type_index, Signature> _componentMap;

       public:
        /**
     * @brief Construct a new Registry object.
     *
     * Initializes internal maps. Random generator is initialized on demand
     * by calling _initRandomizer() before address generation.
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
     * @brief Attach a component type T to an entity (set the component bit).
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
    };
}  // namespace ecs

#include "Registry.tpp"