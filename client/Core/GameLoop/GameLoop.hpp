/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** GameLoop.hpp
*/

#ifndef GAMELOOP_HPP
#define GAMELOOP_HPP

#include <memory>
#include "../EventBus/EventBus.hpp"
#include "../../Input/InputBuffer.hpp"
#include "../../Network/Replicator.hpp"
#include "../../Rendering/Rendering.hpp"

class GameLoop {
public:
    GameLoop();
    ~GameLoop();

    bool initialize();
    void run();
    void shutdown();
    void stop();

private:
    void update(float deltaTime);
    void fixedUpdate(float fixedDeltaTime);
    void render();
    void processInput();
    float calculateDeltaTime();

    
    std::unique_ptr<EventBus> _eventBus;
    std::unique_ptr<InputBuffer> _inputBuffer;
    std::unique_ptr<Replicator> _replicator;
    std::unique_ptr<Rendering> _rendering;

    bool _running = false;
    bool _initialized = false;

    float _fixedTimestep = 1.0f / 60.0f;
    float _accumulator = 0.0f;
    uint32_t _currentFrame = 0;
};

#endif