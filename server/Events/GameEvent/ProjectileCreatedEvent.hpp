/*
** EPITECH PROJECT, 2025
** Created by hugo on 15/01/2026
** File description:
** ProjectileCreatedEvent.hpp
*/

#pragma once

#include <cstdint>
#include "server/Events/GameEvent/GameEvent.hpp"

namespace server {

    /**
     * @class ProjectileCreatedEvent
     * @brief Event triggered when a new projectile is created in the game
     */
    class ProjectileCreatedEvent : public GameEvent {
       public:
        /**
         * @brief Constructor
         * @param projectileId Entity ID of the created projectile
         * @param ownerId Entity ID of the projectile owner (shooter)
         * @param posX Starting X position
         * @param posY Starting Y position
         * @param dirX Direction X component
         * @param dirY Direction Y component
         * @param speed Movement speed
         * @param damage Damage value
         * @param friendly True for player projectile, false for enemy
         */
        explicit ProjectileCreatedEvent(uint32_t projectileId = 0, uint32_t ownerId = 0, float posX = 0.0f,
                                        float posY = 0.0f, float dirX = 0.0f, float dirY = 0.0f,
                                        float speed = 0.0f, int damage = 0, bool friendly = true)
            : GameEvent(Type::PROJECTILE_CREATED),
              _projectileId(projectileId),
              _ownerId(ownerId),
              _posX(posX),
              _posY(posY),
              _dirX(dirX),
              _dirY(dirY),
              _speed(speed),
              _damage(damage),
              _friendly(friendly) {}

        ~ProjectileCreatedEvent() override = default;

        /**
         * @brief Get the projectile entity ID
         * @return Projectile entity ID
         */
        [[nodiscard]] uint32_t getProjectileId() const { return _projectileId; }

        /**
         * @brief Get the owner entity ID
         * @return Owner entity ID
         */
        [[nodiscard]] uint32_t getOwnerId() const { return _ownerId; }

        /**
         * @brief Get the starting X position
         * @return X position
         */
        [[nodiscard]] float getPosX() const { return _posX; }

        /**
         * @brief Get the starting Y position
         * @return Y position
         */
        [[nodiscard]] float getPosY() const { return _posY; }

        /**
         * @brief Get the direction X component
         * @return Direction X
         */
        [[nodiscard]] float getDirX() const { return _dirX; }

        /**
         * @brief Get the direction Y component
         * @return Direction Y
         */
        [[nodiscard]] float getDirY() const { return _dirY; }

        /**
         * @brief Get the projectile speed
         * @return Speed value
         */
        [[nodiscard]] float getSpeed() const { return _speed; }

        /**
         * @brief Get the projectile damage
         * @return Damage value
         */
        [[nodiscard]] int getDamage() const { return _damage; }

        /**
         * @brief Check if projectile is friendly
         * @return True if friendly (player), false if enemy
         */
        [[nodiscard]] bool isFriendly() const { return _friendly; }

       private:
        uint32_t _projectileId;
        uint32_t _ownerId;
        float _posX;
        float _posY;
        float _dirX;
        float _dirY;
        float _speed;
        int _damage;
        bool _friendly;
    };

}  // namespace server
