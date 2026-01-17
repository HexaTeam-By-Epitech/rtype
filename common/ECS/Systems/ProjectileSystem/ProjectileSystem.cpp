/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** ProjectileSystem
*/

#include "ProjectileSystem.hpp"
#include <vector>
#include "../../Components/IComponent.hpp"
#include "../../Components/PendingDestroy.hpp"

namespace ecs {
    void ProjectileSystem::update(Registry &registry, float deltaTime) {
        auto entities = registry.getEntitiesWithMask(this->getComponentMask());
        std::vector<std::uint32_t> toDestroy;

        for (auto entityId : entities) {
            auto &projectile = registry.getComponent<Projectile>(entityId);

            float newLifetime = projectile.getLifetime() - deltaTime;
            projectile.setLifetime(newLifetime);

            if (newLifetime <= 0.0f) {
                toDestroy.push_back(entityId);
            }
        }

        for (auto entityId : toDestroy) {
            // Mark for destruction with proper client notification
            if (!registry.hasComponent<PendingDestroy>(entityId)) {
                registry.setComponent<PendingDestroy>(entityId, PendingDestroy(DestroyReason::Expired));
            }
        }
    }

    ComponentMask ProjectileSystem::getComponentMask() const {
        return (1ULL << getComponentType<Projectile>());
    }
}  // namespace ecs
