/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** ComponentBindingHelper implementation
*/

#include "ComponentBindingHelper.hpp"

namespace scripting::bindings {

    void ComponentBindingHelper::applyComponentBindings(sol::state &lua) {
        for (const auto &binding : _bindings) {
            binding.bindFunc(lua);
        }
    }

    void ComponentBindingHelper::applyEntityMethods(sol::usertype<ecs::wrapper::Entity> &entityType) {
        for (const auto &[name, getter] : _getters) {
            std::string getMethodName = "get" + name;
            entityType[getMethodName] = getter;
        }

        for (const auto &[name, checker] : _hasCheckers) {
            std::string hasMethodName = "has" + name;
            entityType[hasMethodName] = checker;
        }
    }

    void ComponentBindingHelper::applyRemoveFunction(sol::state &lua, ecs::wrapper::ECSWorld *world) {
        lua.set_function("removeComponent",
                         [this, world](ecs::Address addr, const std::string &componentName) {
                             auto it = _removers.find(componentName);
                             if (it != _removers.end()) {
                                 auto entity = world->getEntity(addr);
                                 if (entity.isValid()) {
                                     it->second(entity);
                                 }
                             }
                         });
    }

    const std::vector<ComponentBinding> &ComponentBindingHelper::getBindings() const {
        return _bindings;
    }

    void ComponentBindingHelper::clear() {
        _bindings.clear();
        _getters.clear();
        _hasCheckers.clear();
        _removers.clear();
    }

}  // namespace scripting::bindings
