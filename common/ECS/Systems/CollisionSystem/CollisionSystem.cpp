/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** CollisionSystem
*/

#include "CollisionSystem.hpp"
#include "../../Components/IComponent.hpp"

namespace ecs {
    /**
     * @brief Performs collision detection between all collidable entities.
     */
    void CollisionSystem::update(Registry &registry, [[maybe_unused]] float deltaTime) {
        auto entities = registry.getEntitiesWithMask(this->getComponentMask());
        std::vector<std::uint32_t> entitiesVec(entities.begin(), entities.end());

        for (size_t i = 0; i < entitiesVec.size(); ++i) {
            for (size_t j = i + 1; j < entitiesVec.size(); ++j) {
                auto entity1 = entitiesVec[i];
                auto entity2 = entitiesVec[j];

                auto &transform1 = registry.getComponent<Transform>(entity1);
                auto &collider1 = registry.getComponent<Collider>(entity1);
                auto &transform2 = registry.getComponent<Transform>(entity2);
                auto &collider2 = registry.getComponent<Collider>(entity2);

                if (!canCollide(collider1.getLayer(), collider1.getMask(), collider2.getLayer(),
                                collider2.getMask())) {
                    continue;
                }

                if (checkAABB(transform1.getPosition(), collider1.getSize(), collider1.getOffset(),
                              transform2.getPosition(), collider2.getSize(), collider2.getOffset())) {}
            }
        }
    }

    /**
     * @brief Performs AABB (Axis-Aligned Bounding Box) collision test.
     */
    bool CollisionSystem::checkAABB(const Transform::Vector2 &pos1, const Collider::Vector2 &size1,
                                    const Collider::Vector2 &offset1, const Transform::Vector2 &pos2,
                                    const Collider::Vector2 &size2, const Collider::Vector2 &offset2) const {

        float left1 = pos1.x + offset1.x;
        float right1 = left1 + size1.x;
        float top1 = pos1.y + offset1.y;
        float bottom1 = top1 + size1.y;

        float left2 = pos2.x + offset2.x;
        float right2 = left2 + size2.x;
        float top2 = pos2.y + offset2.y;
        float bottom2 = top2 + size2.y;

        return !(right1 < left2 || left1 > right2 || bottom1 < top2 || top1 > bottom2);
    }

    /**
     * @brief Checks if two entities can collide based on layer masks.
     */
    bool CollisionSystem::canCollide(std::uint32_t layer1, std::uint32_t mask1, std::uint32_t layer2,
                                     std::uint32_t mask2) const {
        return (mask1 & layer2) && (mask2 & layer1);
    }

    ComponentMask CollisionSystem::getComponentMask() const {
        return (1ULL << getComponentType<Transform>()) | (1ULL << getComponentType<Collider>());
    }
}  // namespace ecs
