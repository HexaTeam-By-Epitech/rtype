/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** AISystem
*/

#include "AISystem.hpp"
#include <cmath>
#include "../../Components/IComponent.hpp"

namespace ecs {
    /**
     * @brief Updates all enemy AI behaviors for the current frame.
     */
    void AISystem::update(Registry &registry, float deltaTime) {
        auto entities = registry.getEntitiesWithMask(this->getComponentMask());

        for (auto entityId : entities) {
            auto &enemy = registry.getComponent<Enemy>(entityId);
            auto &transform = registry.getComponent<Transform>(entityId);
            auto &velocity = registry.getComponent<Velocity>(entityId);

            applyMovementPattern(enemy, transform, velocity, deltaTime);
        }
    }

    /**
     * @brief Applies specific movement patterns based on enemy type.
     */
    void AISystem::applyMovementPattern(const Enemy &enemy, Transform &transform, Velocity &velocity,
                                        float deltaTime) {
        int pattern = enemy.getAttackPattern();
        auto pos = transform.getPosition();

        // TODO: Implement different movement patterns based on enemy type
    }

    ComponentMask AISystem::getComponentMask() const {
        return (1ULL << getComponentType<Enemy>()) | (1ULL << getComponentType<Transform>()) |
               (1ULL << getComponentType<Velocity>());
    }
}  // namespace ecs
