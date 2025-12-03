/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Player
*/

#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "IComponent.hpp"

namespace ecs {
    class Player : public IComponent {
       public:
        Player(int score, int lives, int playerId)
            : _score(score), _lives(lives), _playerId(playerId) {}
        ~Player() override = default;

        // Getters
        int getScore() const { return _score; }
        int getLives() const { return _lives; }
        int getPlayerId() const { return _playerId; }

        // Setters
        void setScore(int score) { _score = score; }
        void setLives(int lives) { _lives = lives; }
        void setPlayerId(int playerId) { _playerId = playerId; }

        // Override getType method
        ComponentType getType() const override {
            return getComponentType<Player>();
        }

       private:
       int _score;
       int _lives;
       int _playerId;
    };
}

#endif
