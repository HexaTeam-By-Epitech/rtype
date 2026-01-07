/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** Room.cpp
*/

#include "server/Rooms/Room.hpp"
#include <algorithm>
#include "common/ECSWrapper/ECSWorld.hpp"
#include "common/Logger/Logger.hpp"
#include "server/Core/EventBus/EventBus.hpp"
#include "server/Core/ServerLoop/ServerLoop.hpp"
#include "server/Core/ThreadPool/ThreadPool.hpp"
#include "server/Game/Logic/GameLogic.hpp"
#include "server/Game/Logic/IGameLogic.hpp"

namespace server {

    Room::Room(const std::string &id, const std::string &name, size_t maxPlayers, bool isPrivate)
        : _id(id),
          _name(name.empty() ? id : name),
          _state(RoomState::WAITING),
          _maxPlayers(maxPlayers),
          _isPrivate(isPrivate),
          _hostPlayerId(0),
          _startCountdown(0.0f),
          _gameStartSent(false) {

        // Create dedicated EventBus for this room
        _eventBus = std::make_shared<EventBus>();

        // Create dedicated ECSWorld for this room
        std::shared_ptr<ecs::wrapper::ECSWorld> ecsWorld = std::make_shared<ecs::wrapper::ECSWorld>();

        // Create ThreadPool for parallel system execution
        std::shared_ptr<ThreadPool> threadPool = std::make_shared<ThreadPool>(4);
        threadPool->start();

        // Create GameLogic for this room (as unique_ptr since ServerLoop requires it)
        std::unique_ptr<IGameLogic> gameLogic = std::make_unique<GameLogic>(ecsWorld, threadPool, _eventBus);

        // Create ServerLoop with dedicated GameLogic
        _gameLoop = std::make_unique<ServerLoop>(std::move(gameLogic), _eventBus);

        // Initialize game loop
        if (!_gameLoop->initialize()) {
            LOG_ERROR("Failed to initialize game loop for room ", _id);
            return;
        }

        // Start the game loop thread
        _gameLoop->start();

        // Get the GameLogic back from ServerLoop for external access
        _gameLogic = std::shared_ptr<IGameLogic>(&_gameLoop->getGameLogic(), [](IGameLogic *) {
            // Empty deleter - ServerLoop owns the IGameLogic
        });

        LOG_INFO("Room '", _name, "' (", _id, ") created [State: WAITING, Max: ", _maxPlayers,
                 " players, Private: ", (_isPrivate ? "Yes" : "No"), "] with dedicated GameLoop");
    }

    bool Room::join(uint32_t playerId) {
        std::lock_guard<std::mutex> lock(_mutex);

        // Check if room is full
        if (isFull()) {
            LOG_WARNING("Player ", playerId, " cannot join room ", _id, " - room is full");
            return false;
        }

        // Check if room is in valid state for joining
        if (_state != RoomState::WAITING && _state != RoomState::STARTING) {
            LOG_WARNING("Player ", playerId, " cannot join room ", _id, " - game already in progress");
            return false;
        }

        // Check if player already in room
        if (hasPlayer(playerId)) {
            LOG_WARNING("Player ", playerId, " already in room ", _id);
            return false;
        }

        _players.push_back(playerId);

        // Set first player as host
        if (_players.size() == 1) {
            _hostPlayerId = playerId;
            LOG_INFO("Player ", playerId, " is now host of room ", _id);
        }

        LOG_INFO("✓ Player ", playerId, " joined room ", _id, " (", _players.size(), "/", _maxPlayers,
                 " players)");
        return true;
    }

    bool Room::leave(uint32_t playerId) {
        std::lock_guard<std::mutex> lock(_mutex);

        auto it = std::find(_players.begin(), _players.end(), playerId);
        if (it == _players.end()) {
            return false;
        }

        _players.erase(it);
        LOG_INFO("✓ Player ", playerId, " left room ", _id, " (", _players.size(), " players remaining)");

        // If host left, assign new host
        if (playerId == _hostPlayerId && !_players.empty()) {
            _hostPlayerId = _players[0];
            LOG_INFO("Player ", _hostPlayerId, " is now host of room ", _id);
        }

        return true;
    }

