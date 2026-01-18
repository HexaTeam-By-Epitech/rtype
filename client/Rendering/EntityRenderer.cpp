/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** EntityRenderer.cpp - Implementation of entity rendering system
*/

#include "EntityRenderer.hpp"
#include <chrono>
#include <cmath>
#include "../common/Logger/Logger.hpp"

EntityRenderer::EntityRenderer(Graphics::RaylibGraphics &graphics) : _graphics(graphics) {
    LOG_DEBUG("EntityRenderer created");
}

void EntityRenderer::updateEntity(uint32_t id, RType::Messages::Shared::EntityType type, float x, float y,
                                  int health, const std::string &currentAnimation, int srcX, int srcY,
                                  int srcW, int srcH, float velocityX, float velocityY, uint32_t serverTick) {
    uint64_t currentTime = getCurrentTimeMs();

    auto it = _entities.find(id);
    if (it != _entities.end()) {
        bool isLocalPlayer = (id == _myEntityId);

        // Always update type and health first (critical data)
        it->second.type = type;
        it->second.health = health;

        // Check if entity is a projectile (bullets should NOT be interpolated)
        bool isProjectile = (type == RType::Messages::Shared::EntityType::PlayerBullet ||
                             type == RType::Messages::Shared::EntityType::EnemyBullet);

        if (isLocalPlayer && _clientSidePredictionEnabled) {
            // CLIENT-SIDE PREDICTION for local player
            float errorX = x - it->second.x;
            float errorY = y - it->second.y;
            float errorDistance = std::sqrt(errorX * errorX + errorY * errorY);

            // ADAPTIVE MICRO-JITTER FILTERING
            // When player IS MOVING: Apply strict filtering to avoid jitter during active movement
            // When player IS STOPPED: Accept small corrections to prevent drift accumulation
            float jitterThreshold = _localPlayerIsMoving ? 2.0f : 0.5f;

            if (errorDistance < jitterThreshold) {
                // When stopped, we still want to sync positions, just not create visible jumps
                // So we'll do a very gentle correction instead of ignoring it completely
                if (!_localPlayerIsMoving && errorDistance > 0.1f) {
                    // Gentle correction: slowly drift towards server position
                    it->second.prevX = it->second.x;
                    it->second.prevY = it->second.y;
                    it->second.targetX = x;
                    it->second.targetY = y;
                    it->second.interpolationFactor = 0.5f;  // Start halfway for very smooth transition
                }
                return;  // Don't apply large corrections for micro-jitter
            }

            // Only reconcile when error exceeds threshold
            if (errorDistance > _reconciliationThreshold) {
                // Significant desync detected - smooth correction needed
                it->second.prevX = it->second.x;
                it->second.prevY = it->second.y;
                it->second.targetX = x;
                it->second.targetY = y;
                it->second.interpolationFactor = 0.0f;
            }
            // Otherwise keep predicted position - client knows best!
        } else if (_interpolationEnabled && !isProjectile) {
            // TIME-BASED INTERPOLATION for other entities (but NOT projectiles)
            // Projectiles move too fast (300 units/sec) for smooth interpolation
            // Add new snapshot to buffer
            RenderableEntity::Snapshot snapshot;
            snapshot.x = x;
            snapshot.y = y;
            snapshot.velocityX = velocityX;
            snapshot.velocityY = velocityY;
            snapshot.timestamp = currentTime;
            snapshot.serverTick = serverTick;

            it->second.snapshots.push_back(snapshot);

            // Keep only last 3 snapshots
            while (it->second.snapshots.size() > 3) {
                it->second.snapshots.pop_front();
            }

            // Also update legacy fields for smooth transition
            it->second.prevX = it->second.x;
            it->second.prevY = it->second.y;
            it->second.targetX = x;
            it->second.targetY = y;
            it->second.interpolationFactor = 0.0f;
        } else {
            // No interpolation - snap directly (for projectiles and when disabled)
            it->second.x = x;
            it->second.y = y;
        }

        // Always update sprite coords and animation
        it->second.currentAnimation = currentAnimation;
        it->second.startPixelX = srcX;
        it->second.startPixelY = srcY;
        it->second.spriteSizeX = srcW;
        it->second.spriteSizeY = srcH;
    } else {
        // Create new entity
        RenderableEntity newEntity;
        newEntity.entityId = id;
        newEntity.type = type;
        newEntity.x = x;
        newEntity.y = y;
        newEntity.health = health;
        newEntity.interpolationDelay = 100;  // 100ms interpolation delay
        newEntity.extrapolationEnabled = true;
        newEntity.prevX = x;
        newEntity.prevY = y;
        newEntity.targetX = x;
        newEntity.targetY = y;
        newEntity.interpolationFactor = 1.0f;
        newEntity.startPixelX = srcX;
        newEntity.startPixelY = srcY;
        newEntity.spriteSizeX = srcW;
        newEntity.spriteSizeY = srcH;
        newEntity.offsetX = 0;
        newEntity.offsetY = 0;
        newEntity.scale = 3.0f;
        newEntity.currentAnimation = currentAnimation;
        newEntity.currentFrame = 0;

        // Add initial snapshot
        RenderableEntity::Snapshot snapshot;
        snapshot.x = x;
        snapshot.y = y;
        snapshot.velocityX = velocityX;
        snapshot.velocityY = velocityY;
        snapshot.timestamp = currentTime;
        snapshot.serverTick = serverTick;
        newEntity.snapshots.push_back(snapshot);

        _entities[id] = newEntity;
        LOG_DEBUG("Entity created: ID=", id, " Type=", static_cast<int>(type), " at (", x, ",", y, ")");
    }
}

