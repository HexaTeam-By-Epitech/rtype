/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** WeaponSystem
*/

#include "WeaponSystem.hpp"
#include "../../Components/IComponent.hpp"

namespace ecs {
    /**
     * @brief Updates weapon cooldowns for all entities with weapons.
     */
    void WeaponSystem::update(Registry &registry, float deltaTime) {
        auto entities = registry.getEntitiesWithMask(this->getComponentMask());

        for (auto entityId : entities) {
            auto &weapon = registry.getComponent<Weapon>(entityId);

            float cooldown = weapon.getCooldown();
            if (cooldown > 0.0f) {
                weapon.setCooldown(cooldown - deltaTime);
            }
        }
    }

    ComponentMask WeaponSystem::getComponentMask() const {
        return (1ULL << getComponentType<Weapon>()) | (1ULL << getComponentType<Transform>());
    }
}  // namespace ecs
