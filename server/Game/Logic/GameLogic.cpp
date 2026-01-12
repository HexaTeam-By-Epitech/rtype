/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameLogic.cpp - Server-side game logic implementation
*/

#include "server/Game/Logic/GameLogic.hpp"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <thread>
#include "common/ECS/Components/Collider.hpp"
#include "common/ECS/Components/Enemy.hpp"
#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/LuaScript.hpp"
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
#include "server/Core/EventBus/EventBus.hpp"
#include "server/Core/ThreadPool/ThreadPool.hpp"
#include "server/Game/StateManager/GameOverState.hpp"
#include "server/Game/StateManager/InGameState.hpp"
#include "server/Game/StateManager/LobbyState.hpp"
#include "server/Scripting/LuaEngine.hpp"
#include "server/Scripting/LuaSystemAdapter.hpp"

namespace server {

    GameLogic::GameLogic(std::shared_ptr<ecs::wrapper::ECSWorld> world,
                         std::shared_ptr<ThreadPool> threadPool, std::shared_ptr<EventBus> eventBus)
        : _stateManager(std::make_shared<GameStateManager>()),
          _threadPool(threadPool),
          _eventBus(eventBus),
          _gameActive(false) {
        // Create ECSWorld if not provided
        if (!world) {
            _world = std::make_shared<ecs::wrapper::ECSWorld>();
        } else {
            _world = world;
        }

        if (_threadPool) {
            LOG_INFO("GameLogic: ThreadPool enabled for parallel execution (", _threadPool->size(),
                     " workers)");
        } else {
            LOG_DEBUG("GameLogic: Running in single-threaded mode");
        }

        // Initialize Lua scripting engine
        _luaEngine = std::make_unique<scripting::LuaEngine>("server/Scripting/scripts/");
        _luaEngine->setWorld(_world.get());
        LOG_INFO("GameLogic: Lua scripting engine initialized");
        if (_eventBus) {
            LOG_DEBUG("GameLogic: EventBus enabled for event publishing");
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
            _world->createSystem<ecs::MovementSystem>("MovementSystem");
            _world->createSystem<ecs::CollisionSystem>("CollisionSystem");
            _world->createSystem<ecs::HealthSystem>("HealthSystem");
            _world->createSystem<ecs::SpawnSystem>("SpawnSystem");
            _world->createSystem<ecs::AISystem>("AISystem");
            _world->createSystem<ecs::ProjectileSystem>("ProjectileSystem");
            _world->createSystem<ecs::BoundarySystem>("BoundarySystem");
            _world->createSystem<ecs::WeaponSystem>("WeaponSystem");

            // Create and register Lua system (executes after other systems)
            auto luaSystem = std::make_unique<scripting::LuaSystemAdapter>(_luaEngine.get(), _world.get());
            _world->registerSystem("Lua", std::move(luaSystem));
            LOG_INFO("✓ Lua system registered");

            LOG_INFO("✓ All systems registered (", _world->getSystemCount(), " systems)");
            if (_threadPool) {
                LOG_INFO("✓ Systems will execute in parallel mode (4 groups)");
            } else {
                LOG_INFO("✓ Systems will execute sequentially");
            }

            // Initialize game state manager with states
            _stateManager->registerState(0, std::make_shared<LobbyState>());
            _stateManager->registerState(1, std::make_shared<InGameState>());
            _stateManager->registerState(2, std::make_shared<GameOverState>());

            // Connect EventBus to GameStateManager so it can publish events
            if (_eventBus) {
                _stateManager->setEventBus(_eventBus);
            }

            // Start in InGame state (skip lobby for dev)

            _stateManager->changeState(1);

            LOG_INFO("✓ GameStateManager initialized with 3 states");

            // 🧪 TEST: Spawn a test enemy with Lua script

            LOG_INFO("🧪 Spawning test enemy with Lua script...");

            _world->createEntity()
                .with(ecs::Transform(600.0f, 300.0f))
                .with(ecs::Velocity(-1.0f, 0.0f, 80.0f))
                .with(ecs::Health(100, 100))
                .with(ecs::Enemy(0, 100, 0))  // type=0, score=100, pattern=0
                .with(ecs::LuaScript("test_movement.lua"));

            LOG_INFO("✓ Test enemy spawned at (600, 300) with script: test_movement.lua");

            _gameActive = true;

            LOG_INFO("✓ Initialization complete!");
            return true;
        } catch (const std::exception &e) {
            LOG_ERROR("Initialization failed: ", e.what());
            _initialized.store(false);
            return false;
        }
    }

