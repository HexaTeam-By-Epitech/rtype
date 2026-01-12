/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** Animation
*/

#pragma once

#include <string>
#include "IComponent.hpp"

namespace ecs {
    /**
     * @class Animation
     * @brief Component managing current animation playback state.
     * 
     * Stores the current animation state including which animation clip is playing,
     * the current frame index, playback timer, and control flags for playback behavior.
     */
    class Animation : public IComponent {
       public:
        /**
         * @brief Constructor with animation parameters.
         * @param initialClip Initial animation clip name
         * @param loop Whether the animation should loop
         * @param isPlaying Whether animation starts playing immediately
         */
        Animation(const std::string &initialClip = "idle", bool loop = true, bool isPlaying = true)
            : _currentClipName(initialClip),
              _timer(0.0f),
              _currentFrameIndex(0),
              _isPlaying(isPlaying),
              _loop(loop) {}

        ~Animation() override = default;

        /**
         * @brief Get the current animation clip name.
         * @return std::string The current clip identifier.
         */
        std::string getCurrentClipName() const { return _currentClipName; }

        /**
         * @brief Get the playback timer.
         * @return float The current timer value in seconds.
         */
        float getTimer() const { return _timer; }

        /**
         * @brief Get the current frame index.
         * @return int The index of the current frame.
         */
        int getCurrentFrameIndex() const { return _currentFrameIndex; }

        /**
         * @brief Check if animation is playing.
         * @return bool True if animation is playing.
         */
        bool isPlaying() const { return _isPlaying; }

        /**
         * @brief Check if animation is looping.
         * @return bool True if animation loops.
         */
        bool isLooping() const { return _loop; }

        /**
         * @brief Set the current animation clip name.
         * @param clipName New animation clip identifier
         */
        void setCurrentClipName(const std::string &clipName) { _currentClipName = clipName; }

        /**
         * @brief Set the playback timer.
         * @param timer New timer value in seconds
         */
        void setTimer(float timer) { _timer = timer; }

        /**
         * @brief Set the current frame index.
         * @param frameIndex New frame index
         */
        void setCurrentFrameIndex(int frameIndex) { _currentFrameIndex = frameIndex; }

        /**
         * @brief Set the playing state.
         * @param playing True to play, false to pause
         */
        void setPlaying(bool playing) { _isPlaying = playing; }

        /**
         * @brief Set the looping state.
         * @param loop True to enable looping
         */
        void setLoop(bool loop) { _loop = loop; }

        /**
         * @brief Get the component type ID.
         * @return ComponentType Unique ID for Animation component.
         */
        ComponentType getType() const override { return getComponentType<Animation>(); }

       private:
        std::string _currentClipName;  ///< Current animation clip name
        float _timer;                  ///< Playback timer in seconds
        int _currentFrameIndex;        ///< Current frame index
        bool _isPlaying;               ///< Animation playing state
        bool _loop;                    ///< Animation looping flag
    };
}  // namespace ecs
