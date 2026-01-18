/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** SoundEffectManager - Manages menu sound effects
*/

#pragma once

#include "Audio/ISoundEffectService.hpp"
#include "Graphics/IGraphics.hpp"

namespace Audio {
    /**
     * @brief Default volume for sound effects (65% of original volume).
     *
     * Adjust this constant to change the default volume for all menu sounds.
     */
    constexpr float DEFAULT_SOUND_EFFECT_VOLUME = 0.65F;

    /**
     * @brief Sound effect manager implementation using the graphics audio system.
     *
     * This class manages loading and playing menu sound effects.
     * It uses the IGraphics interface for audio operations, maintaining
     * consistency with the rest of the codebase.
     *
     * Sound file paths (relative to project root):
     * - Button click: assets/sounds/menu-click.mp3
     * - Main menu open: assets/sounds/main-menu-open.mp3
     * - Room menu open: assets/sounds/rooms-menu-open.mp3
     */
    class SoundEffectManager : public ISoundEffectService {
       public:
        /**
         * @brief Construct the sound effect manager.
         * @param graphics Graphics interface with audio capabilities.
         */
        explicit SoundEffectManager(Graphics::IGraphics &graphics);

        /**
         * @brief Destructor - unloads all sounds.
         */
        ~SoundEffectManager() override;

        // Non-copyable
        SoundEffectManager(const SoundEffectManager &) = delete;
        SoundEffectManager &operator=(const SoundEffectManager &) = delete;

        // Movable
        SoundEffectManager(SoundEffectManager &&) = default;
        SoundEffectManager &operator=(SoundEffectManager &&) = default;

        /**
         * @brief Initialize the audio system and load all sounds.
         * @return true if initialization was successful.
         */
        bool Initialize();

        /** @copydoc ISoundEffectService::PlayClickSound */
        void PlayClickSound() override;

        /** @copydoc ISoundEffectService::PlayMainMenuOpenSound */
        void PlayMainMenuOpenSound() override;

        /** @copydoc ISoundEffectService::PlayRoomMenuOpenSound */
        void PlayRoomMenuOpenSound() override;

        /** @copydoc ISoundEffectService::SetVolume */
        void SetVolume(float volume) override;

        /** @copydoc ISoundEffectService::GetVolume */
        [[nodiscard]] float GetVolume() const override;

       private:
        void LoadSounds();
        void ApplyVolumeToAllSounds();

        Graphics::IGraphics &_graphics;
        float _volume{DEFAULT_SOUND_EFFECT_VOLUME};
        bool _initialized{false};

        // Sound identifiers
        static constexpr const char *SOUND_CLICK = "menu_click";
        static constexpr const char *SOUND_MAIN_MENU_OPEN = "main_menu_open";
        static constexpr const char *SOUND_ROOM_MENU_OPEN = "room_menu_open";

        // Sound file paths
        static constexpr const char *PATH_CLICK = "assets/sounds/menu-click.mp3";
        static constexpr const char *PATH_MAIN_MENU_OPEN = "assets/sounds/main-menu-open.mp3";
        static constexpr const char *PATH_ROOM_MENU_OPEN = "assets/sounds/rooms-menu-open.mp3";
    };
}  // namespace Audio
