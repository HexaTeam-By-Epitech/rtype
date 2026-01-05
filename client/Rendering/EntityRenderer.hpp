/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** EntityRenderer.hpp - Specialized renderer for game entities
*/

#pragma once

#include <cstdint>
#include <unordered_map>
#include "Capnp/Messages/Shared/SharedTypes.hpp"
#include "Graphics/RaylibGraphics/RaylibGraphics.hpp"

/**
 * @class EntityRenderer
 * @brief Specialized renderer for game entities with client-side interpolation
     * 
     * This is a NETWORK MIDDLEWARE for smooth visual rendering, NOT an ECS system.
     * 
     * The EntityRenderer is responsible for:
     * - Maintaining a local cache of entity states received from the server
     * - CLIENT-SIDE INTERPOLATION: Smoothing movement between discrete server updates
     * - Rendering entities based on their type with appropriate visuals
     * - Handling visual differentiation (local player vs other players)
     * - Managing entity lifecycle (creation, update, removal)
     * 
     * INTERPOLATION FLOW:
     * 1. Server sends position at 20-30 Hz (every 30-50ms)
     * 2. updateEntity() saves current pos as "previous", new pos as "target"
     * 3. updateInterpolation() smoothly moves from previous to target (60 FPS)
     * 4. render() displays entity at interpolated position
     * 
     * This provides smooth 60 FPS visuals from 20 Hz server updates.
     */
class EntityRenderer {
   public:
    /**
         * @struct RenderableEntity
         * @brief Cached entity state for rendering
         * 
         * This structure holds the minimal information needed to render
         * an entity. It's updated whenever a network packet arrives with
         * new entity state information.
         */
    struct RenderableEntity {
        uint32_t entityId;                         ///< Unique entity identifier
        RType::Messages::Shared::EntityType type;  ///< Entity type (Player, Enemy, Bullet, etc.)
        float x;                                   ///< World position X coordinate
        float y;                                   ///< World position Y coordinate
        int health;                                ///< Current health (-1 for entities without health)
        float prevX;                               ///< Previous position X (for interpolation)
        float prevY;                               ///< Previous position Y (for interpolation)
        float targetX;                             ///< Target position X (from server)
        float targetY;                             ///< Target position Y (from server)
        float interpolationFactor;                 ///< Progress from 0.0 (prev) to 1.0 (target)

        // Future fields for enhanced rendering:
        // float rotation;        ///< Rotation angle in degrees
        // float scale;           ///< Uniform scale factor
        // uint8_t animFrame;     ///< Current animation frame
    };

    /**
         * @brief Constructor
         * @param graphics Reference to the graphics subsystem (RaylibGraphics)
         * 
         * The EntityRenderer does not own the graphics object, it only
         * holds a reference to use its drawing primitives.
         */
    explicit EntityRenderer(Graphics::RaylibGraphics &graphics);

    /**
         * @brief Destructor
         */
    ~EntityRenderer() = default;

    /**
         * @brief Update or create an entity in the local cache
         * @param id Entity unique identifier
         * @param type Entity type (Player, Enemy, Bullet)
         * @param x World position X
         * @param y World position Y
         * @param health Current health (-1 if not applicable)
         * 
         * If the entity already exists, its state is updated.
         * If it's a new entity, it's added to the cache.
         * 
         * This method should be called whenever a GameState or GameStart
         * message is received from the server.
         */
    void updateEntity(uint32_t id, RType::Messages::Shared::EntityType type, float x, float y,
                      int health); /**
         * @brief Remove an entity from the rendering cache
         * @param id Entity unique identifier to remove
         * 
         * Should be called when receiving an EntityDestroyed message
         * or when the entity is no longer in the GameState.
         */
    void removeEntity(uint32_t id);

    /**
         * @brief Clear all entities from the cache
         * 
         * Useful for scene transitions or when disconnecting from server.
         */
    void clearAllEntities();

    /**
         * @brief Set the local player's entity ID for visual differentiation
         * @param id The entity ID that represents the local player
         * 
         * This allows the renderer to highlight the player's own entity
         * with a different color or visual effect.
         * 
         * Should be called when receiving the GameStart message which
         * contains yourEntityId.
         */
    void setMyEntityId(uint32_t id);

    /**
         * @brief Render all cached entities
         * 
         * Iterates through all entities in the cache and renders them
         * based on their type. Entities are rendered in insertion order
         * (no Z-ordering implemented yet).
         * 
         * Should be called once per frame from Rendering::Render().
         */
    void render();

    /**
         * @brief Get the number of entities currently cached
         * @return Size of the entity cache
         */
    size_t getEntityCount() const { return _entities.size(); }

