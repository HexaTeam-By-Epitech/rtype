/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameLogic.cpp - Server-side game logic implementation
*/

#include "server/Game/Logic/GameLogic.hpp"
#include <algorithm>
#include <cmath>
#include "common/ECS/Components/Collider.hpp"
#include "common/ECS/Components/Enemy.hpp"
#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/Player.hpp"
#include "common/ECS/Components/Projectile.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Components/Velocity.hpp"
#include "common/ECS/Components/Weapon.hpp"
#include "common/ECS/Systems/AISystem/AISystem.hpp"
#include "common/ECS/Systems/BoundarySystem/BoundarySystem.hpp"
#include "common/ECS/Systems/CollisionSystem/CollisionSystem.hpp"
#include "common/ECS/Systems/HealthSystem/HealthSystem.hpp"
#include "common/ECS/Systems/ISystem.hpp"
#include "common/ECS/Systems/MovementSystem/MovementSystem.hpp"
#include "common/ECS/Systems/ProjectileSystem/ProjectileSystem.hpp"
#include "common/ECS/Systems/SpawnSystem/SpawnSystem.hpp"
#include "common/ECS/Systems/WeaponSystem/WeaponSystem.hpp"
#include "common/Logger/Logger.hpp"
#include "server/Core/ThreadPool/ThreadPool.hpp"

namespace server {

    GameLogic::GameLogic(std::shared_ptr<ecs::wrapper::ECSWorld> world,
                         std::shared_ptr<ThreadPool> threadPool)
        : _currentTick(0),
          _stateManager(std::make_shared<GameStateManager>()),
          _threadPool(threadPool),
          _gameActive(false) {
        // Create ECSWorld if not provided
        if (!world) {
            _world = std::make_shared<ecs::wrapper::ECSWorld>();
            LOG_DEBUG("GameLogic: Created new ECSWorld");
        } else {
            _world = world;
            LOG_DEBUG("GameLogic: Using provided ECSWorld");
        }

        if (_threadPool) {
            LOG_INFO("GameLogic: ThreadPool enabled for parallel execution (", _threadPool->size(),
                     " workers)");
        } else {
            LOG_DEBUG("GameLogic: Running in single-threaded mode");
        }

        LOG_DEBUG("GameLogic: GameStateManager initialized");
    }

    GameLogic::~GameLogic() = default;

    bool GameLogic::initialize() {
        if (_initialized.exchange(true)) {
            return true;  // Already initialized
        }

        try {
            LOG_INFO("Initializing game logic...");

            // Create and register all systems with ECSWorld in execution order
            _world->createSystem<ecs::MovementSystem>(ecs::wrapper::SystemId::Movement);
            _world->createSystem<ecs::CollisionSystem>(ecs::wrapper::SystemId::Collision);
            _world->createSystem<ecs::HealthSystem>(ecs::wrapper::SystemId::Health);
            _world->createSystem<ecs::SpawnSystem>(ecs::wrapper::SystemId::Spawn);
            _world->createSystem<ecs::AISystem>(ecs::wrapper::SystemId::AI);
            _world->createSystem<ecs::ProjectileSystem>(ecs::wrapper::SystemId::Projectile);
            _world->createSystem<ecs::BoundarySystem>(ecs::wrapper::SystemId::Boundary);
            _world->createSystem<ecs::WeaponSystem>(ecs::wrapper::SystemId::Weapon);

            LOG_INFO("✓ All systems registered (", _world->getSystemCount(), " systems)");

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

            // Create new player entity using the wrapper API
            ecs::wrapper::Entity playerEntity =
                _world->createEntity()
                    .with(ecs::Transform(PLAYER_SPAWN_X, PLAYER_SPAWN_Y))
                    .with(ecs::Velocity(0.0f, 0.0f, PLAYER_SPEED))
                    .with(ecs::Health(PLAYER_HEALTH, PLAYER_HEALTH))
                    .with(ecs::Player(0, 3, playerId))  // score=0, lives=3
                    .with(ecs::Collider(50.0f, 50.0f, 0.0f, 0.0f, 1, 0xFFFFFFFF, false))
                    .with(ecs::Weapon(10.0f, 0.0f, 0, 25));  // fireRate, cooldown, type, damage

            ecs::Address entityAddress = playerEntity.getAddress();

            // Register player (protected by mutex for thread safety)
            {
                std::lock_guard<std::mutex> lock(_playerMutex);
                _playerMap[playerId] = entityAddress;
            }

            LOG_INFO("✓ Player spawned at (", PLAYER_SPAWN_X, ", ", PLAYER_SPAWN_Y,
                     ") with entity ID: ", entityAddress);

            return entityAddress;
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to spawn player: ", e.what());
            return 0;
        }
    }

    void GameLogic::despawnPlayer(uint32_t playerId) {
        try {
            ecs::Address playerEntity = 0;

            // Find and remove player from map (protected by mutex)
            {
                std::lock_guard<std::mutex> lock(_playerMutex);
                auto it = _playerMap.find(playerId);
                if (it == _playerMap.end()) {
                    LOG_WARNING("Player ", playerId, " not found");
                    return;
                }
                playerEntity = it->second;
                _playerMap.erase(it);
            }

            LOG_INFO("Despawning player ", playerId, " (entity: ", playerEntity, ")");

            // Remove entity from the world
            _world->destroyEntity(playerEntity);
            LOG_INFO("✓ Player removed from game and entity destroyed");
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to despawn player: ", e.what());
        }
    }

    void GameLogic::processPlayerInput(uint32_t playerId, int inputX, int inputY, bool isShooting) {
        std::lock_guard<std::mutex> lock(_inputMutex);
        _pendingInput.push_back({playerId, inputX, inputY, isShooting});
    }

    void GameLogic::_processInput() {
        // Move pending input to local copy under lock to minimize lock time
        std::vector<PlayerInput> inputCopy;
        {
            std::lock_guard<std::mutex> lock(_inputMutex);
            inputCopy = std::move(_pendingInput);
            // _pendingInput is now in a valid but unspecified state (typically empty)
            // No need to explicitly clear after move
        }

        for (const auto &input : inputCopy) {
            auto it = _playerMap.find(input.playerId);
            if (it == _playerMap.end()) {
                continue;  // Player not found, skip
            }

            ecs::Address playerEntity = it->second;
            try {
                // Get entity wrapper and update velocity
                ecs::wrapper::Entity entity = _world->getEntity(playerEntity);
                ecs::Velocity &vel = entity.get<ecs::Velocity>();

                // If no input (0, 0), stop the player completely
                if (input.inputX == 0 && input.inputY == 0) {
                    vel.setDirection(0.0f, 0.0f);
                } else {
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
                }

                // Debug: log processed input once per message (only when there's movement)
                if (input.inputX != 0 || input.inputY != 0) {
                    LOG_DEBUG("Input processed | player=", input.playerId, " dir=(", input.inputX, ", ",
                              input.inputY, ")", " shooting=", (input.isShooting ? "true" : "false"));
                }

                // Handle shooting
                if (input.isShooting) {
                    // Weapon system will handle actual projectile creation
                }
            } catch (const std::exception &e) {
                LOG_ERROR("Error processing input for player ", input.playerId, ": ", e.what());
            }
        }
    }

    void GameLogic::_executeSystems(float deltaTime) {
        // Use the ECSWorld's update method which runs all systems in order
        _world->update(deltaTime);
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

        // Clear all entities from the world
        _world->clear();
        LOG_INFO("✓ Game reset");
    }

}  // namespace server