void EntityRenderer::removeEntity(uint32_t id) {
    auto it = _entities.find(id);
    if (it != _entities.end()) {
        LOG_DEBUG("Entity removed: ID=", id);
        _entities.erase(it);
    }
}

void EntityRenderer::clearAllEntities() {
    LOG_INFO("Clearing all entities (", _entities.size(), " total)");
    _entities.clear();
}

void EntityRenderer::setBackground(const std::string &mainBackground, const std::string &parallaxBackground,
                                   float scrollSpeed, float parallaxSpeedFactor) {
    // Clear previous backgrounds
    clearBackground();

    // Always activate background (even with just black)
    _backgroundActive = true;

    // Configure main background (if provided)
    if (!mainBackground.empty()) {
        _mainBackground.texturePath = mainBackground;
        _mainBackground.textureName = "bg_main";
        _mainBackground.scrollSpeed = scrollSpeed;
        _mainBackground.scrollOffset = 0.0f;
        _mainBackground.loaded = false;

        // Load texture
        std::string fullPath = "assets/" + mainBackground;
        if (_graphics.LoadTexture(_mainBackground.textureName.c_str(), fullPath.c_str()) == 0) {
            _graphics.GetTextureSize(_mainBackground.textureName.c_str(), _mainBackground.textureWidth,
                                     _mainBackground.textureHeight);
            _mainBackground.loaded = true;
            LOG_INFO("Loaded main background: ", fullPath, " (", _mainBackground.textureWidth, "x",
                     _mainBackground.textureHeight, ")");
        } else {
            LOG_WARNING("Failed to load main background: ", fullPath, " - using black background");
        }
    } else {
        LOG_INFO("No main background defined - using black background");
    }

    // Configure parallax background (rendered on top, scrolls slower) - only if provided
    if (!parallaxBackground.empty()) {
        _parallaxBackground.texturePath = parallaxBackground;
        _parallaxBackground.textureName = "bg_parallax";
        _parallaxBackground.scrollSpeed = scrollSpeed * parallaxSpeedFactor;
        _parallaxBackground.scrollOffset = 0.0f;
        _parallaxBackground.loaded = false;

        // Load texture
        std::string fullPath = "assets/" + parallaxBackground;
        if (_graphics.LoadTexture(_parallaxBackground.textureName.c_str(), fullPath.c_str()) == 0) {
            _graphics.GetTextureSize(_parallaxBackground.textureName.c_str(),
                                     _parallaxBackground.textureWidth, _parallaxBackground.textureHeight);
            _parallaxBackground.loaded = true;
            LOG_INFO("Loaded parallax background: ", fullPath, " (", _parallaxBackground.textureWidth, "x",
                     _parallaxBackground.textureHeight, ") speed factor: ", parallaxSpeedFactor);
        } else {
            LOG_WARNING("Failed to load parallax background: ", fullPath, " - no parallax layer");
        }
    }
    // If no parallaxBackground provided, simply don't render any parallax layer (transparent)

    LOG_INFO("Background system activated (main: ", _mainBackground.loaded ? "loaded" : "black",
             ", parallax: ", _parallaxBackground.loaded ? "loaded" : "none", ")");
}

