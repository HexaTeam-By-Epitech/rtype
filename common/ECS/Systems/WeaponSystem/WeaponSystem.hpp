/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** WeaponSystem
*/

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include "../../Components/Buff.hpp"
#include "../../Components/Projectile.hpp"
#include "../../Components/Transform.hpp"
#include "../../Components/Velocity.hpp"
#include "../../Components/Weapon.hpp"
#include "../ISystem.hpp"

namespace ecs {
    /**
     * @class WeaponSystem
     * @brief System managing weapon firing and cooldowns.
     * 
     * Handles weapon cooldown timers, projectile spawning, and weapon mechanics.
     * Requires Weapon and Transform components.
     * Optionally uses Velocity for weapon direction.
     */
    class WeaponSystem : public ISystem {
       public:
        /**
         * @brief Callback for projectile creation events
         * Parameters: projectileId, ownerId, posX, posY, dirX, dirY, speed, damage, friendly
         */
        using ProjectileCreatedCallback =
            std::function<void(uint32_t, uint32_t, float, float, float, float, float, int, bool)>;

        /**
         * @brief Default constructor.
         */
        WeaponSystem() = default;

        /**
         * @brief Constructor with callback for projectile creation events.
         * 
         * @param callback Function to call when a projectile is created
         */
        explicit WeaponSystem(ProjectileCreatedCallback callback) : _projectileCreatedCallback(callback) {}

        /**
         * @brief Default destructor.
         */
        ~WeaponSystem() override = default;

        /**
         * @brief Set the callback for projectile creation events.
         * 
         * @param callback Function to call when a projectile is created
         */
        void setProjectileCreatedCallback(ProjectileCreatedCallback callback) {
            _projectileCreatedCallback = callback;
        }

        /**
         * @brief Manages weapon cooldowns for all entities.
         * 
         * Decrements weapon cooldown timers each frame.
         * When cooldown reaches 0, the weapon is ready to fire.
         * Actual projectile creation is handled by input or AI systems.
         * 
         * @param registry Reference to the ECS registry
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        void update(Registry &registry, float deltaTime) override;

        /**
         * @brief Fire a weapon from an entity, spawning a projectile.
         * 
         * Creates a new projectile entity with appropriate components.
         * Resets the weapon cooldown timer based on fire rate.
         * 
         * @param registry Reference to the ECS registry
         * @param ownerId Entity ID of the weapon owner
         * @param isFriendly Whether this is a friendly projectile
         * @return Address ID of the spawned projectile entity
         */
        std::uint32_t fireWeapon(Registry &registry, std::uint32_t ownerId, bool isFriendly);

        /**
         * @brief Fire a charged shot if charge is sufficient, otherwise fire normal shot.
         * 
         * If the provided charge level meets the minimum threshold (0.5f or 50%),
         * fires a charged projectile with increased damage and speed.
         * Otherwise, fires a normal projectile.
         * 
         * @param registry Reference to the ECS registry
         * @param ownerId Entity ID of the weapon owner
         * @param chargeLevel Current charge level (0.0f to 1.0f)
         * @param isFriendly Whether this is a friendly projectile
         * @return Address ID of the spawned projectile entity
         */
        std::uint32_t fireChargedShot(Registry &registry, std::uint32_t ownerId, float chargeLevel,
                                      bool isFriendly);

        /**
         * @brief Gets the component mask for this system.
         * 
         * @return ComponentMask requiring Weapon and Transform components
         */
        ComponentMask getComponentMask() const override;

       private:
        /**
         * @brief Calculate projectile initial velocity based on owner's velocity.
         * 
         * @param baseSpeed Base projectile speed from weapon
         * @return Velocity component for the projectile
         */
        Velocity calculateProjectileVelocity(float baseSpeed);

        /**
         * @brief Calculate projectile spawn position based on owner position.
         * 
         * @param registry Reference to the ECS registry
         * @param ownerId Entity ID of the weapon owner
         * @return Transform component for the projectile initial position
         */
        Transform calculateProjectileTransform(Registry &registry, std::uint32_t ownerId);

        /**
         * @brief Create a single projectile with specified properties.
         * 
         * @param registry Reference to the ECS registry
         * @param ownerId Entity ID of the weapon owner
         * @param transform Projectile spawn position
         * @param velocity Projectile velocity
         * @param damage Projectile damage
         * @param isFriendly Whether this is a friendly projectile
         * @param isCharged Whether this is a charged shot (affects visuals)
         * @return Address ID of the spawned projectile entity
         */
        std::uint32_t createProjectile(Registry &registry, std::uint32_t ownerId, const Transform &transform,
                                       const Velocity &velocity, float damage, bool isFriendly,
                                       bool isCharged);

        /**
         * @brief Fire multiple projectiles based on multishot buff.
         * 
         * @param registry Reference to the ECS registry
         * @param ownerId Entity ID of the weapon owner
         * @param damage Projectile damage
         * @param speed Projectile speed
         * @param isFriendly Whether projectiles are friendly
         * @param isCharged Whether this is a charged shot
         * @param shotCount Number of projectiles to fire
         */
        void fireMultipleShots(Registry &registry, std::uint32_t ownerId, float damage, float speed,
                               bool isFriendly, bool isCharged, int shotCount);

        ProjectileCreatedCallback _projectileCreatedCallback;
    };
}  // namespace ecs
