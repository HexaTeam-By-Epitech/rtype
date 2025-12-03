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
    /**
     * @class Enemy
     * @brief Component identifying an entity as an enemy with AI behavior.
     * 
     * Stores enemy-specific data including type classification, score value
     * awarded on destruction, and attack pattern identifier for AI systems.
     */
    class Enemy : public IComponent {
       public:
        /**
         * @brief Constructor with enemy parameters.
         * @param enemyType Type/classification of enemy (e.g., 0=basic, 1=advanced)
         * @param scoreValue Points awarded when destroyed
         * @param attackPattern AI behavior pattern identifier
         */
        Enemy(int enemyType, int scoreValue, int attackPattern)
            : _enemyType(enemyType), _scoreValue(scoreValue), _attackPattern(attackPattern) {}
        ~Enemy() override = default;

        /**
         * @brief Get enemy type.
         * @return int Enemy classification type.
         */
        int getEnemyType() const { return _enemyType; }

        /**
         * @brief Get score value.
         * @return int Points awarded on destruction.
         */
        int getScoreValue() const { return _scoreValue; }

        /**
         * @brief Get attack pattern ID.
         * @return int AI behavior pattern identifier.
         */
        int getAttackPattern() const { return _attackPattern; }

        /**
         * @brief Set enemy type.
         * @param enemyType New enemy classification
         */
        void setEnemyType(int enemyType) { _enemyType = enemyType; }

        /**
         * @brief Set score value.
         * @param scoreValue New score value
         */
        void setScoreValue(int scoreValue) { _scoreValue = scoreValue; }

        /**
         * @brief Set attack pattern.
         * @param attackPattern New behavior pattern ID
         */
        void setAttackPattern(int attackPattern) { _attackPattern = attackPattern; }

        /**
         * @brief Get the component type ID.
         * @return ComponentType Unique ID for Enemy component.
         */
        ComponentType getType() const override { return getComponentType<Enemy>(); }

       private:
        int _enemyType;      ///< Enemy type/classification
        int _scoreValue;     ///< Score points awarded on destruction
        int _attackPattern;  ///< AI attack pattern identifier
    };
}  // namespace ecs

#endif