void EntityRenderer::clearBackground() {
    if (_mainBackground.loaded) {
        _graphics.UnloadTexture(_mainBackground.textureName.c_str());
        _mainBackground = BackgroundConfig{};
    }
    if (_parallaxBackground.loaded) {
        _graphics.UnloadTexture(_parallaxBackground.textureName.c_str());
        _parallaxBackground = BackgroundConfig{};
    }
    _backgroundActive = false;
    LOG_DEBUG("Background system deactivated");
}

void EntityRenderer::updateBackground(float deltaTime) {
    if (!_backgroundActive) {
        return;
    }

    // Update scroll offsets (scrolling left = negative offset increases)
    if (_mainBackground.loaded) {
        _mainBackground.scrollOffset += _mainBackground.scrollSpeed * deltaTime;
        // Wrap around for seamless tiling
        if (_mainBackground.textureWidth > 0) {
            while (_mainBackground.scrollOffset >= _mainBackground.textureWidth) {
                _mainBackground.scrollOffset -= _mainBackground.textureWidth;
            }
        }
    }

    if (_parallaxBackground.loaded) {
        _parallaxBackground.scrollOffset += _parallaxBackground.scrollSpeed * deltaTime;
        // Wrap around for seamless tiling
        if (_parallaxBackground.textureWidth > 0) {
            while (_parallaxBackground.scrollOffset >= _parallaxBackground.textureWidth) {
                _parallaxBackground.scrollOffset -= _parallaxBackground.textureWidth;
            }
        }
    }
}

void EntityRenderer::renderBackground() {
    int screenWidth = _graphics.GetWindowWidth();
    int screenHeight = _graphics.GetWindowHeight();

    // Always draw a black background first as base
    _graphics.DrawRectFilled(0, 0, screenWidth, screenHeight, 0xFF000000);

    if (!_backgroundActive) {
        return;
    }

    // Render main background (bottom layer)
    // Scale texture to fit screen height, calculate scaled width for tiling
    if (_mainBackground.loaded && _mainBackground.textureWidth > 0 && _mainBackground.textureHeight > 0) {
        // Calculate the scaled dimensions to fit screen height
        float scale = static_cast<float>(screenHeight) / static_cast<float>(_mainBackground.textureHeight);
        float scaledWidth = static_cast<float>(_mainBackground.textureWidth) * scale;

        // Calculate how many tiles needed to cover screen + 1 extra for seamless scroll
        int tilesNeeded = static_cast<int>(std::ceil(static_cast<float>(screenWidth) / scaledWidth)) + 2;

        // Wrap scroll offset to prevent overflow
        float wrappedOffset = std::fmod(_mainBackground.scrollOffset * scale, scaledWidth);
        if (wrappedOffset < 0) {
            wrappedOffset += scaledWidth;
        }

        for (int i = 0; i < tilesNeeded; i++) {
            float drawX = (static_cast<float>(i) * scaledWidth) - wrappedOffset;

            // Draw texture stretched to fit screen height, tiled horizontally
            _graphics.DrawTexturePro(_mainBackground.textureName.c_str(), 0, 0, _mainBackground.textureWidth,
                                     _mainBackground.textureHeight, drawX, 0.0f, scaledWidth,
                                     static_cast<float>(screenHeight), 0xFFFFFFFF);
        }
    }

    // Render parallax background on top (overlay layer - only if loaded)
    // This layer should have transparency in the texture (e.g., stars with transparent background)
    if (_parallaxBackground.loaded && _parallaxBackground.textureWidth > 0 &&
        _parallaxBackground.textureHeight > 0) {
        float scale =
            static_cast<float>(screenHeight) / static_cast<float>(_parallaxBackground.textureHeight);
        float scaledWidth = static_cast<float>(_parallaxBackground.textureWidth) * scale;

        int tilesNeeded = static_cast<int>(std::ceil(static_cast<float>(screenWidth) / scaledWidth)) + 2;

        float wrappedOffset = std::fmod(_parallaxBackground.scrollOffset * scale, scaledWidth);
        if (wrappedOffset < 0) {
            wrappedOffset += scaledWidth;
        }

        for (int i = 0; i < tilesNeeded; i++) {
            float drawX = (static_cast<float>(i) * scaledWidth) - wrappedOffset;

            _graphics.DrawTexturePro(_parallaxBackground.textureName.c_str(), 0, 0,
                                     _parallaxBackground.textureWidth, _parallaxBackground.textureHeight,
                                     drawX, 0.0f, scaledWidth, static_cast<float>(screenHeight), 0xFFFFFFFF);
        }
    }
}

