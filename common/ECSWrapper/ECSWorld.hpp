/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ECSWorld - High-level wrapper for the ECS Registry
*/

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "../ECS/Registry.hpp"
#include "../ECS/Systems/ISystem.hpp"

namespace ecs::wrapper {

    enum class SystemId { Movement, Collision, Health, Spawn, AI, Projectile, Boundary, Weapon };

    std::string_view systemIdToName(SystemId id);

    /**
     * @class Entity
     * @brief High-level entity wrapper providing fluent interface.
     * 
     * Provides a clean, chainable API for entity operations.
     * Acts as a handle to an entity in the registry.
     */
    class Entity {
       private:
        Address _address;
        Registry *_registry;

       public:
        /**
         * @brief Construct an Entity wrapper.
         * 
         * @param address The entity's address in the registry
         * @param registry Pointer to the ECS registry
         */
        Entity(Address address, Registry *registry);

        /**
         * @brief Get the entity's address.
         * 
         * @return Address The entity's unique identifier
         */
        Address getAddress() const;

        /**
         * @brief Add/set a component to this entity.
         * 
         * @tparam T Component type
         * @param component The component data
         * @return Entity& Reference to this entity for chaining
         */
        template <typename T>
        Entity &with(const T &component);

        /**
         * @brief Get a component from this entity.
         * 
         * @tparam T Component type
         * @return T& Reference to the component
         * @throws std::runtime_error if component doesn't exist
         */
        template <typename T>
        T &get();

        /**
         * @brief Get a component from this entity (const version).
         * 
         * @tparam T Component type
         * @return const T& Const reference to the component
         * @throws std::runtime_error if component doesn't exist
         */
        template <typename T>
        const T &get() const;

        /**
         * @brief Check if this entity has a specific component.
         * 
         * @tparam T Component type
         * @return bool true if component exists, false otherwise
         */
        template <typename T>
        bool has() const;

        /**
         * @brief Remove a component from this entity.
         * 
         * @tparam T Component type
         * @return Entity& Reference to this entity for chaining
         */
        template <typename T>
        Entity &remove();

        /**
         * @brief Check if this entity is valid.
         * 
         * @return bool true if entity exists in registry
         */
        bool isValid() const;

        /**
         * @brief Implicit conversion to Address for compatibility.
         */
        operator Address() const;
    };

    /**
     * @class ECSWorld
     * @brief High-level ECS manager providing clean server-side API.
     * 
     * Manages the ECS registry, entities, and systems with a clean interface.
     * Designed for server-side game logic with emphasis on code clarity.
     */
    class ECSWorld {
       private:
        std::unique_ptr<Registry> _registry;
        std::unordered_map<std::string, std::unique_ptr<ISystem>> _systems;
        std::vector<std::string> _systemsOrder;

       public:
        /**
         * @brief Construct a new ECSWorld.
         */
        ECSWorld();

        /**
         * @brief Destroy the ECSWorld and cleanup resources.
         */
        ~ECSWorld();

        // Entity Management

        /**
         * @brief Create a new entity.
         * 
         * @return Entity Wrapper around the new entity
         */
        Entity createEntity();

        /**
         * @brief Create multiple entities at once.
         * 
         * @param count Number of entities to create
         * @return std::vector<Entity> Vector of created entities
         */
        std::vector<Entity> createEntities(size_t count);

        /**
         * @brief Get an entity wrapper from an address.
         * 
         * @param address The entity's address
         * @return Entity Wrapper around the entity
         */
        Entity getEntity(Address address);

        /**
         * @brief Destroy an entity and remove it from the world.
         * 
         * @param entity The entity to destroy
         */
        void destroyEntity(const Entity &entity);

        /**
         * @brief Destroy an entity by its address.
         * 
         * @param address The entity's address
         */
        void destroyEntity(Address address);

        // Component Queries

