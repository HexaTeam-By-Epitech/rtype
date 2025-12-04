/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** ProjectileSystem
*/

#include "ProjectileSystem.hpp"
#include "../../Components/IComponent.hpp"
#include <vector>

namespace ecs {
    void ProjectileSystem::update(Registry &registry, float deltaTime) {
        auto entities = registry.getEntitiesWithMask(this->getComponentMask());
        std::vector<std::uint32_t> toDestroy;

        for (auto entityId : entities) {
            auto &projectile = registry.getComponent<Projectile>(entityId);

            int newLifetime = projectile.getLifetime() - 1;
            projectile.setLifetime(newLifetime);

            if (newLifetime <= 0) {
                toDestroy.push_back(entityId);
            }
        }

        for (auto entityId : toDestroy) {
            registry.destroyEntity(entityId);
        }
    }

    ComponentMask ProjectileSystem::getComponentMask() const {
        return (1ULL << getComponentType<Projectile>());
    }
}