void EntityRenderer::setMyEntityId(uint32_t id) {
    _myEntityId = id;
    LOG_INFO("Local player entity ID set to: ", id);
    LOG_DEBUG("_myEntityId is now: ", _myEntityId);
}

void EntityRenderer::render() {
    // Always render background first (even if no entities)
    renderBackground();

    if (_entities.empty()) {
        return;
    }

    // Debug: count entities by type once per second
    static int frameCount = 0;
    if (++frameCount % 60 == 0) {
        int wallCount = 0, playerCount = 0, enemyCount = 0;
        for (const auto &[id, entity] : _entities) {
            if (entity.type == RType::Messages::Shared::EntityType::Wall)
                wallCount++;
            else if (entity.type == RType::Messages::Shared::EntityType::Player)
                playerCount++;
            else if (entity.type == RType::Messages::Shared::EntityType::EnemyType1)
                enemyCount++;
        }
        LOG_INFO("EntityRenderer: Rendering ", _entities.size(), " entities - Players:", playerCount,
                 " Enemies:", enemyCount, " Walls:", wallCount);
    }

    // Note: Interpolation is updated separately via updateInterpolation()
    // which should be called from GameLoop before render()

    for (const auto &[id, entity] : _entities) {
        switch (entity.type) {
            case RType::Messages::Shared::EntityType::Player:
                renderPlayer(entity, id == _myEntityId);
                break;

            case RType::Messages::Shared::EntityType::EnemyType1:
                renderEnemy(entity);
                break;

            case RType::Messages::Shared::EntityType::PlayerBullet:
            case RType::Messages::Shared::EntityType::EnemyBullet:
                renderProjectile(entity);
                break;

            case RType::Messages::Shared::EntityType::Wall:
                renderWall(entity);
                break;

            case RType::Messages::Shared::EntityType::OrbitalModule:
                renderOrbitalModule(entity);
                break;

            default:
                LOG_WARNING("Unknown entity type: ", static_cast<int>(entity.type));
                break;
        }
        if (_showDebugInfo) {
            renderDebugInfo(entity);
        }
    }
}

void EntityRenderer::renderPlayer(const RenderableEntity &entity, bool isLocalPlayer) {
    // Draw player sprite with animation
    // R-Type player ship sprites are in the top-left area of r-typesheet1

    // Source rectangle on the sprite sheet (frame from animation)
    int srcX = entity.startPixelX;
    int srcY = entity.startPixelY;
    int srcWidth = entity.spriteSizeX > 0 ? entity.spriteSizeX : 33;
    int srcHeight = entity.spriteSizeY > 0 ? entity.spriteSizeY : 17;

    // Scale up 3x for better visibility
    float scale = entity.scale > 0.0f ? entity.scale : 3.0f;

    // Visual differentiation: tint green for local player
    uint32_t tint = isLocalPlayer ? 0xFF008000 : 0xFFFFFFFF;

    _graphics.DrawTextureEx("PlayerShips.gif", srcX, srcY, srcWidth, srcHeight,
                            entity.x - (srcWidth * scale / 2), entity.y - (srcHeight * scale / 2), 0.0f,
                            scale, tint);

    // Render health bar if entity has health
    if (entity.health > 0) {
        renderHealthBar(entity.x, entity.y - 30.0f, entity.health, 100);
    }

    if (isLocalPlayer) {
        // Show local player indicator
        _graphics.DrawText(-1, "YOU", static_cast<int>(entity.x - 15), static_cast<int>(entity.y - 50), 14,
                           0x9DFF73AA);

        // TODO: Add charge indicator
        // When weapon is charging, show a progress bar or glow effect around the ship
        // Example: Draw a circular progress bar based on charge level (0.0 to 1.0)
        // if (entity.weaponChargeLevel > 0.0f) {
        //     renderChargeIndicator(entity.x, entity.y, entity.weaponChargeLevel);
        // }
    }
}

void EntityRenderer::renderEnemy(const RenderableEntity &entity) {
    // Red enemy visualization
    // TODO: Load enemy sprite based on type (EnemyType1, EnemyType2, etc.)

    // Placeholder: Red rectangle
    uint32_t color = 0xFF0000FF;  // Red
    float halfSize = 12.0f;

    _graphics.DrawRectFilled(static_cast<int>(entity.x - halfSize), static_cast<int>(entity.y - halfSize), 24,
                             24, color);

    // Render health bar for enemies
    if (entity.health > 0) {
        renderHealthBar(entity.x, entity.y - 20.0f, entity.health, 50);
    }
}

