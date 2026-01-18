/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Enemy
*/

#pragma once

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
         */
        Enemy(int enemyType, int scoreValue) : _enemyType(enemyType), _scoreValue(scoreValue) {}
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
         * @brief Get the component type ID.
         * @return ComponentType Unique ID for Enemy component.
         */
        ComponentType getType() const override { return getComponentType<Enemy>(); }

       private:
        int _enemyType;   ///< Enemy type/classification
        int _scoreValue;  ///< Score points awarded on destruction
    };
}  // namespace ecs
