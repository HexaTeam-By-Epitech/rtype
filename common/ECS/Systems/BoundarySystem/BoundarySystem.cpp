/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** BoundarySystem
*/

#include "BoundarySystem.hpp"
#include <vector>
#include "../../Components/IComponent.hpp"

namespace ecs {
    BoundarySystem::BoundarySystem(int screenWidth, int screenHeight)
        : _screenWidth(screenWidth), _screenHeight(screenHeight) {}

    /**
     * @brief Checks entities against boundaries and destroys out-of-bounds entities.
     */
    void BoundarySystem::update(Registry &registry, [[maybe_unused]] float deltaTime) {
        auto entities = registry.getEntitiesWithMask(this->getComponentMask());
        std::vector<std::uint32_t> toDestroy;

        for (auto entityId : entities) {
            auto &transform = registry.getComponent<Transform>(entityId);
            auto pos = transform.getPosition();

            if (pos.x < -100 || pos.x > _screenWidth + 100 || pos.y < -100 || pos.y > _screenHeight + 100) {
                toDestroy.push_back(entityId);
            }
        }

        for (auto entityId : toDestroy) {
            registry.destroyEntity(entityId);
        }
    }

    void BoundarySystem::setScreenSize(int width, int height) {
        _screenWidth = width;
        _screenHeight = height;
    }

    ComponentMask BoundarySystem::getComponentMask() const {
        return (1ULL << getComponentType<Transform>());
    }
}  // namespace ecs
