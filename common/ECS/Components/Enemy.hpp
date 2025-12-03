/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Enemy
*/

#ifndef ENEMY_HPP_
#define ENEMY_HPP_

#include "IComponent.hpp"

namespace ecs {
    class Enemy : public IComponent {
       public:
        Enemy(int enemyType, int scoreValue, int attackPattern)
            : _enemyType(enemyType), _scoreValue(scoreValue), _attackPattern(attackPattern) {}
        ~Enemy() override = default;

        // Getters
        int getEnemyType() const { return _enemyType; }
        int getScoreValue() const { return _scoreValue; }
        int getAttackPattern() const { return _attackPattern; }

        // Setters
        void setEnemyType(int enemyType) { _enemyType = enemyType; }
        void setScoreValue(int scoreValue) { _scoreValue = scoreValue; }
        void setAttackPattern(int attackPattern) { _attackPattern = attackPattern; }

        // Override getType method
        ComponentType getType() const override {
            return getComponentType<Enemy>();
        }

       private:
       int _enemyType;
       int _scoreValue;
       int _attackPattern;
    };
}

#endif
