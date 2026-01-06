/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** LuaSystem
*/

#include "LuaSystem.hpp"
#include "common/ECS/Registry.hpp"

namespace ecs {
    void LuaSystem::update(Registry &registry, float deltaTime) {
        auto entities = registry.getEntitiesWithMask(this->getComponentMask());
        std::vector<std::uint32_t> toDestroy;
        for (const auto &entity : entities) {
            continue;
            // TODO: Integrate with Lua engine to execute scripts
        }
    }

    ComponentMask LuaSystem::getComponentMask() const {
        return (1ULL << getComponentType<LuaScript>());
    }
}  // namespace ecs
