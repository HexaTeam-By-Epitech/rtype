/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** SoundEffectManager - Manages menu sound effects
*/

#include "Audio/SoundEffectManager.hpp"
#include "../common/Logger/Logger.hpp"

namespace Audio {
    SoundEffectManager::SoundEffectManager(Graphics::IGraphics &graphics) : _graphics(graphics) {}

    SoundEffectManager::~SoundEffectManager() {
        if (_initialized) {
            _graphics.UnloadSound(SOUND_CLICK);
            _graphics.UnloadSound(SOUND_MAIN_MENU_OPEN);
            _graphics.UnloadSound(SOUND_ROOM_MENU_OPEN);
        }
    }

    bool SoundEffectManager::Initialize() {
        if (_initialized) {
            return true;
        }

        _graphics.InitAudioDevice();

        if (!_graphics.IsAudioDeviceReady()) {
            LOG_ERROR("[SoundEffectManager] Failed to initialize audio device");
            return false;
        }

        LoadSounds();
        ApplyVolumeToAllSounds();

        _initialized = true;
        LOG_INFO("[SoundEffectManager] Initialized with volume: ", static_cast<int>(_volume * 100), "%");
        return true;
    }

    void SoundEffectManager::LoadSounds() {
        if (!_graphics.LoadSound(SOUND_CLICK, PATH_CLICK)) {
            LOG_WARNING("[SoundEffectManager] Failed to load click sound: ", PATH_CLICK);
        }

        if (!_graphics.LoadSound(SOUND_MAIN_MENU_OPEN, PATH_MAIN_MENU_OPEN)) {
            LOG_WARNING("[SoundEffectManager] Failed to load main menu open sound: ", PATH_MAIN_MENU_OPEN);
        }

        if (!_graphics.LoadSound(SOUND_ROOM_MENU_OPEN, PATH_ROOM_MENU_OPEN)) {
            LOG_WARNING("[SoundEffectManager] Failed to load room menu open sound: ", PATH_ROOM_MENU_OPEN);
        }
    }

    void SoundEffectManager::ApplyVolumeToAllSounds() {
        _graphics.SetSoundVolume(SOUND_CLICK, _volume);
        _graphics.SetSoundVolume(SOUND_MAIN_MENU_OPEN, _volume);
        _graphics.SetSoundVolume(SOUND_ROOM_MENU_OPEN, _volume);
    }

    void SoundEffectManager::PlayClickSound() {
        if (_initialized) {
            _graphics.PlaySound(SOUND_CLICK);
        }
    }

    void SoundEffectManager::PlayMainMenuOpenSound() {
        if (_initialized) {
            _graphics.PlaySound(SOUND_MAIN_MENU_OPEN);
        }
    }

    void SoundEffectManager::PlayRoomMenuOpenSound() {
        if (_initialized) {
            _graphics.PlaySound(SOUND_ROOM_MENU_OPEN);
        }
    }

    void SoundEffectManager::SetVolume(float volume) {
        _volume = (volume < 0.0F) ? 0.0F : (volume > 1.0F) ? 1.0F : volume;
        if (_initialized) {
            ApplyVolumeToAllSounds();
        }
    }

    float SoundEffectManager::GetVolume() const {
        return _volume;
    }
}  // namespace Audio
