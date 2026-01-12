/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** AnimationSet
*/

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "IComponent.hpp"
#include "Sprite.hpp"

namespace ecs {
    /**
     * @struct AnimationClip
     * @brief Defines a sequence of frames for an animation.
     * 
     * Contains all frames (as source rectangles), timing information,
     * loop behavior, and optional transition to next animation.
     */
    struct AnimationClip {
        std::vector<Rectangle> frames;  ///< Frame rectangles in the texture
        float frameDuration;            ///< Duration per frame in seconds
        bool loop;                      ///< Whether animation loops
        std::string nextClip;           ///< Next clip name after completion (optional)

        /**
         * @brief Default constructor.
         */
        AnimationClip() : frameDuration(0.1f), loop(true), nextClip("") {}

        /**
         * @brief Constructor with all parameters.
         * @param frames Frame rectangles
         * @param duration Duration per frame in seconds
         * @param loop Whether animation loops
         * @param nextClip Next animation clip name
         */
        AnimationClip(const std::vector<Rectangle> &frames, float duration = 0.1f, bool loop = true,
                      const std::string &nextClip = "")
            : frames(frames), frameDuration(duration), loop(loop), nextClip(nextClip) {}
    };

    /**
     * @class AnimationSet
     * @brief Component containing all available animations for an entity.
     * 
     * Stores the texture key and a map of named animation clips.
     * This component defines what animations an entity can play.
     */
    class AnimationSet : public IComponent {
       public:
        /**
         * @brief Constructor with texture key.
         * @param textureKey Texture identifier for all animations
         */
        explicit AnimationSet(const std::string &textureKey) : _textureKey(textureKey) {}

        ~AnimationSet() override = default;

        /**
         * @brief Get the texture key.
         * @return std::string The texture identifier.
         */
        std::string getTextureKey() const { return _textureKey; }

        /**
         * @brief Get all animation clips.
         * @return const std::unordered_map<std::string, AnimationClip>& Map of clip names to clips.
         */
        const std::unordered_map<std::string, AnimationClip> &getClips() const { return _clips; }

        /**
         * @brief Get a specific animation clip.
         * @param clipName Name of the animation clip
         * @return const AnimationClip* Pointer to clip, or nullptr if not found.
         */
        const AnimationClip *getClip(const std::string &clipName) const {
            auto it = _clips.find(clipName);
            return (it != _clips.end()) ? &it->second : nullptr;
        }

        /**
         * @brief Check if a clip exists.
         * @param clipName Name of the animation clip
         * @return bool True if clip exists.
         */
        bool hasClip(const std::string &clipName) const { return _clips.find(clipName) != _clips.end(); }

        /**
         * @brief Set the texture key.
         * @param textureKey New texture identifier
         */
        void setTextureKey(const std::string &textureKey) { _textureKey = textureKey; }

        /**
         * @brief Add an animation clip.
         * @param clipName Name of the clip
         * @param clip Animation clip data
         */
        void addClip(const std::string &clipName, const AnimationClip &clip) { _clips[clipName] = clip; }

        /**
         * @brief Remove an animation clip.
         * @param clipName Name of the clip to remove
         */
        void removeClip(const std::string &clipName) { _clips.erase(clipName); }

        /**
         * @brief Get the component type ID.
         * @return ComponentType Unique ID for AnimationSet component.
         */
        ComponentType getType() const override { return getComponentType<AnimationSet>(); }

       private:
        std::string _textureKey;                                ///< Texture identifier for animations
        std::unordered_map<std::string, AnimationClip> _clips;  ///< Map of animation clips
    };
}  // namespace ecs
