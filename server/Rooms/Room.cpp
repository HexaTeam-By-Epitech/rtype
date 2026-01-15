/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** Room.cpp
*/

#include "server/Rooms/Room.hpp"
#include <algorithm>
#include <stdexcept>
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
          _gameStartSent(false) {

        _eventBus = std::make_shared<EventBus>();
        std::shared_ptr<ecs::wrapper::ECSWorld> ecsWorld = std::make_shared<ecs::wrapper::ECSWorld>();
        std::shared_ptr<ThreadPool> threadPool = std::make_shared<ThreadPool>(4);
        threadPool->start();
        std::unique_ptr<IGameLogic> gameLogic = std::make_unique<GameLogic>(ecsWorld, threadPool, _eventBus);
        _gameLoop = std::make_unique<ServerLoop>(std::move(gameLogic), _eventBus);

        if (!_gameLoop->initialize()) {
            LOG_ERROR("Failed to initialize game loop for room ", _id);
            throw std::runtime_error("Failed to initialize game loop for room " + _id);
        }

        _gameLoop->start();

        _gameLogic = std::shared_ptr<IGameLogic>(&_gameLoop->getGameLogic(), [](IGameLogic *) {});

        LOG_INFO("Room '", _name, "' (", _id, ") created [State: WAITING, Max: ", _maxPlayers,
                 " players, Private: ", (_isPrivate ? "Yes" : "No"), "] with dedicated GameLoop");
    }

    bool Room::join(uint32_t playerId) {
        std::lock_guard<std::mutex> lock(_mutex);

        if (isFull()) {
            LOG_WARNING("Player ", playerId, " cannot join room ", _id, " - room is full");
            return false;
        }

        if (_state != RoomState::WAITING && _state != RoomState::STARTING) {
            LOG_WARNING("Player ", playerId, " cannot join room ", _id, " - game already in progress");
            return false;
        }
        if (hasPlayer(playerId)) {
            LOG_WARNING("Player ", playerId, " already in room ", _id);
            return false;
        }
        _players.push_back(playerId);

        if (_players.size() == 1) {
            _hostPlayerId = playerId;
            LOG_INFO("Player ", playerId, " is host of room ", _id);
        }

        LOG_INFO("Player ", playerId, " joined room ", _id, " (", _players.size(), "/", _maxPlayers, ")");
        return true;
    }

    bool Room::joinAsSpectator(uint32_t playerId) {
        std::lock_guard<std::mutex> lock(_mutex);

        if (hasPlayer(playerId) || hasSpectator(playerId)) {
            LOG_WARNING("Player ", playerId, " already in room ", _id);
            return false;
        }

        _spectators.push_back(playerId);
        LOG_INFO("Spectator ", playerId, " joined room ", _id, " (", _spectators.size(), " spectators)");
        return true;
    }

    bool Room::leave(uint32_t playerId) {
        std::lock_guard<std::mutex> lock(_mutex);

        // Check if it's a regular player
        auto it = std::find(_players.begin(), _players.end(), playerId);
        if (it != _players.end()) {
            _players.erase(it);
            LOG_INFO("Player ", playerId, " left room ", _id, " (", _players.size(), " remaining)");

            if (playerId == _hostPlayerId && !_players.empty()) {
                _hostPlayerId = _players[0];
                LOG_INFO("Player ", _hostPlayerId, " is new host");
            }

            // Reset room to WAITING if all players left
            if (_players.empty() && (_state == RoomState::IN_PROGRESS || _state == RoomState::STARTING)) {
                setState(RoomState::WAITING);
                _gameStartSent = false;
                LOG_INFO("Room ", _id, " reset to WAITING (no players left)");
            }

            return true;
        }

        // Check if it's a spectator
        auto specIt = std::find(_spectators.begin(), _spectators.end(), playerId);
        if (specIt != _spectators.end()) {
            _spectators.erase(specIt);
            LOG_INFO("Spectator ", playerId, " left room ", _id, " (", _spectators.size(),
                     " spectators remaining)");
            return true;
        }

        return false;
    }

    void Room::setState(RoomState state) {
        if (_state != state) {
            const char *stateNames[] = {"WAITING", "STARTING", "IN_PROGRESS", "FINISHED"};
            LOG_INFO("Room '", _name, "' state: ", stateNames[static_cast<int>(_state)], " -> ",
                     stateNames[static_cast<int>(state)]);
            _state = state;
        }
    }

    size_t Room::getPlayerCount() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _players.size();
    }

    std::vector<uint32_t> Room::getPlayers() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _players;
    }

    std::vector<uint32_t> Room::getSpectators() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _spectators;
    }

    bool Room::hasPlayer(uint32_t playerId) const {
        return std::find(_players.begin(), _players.end(), playerId) != _players.end();
    }

    bool Room::hasSpectator(uint32_t playerId) const {
        return std::ranges::find(_spectators, playerId) != _spectators.end();
    }

    RoomInfo Room::getInfo() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return RoomInfo{
            _id, _name, _state, _players.size(), _maxPlayers, _isPrivate, std::to_string(_hostPlayerId)};
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

        if (_gameLogic) {
            if (!_gameLogic->initialize()) {
                LOG_ERROR("Failed to initialize game logic for room ", _id);
                return false;
            }

            // Spawn enemies (Lua scripts) now that game is starting
            // Cast to GameLogic to access spawnEnemies()
            LOG_INFO("Will call onGameStart for room ", _id);
            if (auto *gameLogic = dynamic_cast<GameLogic *>(_gameLogic.get())) {
                gameLogic->onGameStart();
            }

            // Spawn players and validate entity IDs
            std::vector<uint32_t> failedPlayers;
            for (uint32_t playerId : _players) {
                uint32_t entityId = _gameLogic->spawnPlayer(playerId, "Player" + std::to_string(playerId));
                if (entityId == 0) {
                    LOG_ERROR("Failed to spawn player ", playerId, " in room ", _id);
                    failedPlayers.push_back(playerId);
                }
            }

            // Remove players that failed to spawn
            if (!failedPlayers.empty()) {
                for (uint32_t playerId : failedPlayers) {
                    auto it = std::find(_players.begin(), _players.end(), playerId);
                    if (it != _players.end()) {
                        _players.erase(it);
                        LOG_WARNING("Removed player ", playerId, " from room ", _id, " due to spawn failure");
                    }
                }

                // Check if we still have players after removals
                if (_players.empty()) {
                    LOG_ERROR("No players left in room ", _id, " after spawn failures");
                    return false;
                }
            }
        }

        setState(RoomState::IN_PROGRESS);
        LOG_INFO("Game started in room ", _id, " with ", _players.size(), " players");
        return true;
    }

    void Room::update(float deltaTime) {
        if (_state == RoomState::IN_PROGRESS && _gameLogic) {
            static uint32_t tick = 0;
            _gameLogic->update(deltaTime, tick++);

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

    void Room::requestStartGame() {
        if (_state == RoomState::WAITING) {
            LOG_INFO("Room ", _id, " starting game immediately (", _players.size(), " players)");
            startGame();
        }
    }

    bool Room::tryMarkGameStartSent() {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_gameStartSent) {
            return false;  // Already sent
        }
        _gameStartSent = true;
        return true;  // Successfully marked as sent
    }

}  // namespace server
