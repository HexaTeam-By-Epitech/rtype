/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** HealthSystem
*/

#include "HealthSystem.hpp"
#include "../../Components/IComponent.hpp"

namespace ecs {
    /**
     * @brief Updates health states and processes entity deaths.
     */
    void HealthSystem::update(Registry &registry, float deltaTime) {
        auto entities = registry.getEntitiesWithMask(this->getComponentMask());
        std::vector<std::uint32_t> toDestroy;

        for (auto entityId : entities) {
            auto &health = registry.getComponent<Health>(entityId);

            if (health.isInvincible()) {
                int timer = health.getInvincibilityTimer() - 1;
                health.setInvincibilityTimer(timer);

                if (timer <= 0) {
                    health.setInvincible(false);
                    health.setInvincibilityTimer(0);
                }
            }

            if (health.getCurrentHealth() <= 0) {
                toDestroy.push_back(entityId);
            }
        }

        for (auto entityId : toDestroy) {
            registry.destroyEntity(entityId);
        }
    }

    ComponentMask HealthSystem::getComponentMask() const {
        return (1ULL << getComponentType<Health>());
    }
}
