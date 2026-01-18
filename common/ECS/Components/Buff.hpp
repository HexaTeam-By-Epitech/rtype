/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Buff - Component for temporary and permanent buffs
*/

#pragma once

#include <algorithm>
#include <vector>
#include "IComponent.hpp"

namespace ecs {

    /**
     * @enum BuffType
     * @brief Types of buffs that can be applied to entities
     */
    enum class BuffType {
        // Temporary buffs (duration-based)
        SpeedBoost,     ///< Increases movement speed
        DamageBoost,    ///< Increases weapon damage
        FireRateBoost,  ///< Increases fire rate
        Shield,         ///< Temporary invincibility
        HealthRegen,    ///< Regenerates health over time

        // Permanent buffs (modify behavior permanently)
        MultiShot,          ///< Shoot in multiple directions
        PiercingShot,       ///< Projectiles pierce through enemies
        HomingShot,         ///< Projectiles home towards enemies
        MaxHealthIncrease,  ///< Permanently increase max health
        DoubleShot,         ///< Fire two projectiles at once
        TripleShot          ///< Fire three projectiles at once
    };

    /**
     * @struct BuffInstance
     * @brief Individual buff with its properties
     */
    struct BuffInstance {
        BuffType type;     ///< Type of buff
        float duration;    ///< Remaining duration (0.0f = permanent)
        float value;       ///< Buff value (multiplier or absolute value)
        bool isPermanent;  ///< True if buff never expires

        BuffInstance(BuffType t, float dur, float val, bool perm = false)
            : type(t), duration(dur), value(val), isPermanent(perm) {}
    };

    /**
     * @class Buff
     * @brief Component managing active buffs on an entity
     * 
     * Stores and manages multiple active buffs with their durations.
     * Supports both temporary (time-limited) and permanent buffs.
     */
    class Buff : public IComponent {
       public:
        /**
         * @brief Default constructor
         */
        Buff() = default;

        /**
         * @brief Constructor with initial buff
         * @param type Type of buff
         * @param duration Duration in seconds (0.0f for permanent)
         * @param value Buff value/multiplier
         */
        Buff(BuffType type, float duration, float value) { addBuff(type, duration, value); }

        ~Buff() override = default;

        /**
         * @brief Add a new buff
         * @param type Type of buff
         * @param duration Duration in seconds (0.0f for permanent)
         * @param value Buff value/multiplier
         */
        void addBuff(BuffType type, float duration, float value) {
            bool isPermanent = (duration <= 0.0f);

            // Check if buff already exists
            for (auto &buff : _buffs) {
                if (buff.type == type) {
                    // Refresh duration and update value
                    buff.duration = duration;
                    buff.value = value;
                    buff.isPermanent = isPermanent;
                    return;
                }
            }

            // Add new buff
            _buffs.emplace_back(type, duration, value, isPermanent);
        }

        /**
         * @brief Remove a specific buff
         * @param type Type of buff to remove
         */
        void removeBuff(BuffType type) {
            _buffs.erase(std::remove_if(_buffs.begin(), _buffs.end(),
                                        [type](const BuffInstance &b) { return b.type == type; }),
                         _buffs.end());
        }

        /**
         * @brief Check if entity has a specific buff
         * @param type Type of buff to check
         * @return True if buff is active
         */
        bool hasBuff(BuffType type) const {
            for (const auto &buff : _buffs) {
                if (buff.type == type)
                    return true;
            }
            return false;
        }

        /**
         * @brief Get buff value for a specific type
         * @param type Type of buff
         * @return Buff value, or 1.0f if not found
         */
        float getBuffValue(BuffType type) const {
            for (const auto &buff : _buffs) {
                if (buff.type == type)
                    return buff.value;
            }
            return 1.0f;
        }

        /**
         * @brief Get all active buffs
         * @return Vector of all buff instances
         */
        const std::vector<BuffInstance> &getBuffs() const { return _buffs; }

        /**
         * @brief Get mutable reference to buffs (for system updates)
         * @return Mutable vector reference
         */
        std::vector<BuffInstance> &getBuffsMutable() { return _buffs; }

        /**
         * @brief Check if any buffs are active
         * @return True if at least one buff is active
         */
        bool hasAnyBuffs() const { return !_buffs.empty(); }

        /**
         * @brief Clear all buffs
         */
        void clearAllBuffs() { _buffs.clear(); }

        /**
         * @brief Get the component type ID
         * @return ComponentType Unique ID for Buff component
         */
        ComponentType getType() const override { return getComponentType<Buff>(); }

       private:
        std::vector<BuffInstance> _buffs;  ///< Active buffs on this entity
    };

}  // namespace ecs