    std::string Room::getId() const {
        return _id;
    }

    std::string Room::getName() const {
        return _name;
    }

    RoomState Room::getState() const {
        return _state;
    }

    void Room::setState(RoomState state) {
        if (_state != state) {
            const char *stateNames[] = {"WAITING", "STARTING", "IN_PROGRESS", "FINISHED"};
            LOG_INFO("Room '", _name, "' state: ", stateNames[static_cast<int>(_state)], " -> ",
                     stateNames[static_cast<int>(state)]);
            _state = state;

            // Start countdown when entering STARTING state
            if (state == RoomState::STARTING) {
                _startCountdown = 3.0f;  // 3 seconds countdown
            }
        }
    }

    size_t Room::getPlayerCount() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _players.size();
    }

    size_t Room::getMaxPlayers() const {
        return _maxPlayers;
    }

    bool Room::isFull() const {
        return _players.size() >= _maxPlayers;
    }

    std::vector<uint32_t> Room::getPlayers() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _players;
    }

    bool Room::hasPlayer(uint32_t playerId) const {
        return std::find(_players.begin(), _players.end(), playerId) != _players.end();
    }

    RoomInfo Room::getInfo() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return RoomInfo{
            _id, _name, _state, _players.size(), _maxPlayers, _isPrivate, std::to_string(_hostPlayerId)};
    }

    void Room::setGameLogic(std::shared_ptr<IGameLogic> gameLogic) {
        _gameLogic = gameLogic;
        LOG_INFO("GameLogic instance attached to room ", _id);
    }

    std::shared_ptr<IGameLogic> Room::getGameLogic() const {
        return _gameLogic;
    }

    ServerLoop *Room::getServerLoop() const {
        return _gameLoop.get();
    }

    std::shared_ptr<EventBus> Room::getEventBus() const {
        return _eventBus;
    }

    bool Room::startGame() {
        std::lock_guard<std::mutex> lock(_mutex);

        if (_state != RoomState::WAITING && _state != RoomState::STARTING) {
            LOG_WARNING("Cannot start game in room ", _id, " - invalid state");
            return false;
        }

        if (_players.empty()) {
            LOG_WARNING("Cannot start game in room ", _id, " - no players");
            return false;
        }

        // Initialize game logic if present
        if (_gameLogic) {
            if (!_gameLogic->initialize()) {
                LOG_ERROR("Failed to initialize game logic for room ", _id);
                return false;
            }

            // Spawn all players in the game
            for (uint32_t playerId : _players) {
                uint32_t entityId = _gameLogic->spawnPlayer(playerId, "Player" + std::to_string(playerId));
                LOG_INFO("✓ Spawned player ", playerId, " as entity ", entityId);
            }
        }

        setState(RoomState::IN_PROGRESS);
        LOG_INFO("✓ Game started in room ", _id, " with ", _players.size(), " players");

        // Note: Server will detect state change and send GameStart messages to players

        return true;
    }

    void Room::update(float deltaTime) {
        // Auto-start countdown when room has players and is waiting
        if (_state == RoomState::WAITING && _players.size() >= 1) {
            // Start countdown if we have at least 1 player (for testing)
            setState(RoomState::STARTING);
            LOG_INFO("Room ", _id, " starting countdown (", _players.size(), " players)");
        }

        // Handle STARTING state countdown
        if (_state == RoomState::STARTING) {
            _startCountdown -= deltaTime;
            if (_startCountdown <= 0.0f) {
                startGame();
            }
        }

        // Update game logic if game is running
        if (_state == RoomState::IN_PROGRESS && _gameLogic) {
            static uint32_t tick = 0;
            _gameLogic->update(deltaTime, tick++);

            // Check if game ended
            if (!_gameLogic->isGameActive()) {
                setState(RoomState::FINISHED);
                LOG_INFO("Game ended in room ", _id);
            }
        }
    }

    void Room::setHost(uint32_t playerId) {
        if (hasPlayer(playerId)) {
            _hostPlayerId = playerId;
            LOG_INFO("Player ", playerId, " is now host of room ", _id);
        }
    }

    uint32_t Room::getHost() const {
        return _hostPlayerId;
    }

}  // namespace server
