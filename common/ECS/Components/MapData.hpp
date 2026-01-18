/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** MapData.hpp - Component storing map/level information
*/

#pragma once

#include <string>
#include "IComponent.hpp"

namespace ecs {
    /**
     * @class MapData
     * @brief Component storing information about the current map/level.
     * 
     * Contains all data needed to configure and manage a game map, including
     * scrolling speed, background assets, spawn script, and custom metadata.
     * 
     * This component is typically attached to a singleton entity that manages
     * the active map state.
     */
    class MapData : public IComponent {
       public:
        /**
         * @brief Default constructor.
         * Initializes with empty map data.
         */
        MapData()
            : _mapId(""),
              _name(""),
              _scrollSpeed(0.0f),
              _backgroundSprite(""),
              _parallaxBackgroundSprite(""),
              _parallaxSpeedFactor(0.5f),
              _spawnScript(""),
              _duration(0.0f),
              _nextMapId(""),
              _elapsedTime(0.0f),
              _isCompleted(false) {}

        /**
         * @brief Constructor with basic parameters.
         * @param mapId Unique identifier for this map
         * @param scrollSpeed Horizontal scroll speed (pixels/second)
         * @param backgroundSprite Background sprite asset path
         * @param spawnScript Lua script path for spawn management
         */
        MapData(const std::string &mapId, float scrollSpeed, const std::string &backgroundSprite,
                const std::string &spawnScript)
            : _mapId(mapId),
              _name(""),
              _scrollSpeed(scrollSpeed),
              _backgroundSprite(backgroundSprite),
              _parallaxBackgroundSprite(""),
              _parallaxSpeedFactor(0.5f),
              _spawnScript(spawnScript),
              _duration(0.0f),
              _nextMapId(""),
              _elapsedTime(0.0f),
              _isCompleted(false) {}

        /**
         * @brief Full constructor with all parameters.
         * @param mapId Unique identifier for this map
         * @param name Display name of the map
         * @param scrollSpeed Horizontal scroll speed (pixels/second)
         * @param backgroundSprite Background sprite asset path
         * @param spawnScript Lua script path for spawn management
         * @param duration Map duration in seconds (0 = infinite)
         * @param nextMapId ID of the next map to load after completion
         * @param parallaxBackground Parallax layer sprite (rendered on top, scrolls slower)
         * @param parallaxSpeedFactor Speed factor for parallax layer (0.5 = half speed)
         */
        MapData(const std::string &mapId, const std::string &name, float scrollSpeed,
                const std::string &backgroundSprite, const std::string &spawnScript, float duration,
                const std::string &nextMapId, const std::string &parallaxBackground = "",
                float parallaxSpeedFactor = 0.5f)
            : _mapId(mapId),
              _name(name),
              _scrollSpeed(scrollSpeed),
              _backgroundSprite(backgroundSprite),
              _parallaxBackgroundSprite(parallaxBackground),
              _parallaxSpeedFactor(parallaxSpeedFactor),
              _spawnScript(spawnScript),
              _duration(duration),
              _nextMapId(nextMapId),
              _elapsedTime(0.0f),
              _isCompleted(false) {}

        ~MapData() override = default;

        // ===== Getters =====

        /**
         * @brief Get the map unique identifier.
         * @return Map ID string
         */
        const std::string &getMapId() const { return _mapId; }

        /**
         * @brief Get the map display name.
         * @return Map name string
         */
        const std::string &getName() const { return _name; }

        /**
         * @brief Get the scroll speed.
         * @return Scroll speed in pixels/second
         */
        float getScrollSpeed() const { return _scrollSpeed; }

        /**
         * @brief Get the background sprite path.
         * @return Background sprite asset path
         */
        const std::string &getBackgroundSprite() const { return _backgroundSprite; }

        /**
         * @brief Get the parallax background sprite path.
         * @return Parallax background sprite asset path (rendered on top, scrolls slower)
         */
        const std::string &getParallaxBackgroundSprite() const { return _parallaxBackgroundSprite; }

