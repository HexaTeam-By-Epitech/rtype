/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** CollisionSystem
*/

#pragma once

#include "../../Components/Buff.hpp"
#include "../../Components/Collectible.hpp"
#include "../../Components/Collider.hpp"
#include "../../Components/Health.hpp"
#include "../../Components/Player.hpp"
#include "../../Components/Transform.hpp"
#include "../ISystem.hpp"

namespace ecs {
    /**
     * @class CollisionSystem
     * @brief System handling collision detection between entities.
     * 
     * Detects AABB collisions and manages layer-based filtering.
     * Requires Transform and Collider components.
     */
    class CollisionSystem : public ISystem {
       public:
        /**
         * @brief Default constructor.
         */
        CollisionSystem() = default;

        /**
         * @brief Default destructor.
         */
        ~CollisionSystem() override = default;

        /**
         * @brief Detects and handles collisions between entities.
         * 
         * Performs N² collision detection between all entities with colliders.
         * Uses AABB (Axis-Aligned Bounding Box) collision detection and
         * layer-based filtering to determine valid collisions.
         * 
         * @param registry Reference to the ECS registry
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        void update(Registry &registry, float deltaTime) override;

        /**
         * @brief Gets the component mask for this system.
         * 
         * @return ComponentMask requiring Transform and Collider components
         */
        ComponentMask getComponentMask() const override;

       private:
        /**
         * @brief Checks AABB collision between two entities.
         * 
         * Tests if two axis-aligned bounding boxes overlap.
         * 
         * @param pos1 Position of first entity
         * @param size1 Size of first collider
         * @param offset1 Offset of first collider from position
         * @param pos2 Position of second entity
         * @param size2 Size of second collider
         * @param offset2 Offset of second collider from position
         * @return true if boxes overlap, false otherwise
         */
        bool checkAABB(const Transform::Vector2 &pos1, const Collider::Vector2 &size1,
                       const Collider::Vector2 &offset1, const Transform::Vector2 &pos2,
                       const Collider::Vector2 &size2, const Collider::Vector2 &offset2) const;

        /**
         * @brief Handle collision between player and collectible
         * 
         * Applies collectible effects to the player and marks collectible for destruction.
         * 
         * @param playerAddr Player entity address
         * @param collectibleAddr Collectible entity address
         * @param registry Reference to the ECS registry
         * @param entitiesToDestroy Vector to collect entities that should be destroyed after collision processing
         */
        void handlePickup(Address playerAddr, Address collectibleAddr, Registry &registry,
                          std::vector<Address> &entitiesToDestroy);

        /**
         * @brief Checks if two entities can collide based on layers.
         * 
         * Uses bitwise operations to determine if collision layers are compatible.
         * Entity A can collide with Entity B if:
         * - A's mask includes B's layer AND
         * - B's mask includes A's layer
         * 
         * @param layer1 Collision layer of first entity
         * @param mask1 Collision mask of first entity
         * @param layer2 Collision layer of second entity
         * @param mask2 Collision mask of second entity
         * @return true if entities should collide, false otherwise
         */
        bool canCollide(std::uint32_t layer1, std::uint32_t mask1, std::uint32_t layer2,
                        std::uint32_t mask2) const;

        /**
         * @brief Resolves collision between player and wall by instantly killing the player.
         * 
         * Walls are instant death obstacles - deals massive damage to kill player on contact.
         * 
         * @param playerAddr Player entity address
         * @param wallAddr Wall entity address
         * @param playerTransform Player's transform component
         * @param playerCollider Player's collider component
         * @param wallTransform Wall's transform component
         * @param wallCollider Wall's collider component
         * @param registry Reference to the ECS registry
         */
        void resolveWallCollision(Address playerAddr, Address wallAddr, Transform &playerTransform,
                                  Collider &playerCollider, Transform &wallTransform, Collider &wallCollider,
                                  Registry &registry);

        /**
         * @brief Handle collision between orbital module and enemy.
         * 
         * Applies damage from the module to the enemy on contact.
         * 
         * @param moduleAddr Orbital module entity address
         * @param enemyAddr Enemy entity address
         * @param registry Reference to the ECS registry
         */
        void handleModuleEnemyCollision(Address moduleAddr, Address enemyAddr, Registry &registry);

        /**
         * @brief Handle collision between orbital module and projectile.
         * 
         * Blocks enemy projectiles by destroying them on contact with the module.
         * 
         * @param moduleAddr Orbital module entity address
         * @param projectileAddr Projectile entity address
         * @param registry Reference to the ECS registry
         * @param entitiesToDestroy Vector to collect entities that should be destroyed
         */
        void handleModuleProjectileCollision(Address moduleAddr, Address projectileAddr, Registry &registry,
                                             std::vector<Address> &entitiesToDestroy);

        /**
         * @brief Handle projectile collision with other entities.
         * 
         * If entity1 is a projectile and entity2 is an enemy, applies damage
         * to the enemy and marks the projectile for destruction.
         * 
         * @param registry Reference to the ECS registry
         * @param entity1 First entity (potentially a projectile)
         * @param entity2 Second entity (potentially an enemy)
         * @param projectilesToDestroy Vector to collect projectiles that should be destroyed
         */
        void handleProjectileCollision(Registry &registry, std::uint32_t entity1, std::uint32_t entity2,
                                       std::vector<std::uint32_t> &projectilesToDestroy);
    };
}  // namespace ecs