void EntityRenderer::renderProjectile(const RenderableEntity &entity) {
    // Draw projectile sprite with animation from sprite sheet

    // Source rectangle on the sprite sheet (frame from animation)
    int srcX = entity.startPixelX > 0 ? entity.startPixelX : 267;
    int srcY = entity.startPixelY > 0 ? entity.startPixelY : 84;
    int srcWidth = entity.spriteSizeX > 0 ? entity.spriteSizeX : 17;
    int srcHeight = entity.spriteSizeY > 0 ? entity.spriteSizeY : 13;

    // Debug log (only for first few frames)
    static int debugCount = 0;
    if (debugCount < 10) {
        LOG_DEBUG("Projectile ", entity.entityId, ": sprite(", srcX, ",", srcY, ",", srcWidth, ",", srcHeight,
                  ") anim=", entity.currentAnimation);
        debugCount++;
    }

    // Use scale from entity (server sends 2.0f for normal, 2.5f for charged)
    float scale = entity.scale > 0.0f ? entity.scale : 2.0f;

    // Different tint for enemy bullets
    uint32_t tint = 0xFFFFFFFF;
    if (entity.type == RType::Messages::Shared::EntityType::EnemyBullet) {
        tint = 0xFF5555FF;  // Reddish tint for enemy bullets
    }

    _graphics.DrawTextureEx("Projectiles", srcX, srcY, srcWidth, srcHeight, entity.x - (srcWidth * scale / 2),
                            entity.y - (srcHeight * scale / 2), 0.0f, scale, tint);
}

void EntityRenderer::renderHealthBar(float x, float y, int health, int maxHealth) {
    // Sanity check
    if (maxHealth <= 0 || health < 0) {
        return;
    }

    // Bar dimensions
    float barWidth = 30.0f;
    float barHeight = 4.0f;
    float healthRatio = static_cast<float>(health) / static_cast<float>(maxHealth);

    // Clamp health ratio to [0, 1]
    if (healthRatio > 1.0f)
        healthRatio = 1.0f;
    if (healthRatio < 0.0f)
        healthRatio = 0.0f;

    // Calculate bar position (centered above entity)
    float barX = x - barWidth / 2.0f;

    // Background bar (damage indicator - red)
    _graphics.DrawRectFilled(static_cast<int>(barX), static_cast<int>(y), static_cast<int>(barWidth),
                             static_cast<int>(barHeight), 0xFF0000FF);

    // Foreground bar (remaining health - green)
    _graphics.DrawRectFilled(static_cast<int>(barX), static_cast<int>(y),
                             static_cast<int>(barWidth * healthRatio), static_cast<int>(barHeight),
                             0x00FF00FF);

    // Optional: Border around health bar for visibility
    // _graphics.DrawRectangleBorder(barX, y, barWidth, barHeight, 1.0f, 0xFFFFFFFF);
}

void EntityRenderer::renderWall(const RenderableEntity &entity) {
    // Wall rendering with Wall.png texture
    float width = entity.spriteSizeX > 0 ? static_cast<float>(entity.spriteSizeX) : 50.0f;
    float height = entity.spriteSizeY > 0 ? static_cast<float>(entity.spriteSizeY) : 50.0f;

    // Calculate position (top-left corner for drawing)
    float x = entity.x - width / 2.0f;
    float y = entity.y - height / 2.0f;

    // Color tint: ABGR format
    uint32_t tint = 0xFFFFFFFF;  // White (no tint) by default
    if (entity.health > 0) {
        // Destructible wall - tint red when damaged
        float healthRatio = entity.health / 100.0f;
        uint8_t red = 255;
        uint8_t green = static_cast<uint8_t>(255 * healthRatio);
        uint8_t blue = static_cast<uint8_t>(255 * healthRatio);
        // ABGR format: 0xAABBGGRR
        tint = 0xFF000000 | (blue << 16) | (green << 8) | red;
    }

    // Draw Wall.png texture stretched to fit the wall dimensions
    // Respect the actual width and height separately (no average scale)
    float scaleX = width / 50.0f;   // Assuming Wall.png is 50 pixels wide
    float scaleY = height / 50.0f;  // Assuming Wall.png is 50 pixels tall

    // For proper stretching, we need to use DrawTexturePro or tile it
    // Let's just fill with solid color for now, more efficient for large walls
    uint32_t wallColor = 0xFF13458BFF;  // Brown color in ABGR
    if (entity.health > 0) {
        wallColor = tint;  // Use health-based tint for destructible walls
    }

    _graphics.DrawRectFilled(static_cast<int>(x), static_cast<int>(y), static_cast<int>(width),
                             static_cast<int>(height), wallColor);

    // Draw border for visibility
    _graphics.DrawRectangleLines(static_cast<int>(x), static_cast<int>(y), static_cast<int>(width),
                                 static_cast<int>(height), 0xFF000000);

    // If destructible, show health bar
    if (entity.health > 0) {
        renderHealthBar(entity.x, y - 10.0f, entity.health, 100);
    }
}