        /**
         * @brief Get the parallax speed factor.
         * @return Speed factor for parallax layer (0.5 = half speed of main background)
         */
        float getParallaxSpeedFactor() const { return _parallaxSpeedFactor; }

        /**
         * @brief Get the spawn script path.
         * @return Lua script path for spawn management
         */
        const std::string &getSpawnScript() const { return _spawnScript; }

        /**
         * @brief Get the map duration.
         * @return Duration in seconds (0 = infinite)
         */
        float getDuration() const { return _duration; }

        /**
         * @brief Get the next map ID.
         * @return ID of the next map to load
         */
        const std::string &getNextMapId() const { return _nextMapId; }

        /**
         * @brief Get the elapsed time on this map.
         * @return Elapsed time in seconds
         */
        float getElapsedTime() const { return _elapsedTime; }

        /**
         * @brief Check if the map is completed.
         * @return True if map is completed
         */
        bool isCompleted() const { return _isCompleted; }

        // ===== Setters =====

        /**
         * @brief Set the map ID.
         * @param mapId New map ID
         */
        void setMapId(const std::string &mapId) { _mapId = mapId; }

        /**
         * @brief Set the map name.
         * @param name New map name
         */
        void setName(const std::string &name) { _name = name; }

        /**
         * @brief Set the scroll speed.
         * @param speed New scroll speed in pixels/second
         */
        void setScrollSpeed(float speed) { _scrollSpeed = speed; }

        /**
         * @brief Set the background sprite.
         * @param sprite New background sprite path
         */
        void setBackgroundSprite(const std::string &sprite) { _backgroundSprite = sprite; }

        /**
         * @brief Set the parallax background sprite.
         * @param sprite New parallax background sprite path
         */
        void setParallaxBackgroundSprite(const std::string &sprite) { _parallaxBackgroundSprite = sprite; }

        /**
         * @brief Set the parallax speed factor.
         * @param factor New speed factor (0.5 = half speed)
         */
        void setParallaxSpeedFactor(float factor) { _parallaxSpeedFactor = factor; }

        /**
         * @brief Set the spawn script.
         * @param script New Lua script path
         */
        void setSpawnScript(const std::string &script) { _spawnScript = script; }

        /**
         * @brief Set the map duration.
         * @param duration New duration in seconds
         */
        void setDuration(float duration) { _duration = duration; }

        /**
         * @brief Set the next map ID.
         * @param nextMapId New next map ID
         */
        void setNextMapId(const std::string &nextMapId) { _nextMapId = nextMapId; }

        /**
         * @brief Update the elapsed time.
         * @param deltaTime Time increment in seconds
         */
        void updateElapsedTime(float deltaTime) { _elapsedTime += deltaTime; }

        /**
         * @brief Mark the map as completed.
         */
        void markCompleted() { _isCompleted = true; }

        /**
         * @brief Reset the map state (elapsed time and completion).
         */
        void reset() {
            _elapsedTime = 0.0f;
            _isCompleted = false;
        }

        /**
         * @brief Get the component type ID.
         * @return ComponentType Unique ID for MapData component.
         */
        ComponentType getType() const override { return getComponentType<MapData>(); }

       private:
        std::string _mapId;             ///< Unique map identifier
        std::string _name;              ///< Display name
        float _scrollSpeed;             ///< Horizontal scroll speed (px/s)
        std::string _backgroundSprite;  ///< Background asset path
        std::string
            _parallaxBackgroundSprite;  ///< Parallax layer asset path (rendered on top, scrolls slower)
        float _parallaxSpeedFactor;     ///< Speed factor for parallax layer (0.5 = half speed)
        std::string _spawnScript;       ///< Lua script for spawn logic
        float _duration;                ///< Map duration in seconds (0 = infinite)
        std::string _nextMapId;         ///< Next map to load after completion
        float _elapsedTime;             ///< Time spent on this map
        bool _isCompleted;              ///< Completion flag
    };

}  // namespace ecs
