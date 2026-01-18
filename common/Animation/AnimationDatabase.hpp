/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** AnimationDatabase
*/

#pragma once

#include <functional>
#include <initializer_list>
#include <unordered_map>
#include "../ECS/Components/AnimationSet.hpp"

namespace AnimDB {
    /**
     * @brief Helper to create animation clips easily.
     * 
     * @param frames Initializer list of frame rectangles
     * @param duration Duration per frame in seconds
     * @param loop Whether animation loops
     * @param nextClip Next animation clip to play after completion
     * @return ecs::AnimationClip The constructed animation clip
     */
    inline ecs::AnimationClip makeClip(std::initializer_list<ecs::Rectangle> frames, float duration = 0.1f,
                                       bool loop = true, const std::string &nextClip = "") {
        return ecs::AnimationClip(std::vector<ecs::Rectangle>(frames), duration, loop, nextClip);
    }

    /**
     * @brief Create player ship animations.
     * 
     * @return ecs::AnimationSet Complete animation set for player
     */
    inline ecs::AnimationSet createPlayerAnimations() {
        ecs::AnimationSet animSet("PlayerShip");

        // Idle animation - single frame
        animSet.addClip("player_idle", makeClip({{1, 69, 33, 14}}, 0.15f, true));

        // Flying animation - 5 frames loop (slower animation with 0.15s per frame)
        animSet.addClip(
            "player_movement",
            makeClip(
                {{1, 69, 33, 14}, {34, 69, 33, 14}, {67, 69, 33, 14}, {100, 69, 33, 14}, {133, 69, 33, 14}},
                0.2f, true));

        return animSet;
    }

    /**
     * @brief Create drone (orbital module) animations.
     * 
     * @return ecs::AnimationSet Complete animation set for orbital module
     */
    inline ecs::AnimationSet createOrbitalModuleAnimations() {
        ecs::AnimationSet animSet("OrbitalModule");

        // Spin animation - 12 frames
        animSet.addClip("orbital_spin", makeClip({{0, 0, 17, 18},
                                                  {17, 0, 17, 18},
                                                  {34, 0, 17, 18},
                                                  {51, 0, 17, 18},
                                                  {68, 0, 17, 18},
                                                  {85, 0, 17, 18},
                                                  {102, 0, 17, 18},
                                                  {119, 0, 17, 18},
                                                  {136, 0, 17, 18},
                                                  {153, 0, 17, 18},
                                                  {170, 0, 17, 18},
                                                  {187, 0, 17, 18}},
                                                 0.1f, true));

        return animSet;
    }

    /**
     * @brief Create basic enemy animations.
     * 
     * @return ecs::AnimationSet Complete animation set for basic enemy
     */
    inline ecs::AnimationSet createEnemyBasicAnimations() {
        ecs::AnimationSet animSet("BasicEnemy");

        // Simple 16 frames flying animation
        animSet.addClip("enemy_fly", makeClip({{0, 0, 33, 34},
                                               {33, 0, 33, 34},
                                               {66, 0, 33, 34},
                                               {99, 0, 33, 34},
                                               {132, 0, 33, 34},
                                               {165, 0, 33, 34},
                                               {198, 0, 33, 34},
                                               {231, 0, 33, 34},
                                               {0, 34, 33, 34},
                                               {33, 34, 33, 34},
                                               {66, 34, 33, 34},
                                               {99, 34, 33, 34},
                                               {132, 34, 33, 34},
                                               {165, 34, 33, 34},
                                               {198, 34, 33, 34},
                                               {231, 34, 33, 34}},
                                              0.1f, true));

        return animSet;
    }

    /**
     * @brief Create walking enemy animations.
     * 
     * @return ecs::AnimationSet Complete animation set for walking enemy
     */
    inline ecs::AnimationSet createEnemyWalkingAnimations() {
        ecs::AnimationSet animSet("WalkingEnemy");

        // Walking animation left 3 frames
        animSet.addClip("walk_left",
                        makeClip({{0, 0, 33, 34}, {33, 0, 33, 34}, {66, 0, 33, 34}}, 0.15f, true));

        // Walking animation right 3 frames
        animSet.addClip("walk_right",
                        makeClip({{100, 0, 33, 34}, {133, 0, 33, 34}, {166, 0, 33, 34}}, 0.15f, true));

        // Slightly flying left animation 3 frames
        animSet.addClip("fly_left",
                        makeClip({{0, 34, 33, 34}, {33, 34, 33, 34}, {66, 34, 33, 34}}, 0.1f, true));

        // Slightly flying right animation 3 frames
        animSet.addClip("fly_right",
                        makeClip({{100, 34, 33, 34}, {133, 34, 33, 34}, {166, 34, 33, 34}}, 0.1f, true));

        return animSet;
    }

