/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** ComponentBindingHelper - Simplifies component registration
*/

#pragma once

#include <functional>
#include <sol/sol.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include "common/ECSWrapper/ECSWorld.hpp"

namespace scripting::bindings {

    /**
     * @struct ComponentBinding
     * @brief Information about a registered component.
     */
    struct ComponentBinding {
        std::string name;
        std::function<void(sol::state &)> bindFunc;
    };

    /**
     * @class ComponentBindingHelper
     * @brief Helper to simplify component registration.
     * 
     * Instead of modifying 3 files, you register a component once:
     * helper.add<Transform>("Transform", bindingFunc);
     * 
     * The helper automatically generates:
     * - entity:getTransform()
     * - entity:hasTransform()
     * - removeComponent(addr, "Transform")
     */
    class ComponentBindingHelper {
       public:
        /**
         * @brief Add a component to the helper.
         * 
         * @tparam T Component type
         * @param name Component name in Lua
         * @param bindFunc Function that creates the Lua usertype
         * 
         * @example
         * helper.add<Transform>("Transform", [](sol::state& lua) {
         *     auto type = lua.new_usertype<Transform>("Transform");
         *     type["x"] = &Transform::x;
         * });
         */
        template <typename T>
        void add(const std::string &name, std::function<void(sol::state &)> bindFunc);

        /**
         * @brief Register a component that's already been bound to Lua.
         * 
         * Simplified version of add() that doesn't require a binding function.
         * Use this when you've already created the Lua usertype manually.
         * 
         * @tparam T Component type
         * @param name Component name in Lua
         */
        template <typename T>
        void registerComponent(const std::string &name);

        /**
         * @brief Apply all component bindings to the Lua state.
         * @param lua Lua state
         */
        void applyComponentBindings(sol::state &lua);

        /**
         * @brief Apply get/has methods on Entity for all components.
         * 
         * Automatically generates:
         * - entity:getTransform(), entity:getVelocity(), etc.
         * - entity:hasTransform(), entity:hasVelocity(), etc.
         * 
         * @param entityType The Entity usertype
         */
        void applyEntityMethods(sol::usertype<ecs::wrapper::Entity> &entityType);

        /**
         * @brief Create the global removeComponent function for all components.
         * 
         * @param lua Lua state
         * @param world Pointer to the ECS world
         */
        void applyRemoveFunction(sol::state &lua, ecs::wrapper::ECSWorld *world);

        /**
         * @brief Get the list of registered components.
         * @return const std::vector<ComponentBinding>& List of bindings
         */
        const std::vector<ComponentBinding> &getBindings() const;

        /**
         * @brief Clear all registered components (for testing/reinitialization)
         */
        void clear();

       private:
        std::vector<ComponentBinding> _bindings;
        std::unordered_map<std::string, std::function<sol::object(ecs::wrapper::Entity &, sol::this_state)>>
            _getters;
        std::unordered_map<std::string, std::function<bool(ecs::wrapper::Entity &)>> _hasCheckers;
        std::unordered_map<std::string, std::function<void(ecs::wrapper::Entity &)>> _removers;
    };

    // Template implementation
    template <typename T>
    void ComponentBindingHelper::add(const std::string &name, std::function<void(sol::state &)> bindFunc) {
        _bindings.push_back({name, bindFunc});

        _getters[name] = [](ecs::wrapper::Entity &e, sol::this_state L) -> sol::object {
            sol::state_view lua(L);
            if (!e.has<T>()) {
                return sol::lua_nil;
            }
            return sol::make_object(lua, &e.get<T>());
        };

        _hasCheckers[name] = [](ecs::wrapper::Entity &e) -> bool {
            return e.has<T>();
        };

        _removers[name] = [](ecs::wrapper::Entity &e) {
            e.remove<T>();
        };
    }

    /**
     * @brief Register a component that's already been bound to Lua.
     * 
     * Simplified version of add() that doesn't require a binding function.
     * Use this when you've already created the Lua usertype manually.
     * 
     * @tparam T Component type
     * @param name Component name in Lua
     */
    template <typename T>
    void ComponentBindingHelper::registerComponent(const std::string &name) {
        // Store direct lambdas that sol2 can understand
        _getters[name] = [](ecs::wrapper::Entity &e, sol::this_state L) -> sol::object {
            sol::state_view lua(L);
            if (!e.has<T>()) {
                return sol::lua_nil;
            }
            return sol::make_object(lua, &e.get<T>());
        };

        // CRITICAL: Use a direct lambda that sol2 can properly bind
        // The key is to not wrap it in std::function before assigning to usertype
        _hasCheckers[name] = [](ecs::wrapper::Entity &e) -> bool {
            return e.has<T>();
        };

        _removers[name] = [](ecs::wrapper::Entity &e) {
            e.remove<T>();
        };
    }

}  // namespace scripting::bindings