        /**
         * @brief Get all entities with specific components.
         * 
         * @tparam Components Component types to query
         * @return std::vector<Entity> Entities matching the query
         */
        template <typename... Components>
        std::vector<Entity> query();

        /**
         * @brief Iterate over entities with specific components.
         * 
         * @tparam Components Component types required
         * @param callback Function to call for each matching entity
         * 
         * @code
         * world.forEach<Transform, Velocity>([](Entity entity, Transform& t, Velocity& v) {
         *     t.position.x += v.x;
         * });
         * @endcode
         */
        template <typename... Components>
        void forEach(std::function<void(Entity, Components &...)> callback);

        // System Management

        /**
         * @brief Register a system with the world.
         * 
         * @tparam T System type (must inherit from ISystem)
         * @param name Unique name for the system
         * @param system Unique pointer to the system
         */
        template <typename T>
        void registerSystem(const std::string &name, std::unique_ptr<T> system);

        /**
         * @brief Create and register a system.
         * 
         * @tparam T System type (must inherit from ISystem)
         * @tparam Args Constructor argument types
         * @param name Unique name for the system
         * @param args Constructor arguments
         */
        template <typename T, typename... Args>
        void createSystem(const std::string &name, Args &&...args);

        /**
         * @brief Create and register a system using an enum identifier.
         *
         * @tparam T System type (must inherit from ISystem)
         * @tparam Args Constructor argument types
         * @param id Enum identifier for the system
         * @param args Constructor arguments
         */
        template <typename T, typename... Args>
        void createSystem(SystemId id, Args &&...args);

        /**
         * @brief Get a registered system by name.
         * 
         * @tparam T System type
         * @param name The system's name
         * @return T* Pointer to the system, or nullptr if not found
         */
        template <typename T>
        T *getSystem(const std::string &name);

        /**
         * @brief Get a registered system by enum identifier.
         *
         * @tparam T System type
         * @param id Enum identifier for the system
         * @return T* Pointer to the system, or nullptr if not found
         */
        template <typename T>
        T *getSystem(SystemId id);

        /**
         * @brief Remove a system from the world.
         * 
         * @param name The system's name
         */
        void removeSystem(const std::string &name);

        /**
         * @brief Remove a system using its enum identifier.
         *
         * @param id Enum identifier for the system
         */
        void removeSystem(SystemId id);

        /**
         * @brief Update all registered systems.
         * 
         * Systems are updated in registration order.
         * 
         * @param deltaTime Time elapsed since last update (in seconds)
         */
        void update(float deltaTime);

        /**
         * @brief Update a specific system by name.
         * 
         * @param name The system's name
         * @param deltaTime Time elapsed since last update (in seconds)
         * @return bool true if system was found and updated, false otherwise
         */
        bool updateSystem(const std::string &name, float deltaTime);

        /**
         * @brief Update a specific system by enum identifier.
         *
         * @param id Enum identifier for the system
         * @param deltaTime Time elapsed since last update (in seconds)
         * @return bool true if system was found and updated, false otherwise
         */
        bool updateSystem(SystemId id, float deltaTime);

        // Direct Registry Access (for advanced usage)

        /**
         * @brief Get direct access to the underlying registry.
         * 
         * @warning Use with caution. Prefer using the wrapper API.
         * @return Registry& Reference to the registry
         */
        Registry &getRegistry();

        /**
         * @brief Get const access to the underlying registry.
         * 
         * @return const Registry& Const reference to the registry
         */
        const Registry &getRegistry() const;

        /**
         * @brief World state (for accessing from Lua scripts)d
         * 0 = not running, 1 = starting event, 2 = running
         */
        bool startingEvent = false;

        /**
         * @brief Clear all entities from the world.
         * 
         * Systems are preserved.
         */
        void clear();

        /**
         * @brief Get the number of systems registered.
         * 
         * @return size_t Number of systems
         */
        size_t getSystemCount() const;
    };

}  // namespace ecs::wrapper

#include "ECSWorld.tpp"
