/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameLogic.cpp - Server-side game logic implementation
*/

#include "GameLogic.hpp"
#include <algorithm>
#include <cmath>
#include "../../../common/Logger/Logger.hpp"

#include "../../common/ECS/Components/Collider.hpp"
#include "../../common/ECS/Components/Enemy.hpp"
#include "../../common/ECS/Components/Health.hpp"
#include "../../common/ECS/Components/Player.hpp"
#include "../../common/ECS/Components/Projectile.hpp"
#include "../../common/ECS/Components/Transform.hpp"
#include "../../common/ECS/Components/Velocity.hpp"
#include "../../common/ECS/Components/Weapon.hpp"

#include "../../common/ECS/Systems/AISystem/AISystem.hpp"
#include "../../common/ECS/Systems/BoundarySystem/BoundarySystem.hpp"
#include "../../common/ECS/Systems/CollisionSystem/CollisionSystem.hpp"
#include "../../common/ECS/Systems/HealthSystem/HealthSystem.hpp"
#include "../../common/ECS/Systems/ISystem.hpp"
#include "../../common/ECS/Systems/MovementSystem/MovementSystem.hpp"
#include "../../common/ECS/Systems/ProjectileSystem/ProjectileSystem.hpp"
#include "../../common/ECS/Systems/SpawnSystem/SpawnSystem.hpp"
#include "../../common/ECS/Systems/WeaponSystem/WeaponSystem.hpp"

namespace server {

    GameLogic::GameLogic() : _currentTick(0), _gameActive(false) {}

    GameLogic::~GameLogic() = default;

    bool GameLogic::initialize() {
        if (_initialized.exchange(true)) {
            return true;  // Already initialized
        }

        try {
            LOG_INFO("Initializing game logic...");

            // Create and register all systems in execution order
            _systems.push_back(std::make_unique<ecs::MovementSystem>());
            _systems.push_back(std::make_unique<ecs::CollisionSystem>());
            _systems.push_back(std::make_unique<ecs::HealthSystem>());
            _systems.push_back(std::make_unique<ecs::SpawnSystem>());
            _systems.push_back(std::make_unique<ecs::AISystem>());
            _systems.push_back(std::make_unique<ecs::ProjectileSystem>());
            _systems.push_back(std::make_unique<ecs::BoundarySystem>());
            _systems.push_back(std::make_unique<ecs::WeaponSystem>());

            LOG_INFO("✓ All systems registered (", _systems.size(), " systems)");

            _gameActive = true;
            _currentTick = 0;

            LOG_INFO("✓ Initialization complete!");
            return true;
        } catch (const std::exception &e) {
            LOG_ERROR("Initialization failed: ", e.what());
            _initialized.store(false);
            return false;
        }
    }

    void GameLogic::update(float deltaTime) {
        if (!_gameActive) {
            return;
        }

        // 1. Process accumulated player input
        _processInput();

        // Periodic tick summary (once per second at 60 FPS)
        if (_currentTick % 60 == 0) {
            LOG_DEBUG("Tick ", _currentTick, " | Players: ", _playerMap.size());
        }

        // 2. Execute all systems in order
        _executeSystems(deltaTime);

        // 3. Clean up dead entities
        _cleanupDeadEntities();

        // 4. Create game state snapshot
        _createSnapshot();

        // Increment tick
        _currentTick++;
    }