    /**
         * @brief Check if an entity exists in the cache
         * @param id Entity unique identifier
         * @return true if entity exists, false otherwise
         */
    bool hasEntity(uint32_t id) const { return _entities.count(id) > 0; }

    /**
         * @brief Toggle debug information overlay
         * @param enabled true to show entity IDs and health bars
         */
    void setDebugMode(bool enabled) { _showDebugInfo = enabled; }

    /**
         * @brief Enable or disable interpolation for smooth movement
         * @param enabled true to enable interpolation, false for instant updates
         * 
         * When enabled, entities smoothly transition between network updates.
         * When disabled, entities snap directly to new positions.
         */
    void setInterpolationEnabled(bool enabled) { _interpolationEnabled = enabled; }

    /**
         * @brief Set the interpolation speed multiplier
         * @param speed Speed factor (higher = faster, typical: 5-15)
         * 
         * Controls how quickly entities interpolate to target positions.
         * Lower values = smoother but more lag, higher values = snappier but less smooth.
         */
    void setInterpolationSpeed(float speed) { _interpolationSpeed = speed; }

    /**
     * @brief Enable or disable client-side prediction for local player
     * @param enabled true to enable prediction (instant movement), false for interpolation
     * 
     * When enabled, local player moves instantly without waiting for server.
     * When disabled, local player is interpolated like other entities.
     */
    void setClientSidePredictionEnabled(bool enabled) { _clientSidePredictionEnabled = enabled; }

    /**
         * @brief Update interpolation for all entities
         * @param deltaTime Time elapsed since last frame (in seconds)
         * 
         * Should be called every frame before render() to advance interpolation.
         * This provides smooth movement between discrete server updates.
         */
    void updateInterpolation(float deltaTime);

    /**
         * @brief Move an entity locally (client-side prediction)
         * @param entityId Entity to move
         * @param deltaX Movement in X direction
         * @param deltaY Movement in Y direction
         * 
         * Used for local player prediction: moves the entity immediately
         * without waiting for server confirmation. Server will later send
         * corrections which trigger reconciliation if needed.
         * 
         * This provides 0ms input latency for the local player.
         */
    void moveEntityLocally(uint32_t entityId, float deltaX, float deltaY);

   private:
    /**
         * @brief Render a player entity
         * @param entity Entity to render
         * @param isLocalPlayer true if this is the player's own entity
         * 
         * Players are rendered differently based on whether they are
         * the local player (green/highlighted) or other players (blue).
         */
    void renderPlayer(const RenderableEntity &entity, bool isLocalPlayer);

    /**
         * @brief Render an enemy entity
         * @param entity Enemy entity to render
         */
    void renderEnemy(const RenderableEntity &entity);

    /**
         * @brief Render a projectile (player or enemy bullet)
         * @param entity Projectile entity to render
         */
    void renderProjectile(const RenderableEntity &entity);

    /**
         * @brief Render a health bar above an entity
         * @param x World position X (centered)
         * @param y World position Y (above entity)
         * @param health Current health points
         * @param maxHealth Maximum health points
         * 
         * Renders a two-layer bar: red background (damage) and green
         * foreground (remaining health).
         */
    void renderHealthBar(float x, float y, int health, int maxHealth);

    /**
         * @brief Render debug information for an entity
         * @param entity Entity to display debug info for
         * 
         * Shows entity ID and health as text overlay.
         * Only rendered when _showDebugInfo is true.
         */
    void renderDebugInfo(const RenderableEntity &entity);

    /**
         * @brief Linear interpolation between two values
         * @param start Starting value
         * @param end Ending value
         * @param t Interpolation factor (0.0 to 1.0)
         * @return Interpolated value
         */
    [[nodiscard]] float lerp(float start, float end, float t) const;

    /**
         * @brief Clamp a value between min and max
         * @param value Value to clamp
         * @param min Minimum value
         * @param max Maximum value
         * @return Clamped value
         */
    [[nodiscard]] float clamp(float value, float min, float max) const;

    /// Entity cache: maps entity ID to its renderable state
    std::unordered_map<uint32_t, RenderableEntity> _entities;

    /// Local player's entity ID (for visual differentiation)
    uint32_t _myEntityId = 0;

    /// Reference to graphics subsystem for drawing operations
    Graphics::RaylibGraphics &_graphics;

    /// Debug mode: show entity IDs and health bars (toggle with F3)
    bool _showDebugInfo = true;

    /// Interpolation enabled flag
    bool _interpolationEnabled = true;

    /// Interpolation speed multiplier (higher = faster convergence)
    float _interpolationSpeed = 10.0f;

    /// Client-side prediction enabled flag (for local player only)
    bool _clientSidePredictionEnabled = true;

    /// Reconciliation threshold in pixels (corrections smaller than this are ignored)
    float _reconciliationThreshold = 5.0f;
};