    void GameLogic::update(float deltaTime, uint32_t currentTick) {
        if (!_gameActive) {
            return;
        }

        // 1. Process accumulated player input
        _processInput();

        if (_stateManager) {
            _stateManager->update(deltaTime);
        }

        _executeSystems(deltaTime);

        _cleanupDeadEntities();

        _checkGameOverCondition();
    }

    uint32_t GameLogic::spawnPlayer(uint32_t playerId, const std::string &playerName) {
        try {
            LOG_INFO("Spawning player: ", playerName, " (ID: ", playerId, ")");

            // Check if player already exists
            if (_playerMap.contains(playerId)) {
                LOG_ERROR("Player ", playerId, " already exists!");
                return 0;
            }

            // Create new player entity using the wrapper API
            ecs::wrapper::Entity playerEntity =
                _world->createEntity()
                    .with(ecs::Transform(_gameRules.getPlayerSpawnX(), _gameRules.getPlayerSpawnY()))
                    .with(ecs::Velocity(0.0f, 0.0f, _gameRules.getDefaultPlayerSpeed()))
                    .with(
                        ecs::Health(_gameRules.getDefaultPlayerHealth(), _gameRules.getDefaultPlayerHealth()))
                    .with(ecs::Player(0, 3, playerId))  // score=0, lives=3
                    .with(ecs::Collider(50.0f, 50.0f, 0.0f, 0.0f, 1, 0xFFFFFFFF, false))
                    .with(ecs::Weapon(_gameRules.getDefaultPlayerFireRate(), 0.0f, 0,
                                      _gameRules.getDefaultPlayerDamage()));
            ecs::Address entityAddress = playerEntity.getAddress();

            // Register player (protected by mutex for thread safety)
            {
                std::scoped_lock lock(_playerMutex);
                _playerMap[playerId] = entityAddress;
            }

            LOG_INFO("✓ Player spawned at (", _gameRules.getPlayerSpawnX(), ", ",
                     _gameRules.getPlayerSpawnY(), ") with entity ID: ", entityAddress);

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
                std::scoped_lock lock(_playerMutex);
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
        std::scoped_lock lock(_inputMutex);

        // Filter duplicates (redundant inputs)
        // If we have already processed this sequence ID or a newer one, ignore it.
        auto it = _lastProcessedSequenceId.find(playerId);
        if (it != _lastProcessedSequenceId.end()) {
            if (sequenceId <= it->second) {
                return;  // Already processed
            }
        }
        _lastProcessedSequenceId[playerId] = sequenceId;

        // Add to queue
        _pendingInput[playerId].push_back({playerId, inputX, inputY, isShooting, sequenceId});
    }

    void GameLogic::_processInput() {
        std::scoped_lock lock(_inputMutex);

        for (auto &[playerId, inputs] : _pendingInput) {
            if (inputs.empty()) {
                continue;
            }

            // SMART JITTER BUFFER
            // Target buffer size: 2-3 frames (~33-50ms buffer)
            // Strategy:
            // - If buffer > 5: Speed up (process 2 inputs) -> Catch up lag
            // - If buffer < 1: Slow down (process 0 inputs) -> Build up buffer (handled by empty check)
            // - Normal: Process 1 input

            size_t inputsToProcess = 1;
            if (inputs.size() > 5) {
                inputsToProcess = 2;  // Catch up
                LOG_DEBUG("[JITTER] Player ", playerId, " buffer high (", inputs.size(),
                          "), processing 2 inputs");
            }

            for (size_t i = 0; i < inputsToProcess && !inputs.empty(); ++i) {
                const auto &input = inputs.front();
                _applyPlayerInput(playerId, input);
                inputs.pop_front();
            }
        }
    }

    void GameLogic::_applyPlayerInput(uint32_t playerId, const PlayerInput &input) {
        auto it = _playerMap.find(playerId);
        if (it == _playerMap.end()) {
            return;
        }

        ecs::Address playerEntity = it->second;
        try {
            // Get entity wrapper and check if it still exists
            ecs::wrapper::Entity entity = _world->getEntity(playerEntity);

            // Check if entity has Velocity component (entity might be destroyed)
            if (!entity.has<ecs::Velocity>()) {
                LOG_WARNING("Player ", playerId, " entity has no Velocity component (entity destroyed?)");
                return;
            }

            ecs::Velocity &vel = entity.get<ecs::Velocity>();

            // If no input (0, 0), stop the player completely
            if (input.inputX == 0 && input.inputY == 0) {
                vel.setDirection(0.0f, 0.0f);
                // Debug log throttled (thread-local to avoid races)
                thread_local uint32_t stopLogCount = 0;
                if (++stopLogCount % 60 == 0) {
                    LOG_DEBUG("[INPUT] Player=", playerId, " STOPPED");
                }
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
                // Debug log throttled
                thread_local uint32_t moveLogCount = 0;
                if (++moveLogCount % 60 == 0) {
                    LOG_DEBUG("[INPUT] Player=", playerId, " dir=(", dirX, ", ", dirY, ")");
                }
            }

            // Handle shooting
            if (input.isShooting) {
                // Weapon system will handle actual projectile creation
            }
        } catch (const std::exception &e) {
            LOG_ERROR("Error applying input for player ", playerId, ": ", e.what());
        }
    }

    void GameLogic::_executeSystems(float deltaTime) {
        if (!_threadPool) {
            // Sequential execution (no ThreadPool)
            _world->update(deltaTime);
            return;
        }

        // Parallel execution with ThreadPool
        // Group systems by dependency - systems in the same group can run in parallel

        // Group 1: Independent systems (can run in parallel)
        std::vector<std::string> group1 = {"MovementSystem"};

        // Group 2: Depends on positions (after Movement)
        std::vector<std::string> group2 = {"CollisionSystem", "BoundarySystem"};

        // Group 3: Depends on collision results
        std::vector<std::string> group3 = {"HealthSystem", "ProjectileSystem"};

        // Group 4: AI, spawning, and scripting (can run in parallel)
        std::vector<std::string> group4 = {"AISystem", "SpawnSystem", "WeaponSystem", "Lua"};

        // Execute each group in order, but parallelize within groups
        auto executeGroup = [this, deltaTime](const std::vector<std::string> &group) {
            // Use shared_ptr to safely share the atomic counter between threads
            auto completed = std::make_shared<std::atomic<int>>(0);
            const int total = static_cast<int>(group.size());

            for (const auto &systemName : group) {
                // Capture completed by value (shared_ptr copy is thread-safe)
                _threadPool->enqueue([this, systemName, deltaTime, completed]() {
                    _world->updateSystem(systemName, deltaTime);
                    (*completed)++;
                });
            }

            // Wait for all tasks in this group to complete
            while (*completed < total) {
                std::this_thread::yield();
            }
        };

        // Execute groups sequentially, systems within groups in parallel
        executeGroup(group1);
        executeGroup(group2);
        executeGroup(group3);
        executeGroup(group4);
    }

    void GameLogic::_cleanupDeadEntities() {
        // Query all entities with Health component
        auto entities = _world->query<ecs::Health>();

        std::vector<ecs::Address> toDestroy;

        // Find entities that need to be removed
        for (auto &entity : entities) {
            ecs::Health &health = entity.get<ecs::Health>();

            if (health.getCurrentHealth() > 0) {
                continue;
            }
            ecs::Address entityAddress = entity.getAddress();
            toDestroy.push_back(entityAddress);

            // Remove from player map if it's a player entity
            if (!entity.has<ecs::Player>()) {
                continue;
            }
            std::scoped_lock lock(_playerMutex);
            for (auto it = _playerMap.begin(); it != _playerMap.end();) {
                if (it->second == entityAddress) {
                    it = _playerMap.erase(it);
                } else {
                    ++it;
                }
            }
        }

        for (ecs::Address address : toDestroy) {
            _world->destroyEntity(address);
        }
    }

    void GameLogic::_checkGameOverCondition() {
        // Only check in InGame state
        if (_stateManager->getCurrentState() != 1) {
            return;
        }

        bool allPlayersDead = true;
        {
            std::scoped_lock lock(_playerMutex);
            if (_playerMap.empty()) {
                return;  // No players, no game over
            }

            for (const auto &[playerId, entityAddress] : _playerMap) {
                try {
                    ecs::wrapper::Entity entity = _world->getEntity(entityAddress);
                    if (entity.has<ecs::Health>() && entity.get<ecs::Health>().getCurrentHealth() > 0) {
                        allPlayersDead = false;
                        break;
                    }

                } catch (...) {
                    // Entity doesn't exist, consider dead
                }
            }
        }

        if (allPlayersDead) {
            LOG_INFO("All players defeated! Changing to GameOver state...");
            _stateManager->changeState(2);  // GameOverState will publish GameEndedEvent
        }
    }

    void GameLogic::resetGame() {
        LOG_INFO("Resetting game...");

        _gameActive = true;
        _playerMap.clear();
        _pendingInput.clear();

        // Clear all entities from the world
        _world->clear();
        LOG_INFO("✓ Game reset");
    }

}  // namespace server