    uint32_t GameLogic::spawnPlayer(uint32_t playerId, const std::string &playerName) {
        try {
            LOG_INFO("Spawning player: ", playerName, " (ID: ", playerId, ")");

            // Check if player already exists
            if (_playerMap.find(playerId) != _playerMap.end()) {
                LOG_ERROR("Player ", playerId, " already exists!");
                return 0;
            }

            // Create new player entity
            ecs::Address playerEntity = _registry.newEntity();

            // Assign components
            _registry.setComponent(playerEntity, ecs::Transform(PLAYER_SPAWN_X, PLAYER_SPAWN_Y));
            _registry.setComponent(playerEntity, ecs::Velocity(0.0f, 0.0f, PLAYER_SPEED));
            _registry.setComponent(playerEntity, ecs::Health(PLAYER_HEALTH, PLAYER_HEALTH));
            _registry.setComponent(playerEntity, ecs::Player(0, 3, playerId));  // score=0, lives=3
            _registry.setComponent(playerEntity,
                                   ecs::Collider(50.0f, 50.0f, 0.0f, 0.0f, 1, 0xFFFFFFFF, false));
            _registry.setComponent(playerEntity,
                                   ecs::Weapon(10.0f, 0.0f, 0, 25));  // fireRate, cooldown, type, damage

            // Register player
            _playerMap[playerId] = playerEntity;

            LOG_INFO("✓ Player spawned at (", PLAYER_SPAWN_X, ", ", PLAYER_SPAWN_Y,
                     ") with entity ID: ", playerEntity);

            return playerEntity;
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to spawn player: ", e.what());
            return 0;
        }
    }

    void GameLogic::despawnPlayer(uint32_t playerId) {
        try {
            auto it = _playerMap.find(playerId);
            if (it == _playerMap.end()) {
                LOG_WARNING("Player ", playerId, " not found");
                return;
            }

            ecs::Address playerEntity = it->second;
            LOG_INFO("Despawning player ", playerId, " (entity: ", playerEntity, ")");

            // Remove player from map
            _playerMap.erase(it);

            // Entity cleanup happens in _cleanupDeadEntities()
            LOG_INFO("✓ Player removed from game");
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to despawn player: ", e.what());
        }
    }

    void GameLogic::processPlayerInput(uint32_t playerId, int inputX, int inputY, bool isShooting) {
        _pendingInput.push_back({playerId, inputX, inputY, isShooting});
    }

    void GameLogic::_processInput() {
        for (const auto &input : _pendingInput) {
            auto it = _playerMap.find(input.playerId);
            if (it == _playerMap.end()) {
                continue;  // Player not found, skip
            }

            ecs::Address playerEntity = it->second;
            try {
                // Update player velocity based on input
                ecs::Velocity &vel = _registry.getComponent<ecs::Velocity>(playerEntity);

                // Normalize diagonal movement
                float dirX = static_cast<float>(input.inputX);
                float dirY = static_cast<float>(input.inputY);

                // Normalize if diagonal
                if (dirX != 0.0f && dirY != 0.0f) {
                    float length = std::sqrt(dirX * dirX + dirY * dirY);
                    dirX /= length;
                    dirY /= length;
                }

                vel.setDirection(dirX, dirY);

                // Debug: log processed input once per message
                LOG_DEBUG("Input processed | player=", input.playerId, " dir=(", dirX, ", ", dirY, ")",
                          " shooting=", (input.isShooting ? "true" : "false"));

                // Handle shooting
                if (input.isShooting) {
                    // Weapon system will handle actual projectile creation
                }
            } catch (const std::exception &e) {
                LOG_ERROR("Error processing input for player ", input.playerId, ": ", e.what());
            }
        }

        _pendingInput.clear();
    }

    void GameLogic::_executeSystems(float deltaTime) {
        for (auto &system : _systems) {
            try {
                system->update(_registry, deltaTime);
            } catch (const std::exception &e) {
                LOG_ERROR("System update failed: ", e.what());
            }
        }
    }

    void GameLogic::_cleanupDeadEntities() {
        // TODO: Implement proper entity deletion
        // This would iterate over all entities with Health component
        // and remove those with health <= 0
    }

    void GameLogic::_createSnapshot() {
        // TODO: Implement state snapshot creation for network synchronization
        // This would create a representation of all entity states
        // for sending to clients
    }

    void GameLogic::resetGame() {
        LOG_INFO("Resetting game...");

        _currentTick = 0;
        _gameActive = true;
        _playerMap.clear();
        _pendingInput.clear();

        // TODO: Clear all entities from registry
        LOG_INFO("✓ Game reset");
    }

}  // namespace server