    /**
     * @brief Create boss body animations.
     * 
     * @return ecs::AnimationSet Complete animation set for boss body
     */
    inline ecs::AnimationSet createBossBodyAnimations() {
        ecs::AnimationSet animSet("r-typesheet10");

        // Idle animation
        animSet.addClip("idle", makeClip({{0, 0, 128, 128}}, 0.1f, true));

        // Hurt animation - flashes then returns to idle
        animSet.addClip("hurt", makeClip({{128, 0, 128, 128}, {256, 0, 128, 128}}, 0.08f, false, "idle"));

        // Attack animation - returns to idle after
        animSet.addClip("attack", makeClip({{0, 128, 128, 128}, {128, 128, 128, 128}, {256, 128, 128, 128}},
                                           0.12f, false, "idle"));

        return animSet;
    }

    /**
     * @brief Create boss arm animations.
     * 
     * @return ecs::AnimationSet Complete animation set for boss arm
     */
    inline ecs::AnimationSet createBossArmAnimations() {
        ecs::AnimationSet animSet("r-typesheet11");

        // Idle animation
        animSet.addClip("idle", makeClip({{0, 0, 64, 64}}, 0.1f, true));

        // Attack animation
        animSet.addClip("attack",
                        makeClip({{0, 0, 64, 64}, {64, 0, 64, 64}, {128, 0, 64, 64}}, 0.15f, false, "idle"));

        return animSet;
    }

    /**
     * @brief Create player bullet animations.
     * 
     * @return ecs::AnimationSet Complete animation set for player projectile
     */
    inline ecs::AnimationSet createPlayerBulletAnimations() {
        ecs::AnimationSet animSet("Projectiles");

        animSet.addClip("projectile_fly",
                        makeClip({{267, 84, 17, 13}, {284, 84, 17, 13}, {301, 84, 17, 13}}, 0.2F, true));
        animSet.addClip("charged_projectile_1", makeClip(
                                                    {
                                                        {200, 121, 32, 10},
                                                        {232, 121, 32, 10},
                                                    },
                                                    0.2F, true));

        return animSet;
    }

    /**
     * @brief Create enemy bullet animations.
     * 
     * @return ecs::AnimationSet Complete animation set for enemy projectile
     */
    inline ecs::AnimationSet createEnemyBulletAnimations() {
        ecs::AnimationSet animSet("r-typesheet2");

        // Simple bullet animation
        animSet.addClip("fly", makeClip({{0, 48, 8, 8}, {8, 48, 8, 8}}, 0.1f, true));

        return animSet;
    }

    /**
     * @brief Factory function type for animation set creation.
     */
    using AnimationFactory = std::function<ecs::AnimationSet()>;

    /**
     * @brief Map of entity types to their animation factory functions.
     * 
     * Use this to quickly get an AnimationSet for any entity type.
     */
    inline const std::unordered_map<std::string, AnimationFactory> FACTORIES = {
        {"player", createPlayerAnimations},
        {"enemy_basic", createEnemyBasicAnimations},
        {"boss_body", createBossBodyAnimations},
        {"boss_arm", createBossArmAnimations},
        {"player_bullet", createPlayerBulletAnimations},
        {"enemy_bullet", createEnemyBulletAnimations}};

    /**
     * @brief Get an AnimationSet by entity type.
     * 
     * @param entityType Type identifier (e.g., "player", "enemy_basic")
     * @return ecs::AnimationSet The animation set for the entity type
     * 
     * @note Returns a default AnimationSet if entity type is not found
     */
    inline ecs::AnimationSet getAnimationSet(const std::string &entityType) {
        auto it = FACTORIES.find(entityType);
        if (it != FACTORIES.end()) {
            return it->second();
        }

        // Default fallback - simple idle animation
        ecs::AnimationSet defaultSet("r-typesheet1");
        defaultSet.addClip("idle", makeClip({{0, 0, 32, 32}}));
        return defaultSet;
    }
}  // namespace AnimDB
