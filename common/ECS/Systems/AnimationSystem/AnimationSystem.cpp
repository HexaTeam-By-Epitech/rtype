/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** AnimationSystem
*/

#include "AnimationSystem.hpp"

namespace ecs {
    /**
     * @brief Updates animation playback for all animated entities.
     */
    void AnimationSystem::update(Registry &registry, float deltaTime) {
        auto entities = registry.getEntitiesWithMask(this->getComponentMask());

        for (auto entityId : entities) {
            // Check if entity still has all required components
            // (entity might have been destroyed/modified by another system)
            if (!registry.hasComponent<Animation>(entityId) ||
                !registry.hasComponent<AnimationSet>(entityId) || !registry.hasComponent<Sprite>(entityId)) {
                continue;
            }

            // Use try-catch to handle race conditions where entity is destroyed
            // between the hasComponent check and getComponent call
            Animation *animation = nullptr;
            AnimationSet *animationSet = nullptr;
            Sprite *sprite = nullptr;

            try {
                animation = &registry.getComponent<Animation>(entityId);
                animationSet = &registry.getComponent<AnimationSet>(entityId);
                sprite = &registry.getComponent<Sprite>(entityId);
            } catch (const std::exception &) {
                // Entity was destroyed/modified between check and access
                continue;
            }

            // Skip if animation is not playing
            if (!animation->isPlaying()) {
                continue;
            }

            // Get the current animation clip
            const std::string &currentClipName = animation->getCurrentClipName();
            if (!animationSet->hasClip(currentClipName)) {
                continue;  // Invalid clip name
            }

            const AnimationClip &clip = *animationSet->getClip(currentClipName);
            if (clip.frames.empty()) {
                continue;  // No frames in clip
            }

            // Advance timer
            float newTimer = animation->getTimer() + deltaTime;
            animation->setTimer(newTimer);

            // Check if it's time to advance to next frame
            if (newTimer >= clip.frameDuration) {
                animation->setTimer(0.0f);  // Reset timer

                // Advance to next frame
                int nextFrame = animation->getCurrentFrameIndex() + 1;

                // Check if we've reached the end of the animation
                if (nextFrame >= static_cast<int>(clip.frames.size())) {
                    if (clip.loop) {
                        // Loop back to first frame
                        nextFrame = 0;
                    } else {
                        // Stop on last frame
                        nextFrame = static_cast<int>(clip.frames.size()) - 1;
                        animation->setPlaying(false);

                        // Transition to next clip if specified
                        if (!clip.nextClip.empty() && animationSet->hasClip(clip.nextClip)) {
                            animation->setCurrentClipName(clip.nextClip);
                            animation->setCurrentFrameIndex(0);
                            animation->setTimer(0.0f);
                            animation->setPlaying(true);
                            continue;  // Skip sprite update, will be handled next frame
                        }
                    }
                }

                animation->setCurrentFrameIndex(nextFrame);
            }

            // Update sprite to display current frame
            int frameIndex = animation->getCurrentFrameIndex();
            if (frameIndex >= 0 && frameIndex < static_cast<int>(clip.frames.size())) {
                sprite->setSourceRect(clip.frames[frameIndex]);
            }
        }
    }

    ComponentMask AnimationSystem::getComponentMask() const {
        return (1ULL << getComponentType<Animation>()) | (1ULL << getComponentType<AnimationSet>()) |
               (1ULL << getComponentType<Sprite>());
    }
}  // namespace ecs
