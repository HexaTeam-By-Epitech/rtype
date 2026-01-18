/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ISoundEffectService - Interface for sound effect management
*/

#pragma once

namespace Audio {
    /**
     * @brief Interface for sound effect management in menus.
     *
     * This interface provides a high-level abstraction for playing
     * sound effects in the UI layer. Implementations should handle
     * loading, caching, and playing sounds.
     *
     * Follows the Interface Segregation Principle by providing
     * only the methods needed for menu sound effects.
     */
    class ISoundEffectService {
       public:
        virtual ~ISoundEffectService() = default;

        /**
         * @brief Play the button click sound effect.
         */
        virtual void PlayClickSound() = 0;

        /**
         * @brief Play the main menu open sound effect.
         */
        virtual void PlayMainMenuOpenSound() = 0;

        /**
         * @brief Play the room menu open sound effect.
         */
        virtual void PlayRoomMenuOpenSound() = 0;

        /**
         * @brief Set the master volume for all sound effects.
         * @param volume Volume level (0.0 to 1.0)
         */
        virtual void SetVolume(float volume) = 0;

        /**
         * @brief Get the current master volume.
         * @return Current volume level (0.0 to 1.0)
         */
        [[nodiscard]] virtual float GetVolume() const = 0;
    };
}  // namespace Audio