void EntityRenderer::renderOrbitalModule(const RenderableEntity &entity) {
    // Draw orbital module sprite with animation

    // Source rectangle on the sprite sheet (frame from animation)
    int srcX = entity.startPixelX;
    int srcY = entity.startPixelY;
    int srcWidth = entity.spriteSizeX > 0 ? entity.spriteSizeX : 17;
    int srcHeight = entity.spriteSizeY > 0 ? entity.spriteSizeY : 18;

    // Use scale from entity (server sends 2.0f)
    float scale = entity.scale > 0.0f ? entity.scale : 2.0f;

    // White tint (no color modification)
    uint32_t tint = 0xFFFFFFFF;

    _graphics.DrawTextureEx("OrbitalModule", srcX, srcY, srcWidth, srcHeight,
                            entity.x - (srcWidth * scale / 2), entity.y - (srcHeight * scale / 2), 0.0f,
                            scale, tint);

    // Optional: Render health bar if it has health
    if (entity.health > 0) {
        renderHealthBar(entity.x, entity.y - 15.0f, entity.health, 50);
    }
}

void EntityRenderer::renderDebugInfo(const RenderableEntity &entity) {
    // Display entity ID above the entity
    std::string idText = "ID:" + std::to_string(entity.entityId);
    float textX = entity.x - 15.0f;
    float textY = entity.y - 45.0f;

    _graphics.DrawText(-1, idText.c_str(), static_cast<int>(textX), static_cast<int>(textY), 10, 0xFFFFFFFF);

    // Display health if applicable
    if (entity.health > 0) {
        std::string hpText = "HP:" + std::to_string(entity.health);
        _graphics.DrawText(-1, hpText.c_str(), static_cast<int>(textX), static_cast<int>(textY - 12), 10,
                           0xFFFFFFFF);
    }

    // Display entity type (for debugging)
    std::string typeText = "Type:" + std::to_string(static_cast<int>(entity.type));
    _graphics.DrawText(-1, typeText.c_str(), static_cast<int>(textX), static_cast<int>(textY - 24), 10,
                       0xAAAAAAAA);
}

void EntityRenderer::updateInterpolation(float deltaTime) {
    if (!_interpolationEnabled) {
        return;
    }

    for (auto &[id, entity] : _entities) {
        // Skip projectiles - they move too fast for interpolation (300 units/sec)
        // Interpolation would cause visual "sliding" instead of smooth linear movement
        if (entity.type == RType::Messages::Shared::EntityType::PlayerBullet ||
            entity.type == RType::Messages::Shared::EntityType::EnemyBullet) {
            continue;
        }

        // Skip if already at target
        if (entity.interpolationFactor >= 1.0f) {
            continue;
        }

        // Advance interpolation factor based on deltaTime and speed
        entity.interpolationFactor += deltaTime * _interpolationSpeed;
        entity.interpolationFactor = clamp(entity.interpolationFactor, 0.0f, 1.0f);

        // Calculate interpolated position using linear interpolation
        entity.x = lerp(entity.prevX, entity.targetX, entity.interpolationFactor);
        entity.y = lerp(entity.prevY, entity.targetY, entity.interpolationFactor);
    }
}

void EntityRenderer::moveEntityLocally(uint32_t entityId, float deltaX, float deltaY) {
    auto it = _entities.find(entityId);
    if (it == _entities.end()) {
        return;  // Entity doesn't exist
    }

    // Apply movement immediately to current position (prediction)
    it->second.x += deltaX;
    it->second.y += deltaY;

    it->second.targetX += deltaX;
    it->second.targetY += deltaY;
    it->second.prevX += deltaX;
    it->second.prevY += deltaY;
}

float EntityRenderer::lerp(float start, float end, float t) const {
    return start + ((end - start) * t);
}

float EntityRenderer::clamp(float value, float min, float max) const {
    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}

uint64_t EntityRenderer::getCurrentTimeMs() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return static_cast<uint64_t>(duration.count());
}
