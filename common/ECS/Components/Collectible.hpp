/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Collectible - Component for pickup items
*/

#pragma once

#include "Buff.hpp"
#include "IComponent.hpp"

namespace ecs {

    /**
     * @enum CollectibleType
     * @brief Types of collectible items
     */
    enum class CollectibleType {
        PowerUp,     ///< Temporary power-up (buffs)
        Upgrade,     ///< Permanent upgrade
        HealthPack,  ///< Restores health
        Score        ///< Score bonus
    };

    /**
     * @class Collectible
     * @brief Component for items that can be picked up by players
     * 
     * Defines pickup items with their effects when collected.
     * Can grant buffs, restore health, or award points.
     */
    class Collectible : public IComponent {
       public:
        /**
         * @brief Constructor for buff-granting collectible
         * @param buffType Type of buff to grant
         * @param duration Duration of buff (0.0f for permanent)
         * @param value Buff value/multiplier
         */
        Collectible(BuffType buffType, float duration, float value)
            : _type(duration > 0.0f ? CollectibleType::PowerUp : CollectibleType::Upgrade),
              _buffType(buffType),
              _duration(duration),
              _value(value),
              _healthRestore(0),
              _scoreValue(0) {}

        /**
         * @brief Constructor for health pack
         * @param healthRestore Amount of health to restore
         */
        explicit Collectible(int healthRestore)
            : _type(CollectibleType::HealthPack),
              _buffType(BuffType::SpeedBoost),  // Unused
              _duration(0.0f),
              _value(0.0f),
              _healthRestore(healthRestore),
              _scoreValue(0) {}

        /**
         * @brief Constructor for score pickup
         * @param scoreValue Score points awarded
         * @param isScoreType Dummy parameter to differentiate from health constructor
         */
        Collectible(int scoreValue, bool isScoreType)
            : _type(CollectibleType::Score),
              _buffType(BuffType::SpeedBoost),  // Unused
              _duration(0.0f),
              _value(0.0f),
              _healthRestore(0),
              _scoreValue(scoreValue) {
            (void)isScoreType;  // Unused, just for overload resolution
        }

        ~Collectible() override = default;

        /**
         * @brief Get collectible type
         * @return CollectibleType Type of item
         */
        CollectibleType getCollectibleType() const { return _type; }

        /**
         * @brief Get buff type (if applicable)
         * @return BuffType Type of buff granted
         */
        BuffType getBuffType() const { return _buffType; }

        /**
         * @brief Get buff duration
         * @return float Duration in seconds
         */
        float getDuration() const { return _duration; }

        /**
         * @brief Get buff value
         * @return float Buff value/multiplier
         */
        float getValue() const { return _value; }

        /**
         * @brief Get health restore amount
         * @return int Health points restored
         */
        int getHealthRestore() const { return _healthRestore; }

        /**
         * @brief Get score value
         * @return int Score points awarded
         */
        int getScoreValue() const { return _scoreValue; }

        /**
         * @brief Check if this grants a buff
         * @return True if collectible grants a buff
         */
        bool grantsBuff() const {
            return _type == CollectibleType::PowerUp || _type == CollectibleType::Upgrade;
        }

        /**
         * @brief Check if this restores health
         * @return True if collectible restores health
         */
        bool restoresHealth() const { return _type == CollectibleType::HealthPack; }

        /**
         * @brief Check if this awards score
         * @return True if collectible awards score
         */
        bool awardsScore() const { return _type == CollectibleType::Score; }

        /**
         * @brief Get the component type ID
         * @return ComponentType Unique ID for Collectible component
         */
        ComponentType getType() const override { return getComponentType<Collectible>(); }

       private:
        CollectibleType _type;  ///< Type of collectible
        BuffType _buffType;     ///< Buff granted (if applicable)
        float _duration;        ///< Buff duration
        float _value;           ///< Buff value
        int _healthRestore;     ///< Health restored (if health pack)
        int _scoreValue;        ///< Score points (if score pickup)
    };

}  // namespace ecs
