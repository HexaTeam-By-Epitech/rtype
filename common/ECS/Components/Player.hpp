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
    /**
     * @class Player
     * @brief Component identifying an entity as a player with game statistics.
     * 
     * Stores player-specific data such as score, remaining lives, and player ID
     * for multiplayer identification.
     */
    class Player : public IComponent {
       public:
        /**
         * @brief Constructor with all player data.
         * @param score Initial score
         * @param lives Number of lives
         * @param playerId Unique player identifier (for multiplayer)
         */
        Player(int score, int lives, int playerId) : _score(score), _lives(lives), _playerId(playerId) {}
        ~Player() override = default;

        /**
         * @brief Get player's score.
         * @return int Current score value.
         */
        int getScore() const { return _score; }

        /**
         * @brief Get remaining lives.
         * @return int Number of lives left.
         */
        int getLives() const { return _lives; }

        /**
         * @brief Get player ID.
         * @return int Unique player identifier.
         */
        int getPlayerId() const { return _playerId; }

        /**
         * @brief Set player's score.
         * @param score New score value
         */
        void setScore(int score) { _score = score; }

        /**
         * @brief Set remaining lives.
         * @param lives New lives count
         */
        void setLives(int lives) { _lives = lives; }

        /**
         * @brief Set player ID.
         * @param playerId New player identifier
         */
        void setPlayerId(int playerId) { _playerId = playerId; }

        /**
         * @brief Get the component type ID.
         * @return ComponentType Unique ID for Player component.
         */
        ComponentType getType() const override { return getComponentType<Player>(); }

       private:
        int _score;     ///< Player's current score
        int _lives;     ///< Remaining lives
        int _playerId;  ///< Unique player identifier
    };
}  // namespace ecs

#endif
