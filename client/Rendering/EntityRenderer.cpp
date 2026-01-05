/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** EntityRenderer.cpp - Implementation of entity rendering system
*/

#include "EntityRenderer.hpp"
#include "../common/Logger/Logger.hpp"

EntityRenderer::EntityRenderer(Graphics::RaylibGraphics &graphics) : _graphics(graphics) {
    LOG_DEBUG("EntityRenderer created");
}

void EntityRenderer::updateEntity(uint32_t id, RType::Messages::Shared::EntityType type, float x, float y,
                                  int health) {
    auto it = _entities.find(id);
    if (it != _entities.end()) {
        if (_interpolationEnabled) {
            // Save current display position as previous
            it->second.prevX = it->second.x;
            it->second.prevY = it->second.y;
            // Set new target position from server
            it->second.targetX = x;
            it->second.targetY = y;
            // Reset interpolation to start from beginning
            it->second.interpolationFactor = 0.0f;
        } else {
            // No interpolation - snap directly to new position
            it->second.x = x;
            it->second.y = y;
        }
        // Always update type and health
        it->second.type = type;
        it->second.health = health;
    } else {
        // Create new entity - initialize with immediate position (no interpolation for first frame)
        _entities[id] = {
            id,      // entityId
            type,    // type
            x,       // x (display position)
            y,       // y (display position)
            health,  // health
            x,       // prevX (same as current for first frame)
            y,       // prevY
            x,       // targetX
            y,       // targetY
            1.0f     // interpolationFactor (fully arrived)
        };
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

void EntityRenderer::setMyEntityId(uint32_t id) {
    _myEntityId = id;
    LOG_INFO("Local player entity ID set to: ", id);
}

void EntityRenderer::render() {
    if (_entities.empty()) {
        return;
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
    // Visual differentiation: local player is green, others are blue
    // TODO: Replace with actual sprite loading when assets are ready
    // Example: _graphics.DrawTextureEx("player_ship", ...)

    // Placeholder: Draw colored rectangle
    // Green for local player (0x00FF00FF), Blue for others (0x0080FFFF)
    uint32_t color = isLocalPlayer ? 0x00FF00FF : 0x0080FFFF;

    // Draw a 32x32 rectangle centered on entity position
    float halfSize = 16.0f;
    _graphics.DrawRectFilled(static_cast<int>(entity.x - halfSize), static_cast<int>(entity.y - halfSize), 32,
                             32, color);

    // Render health bar if entity has health
    if (entity.health > 0) {
        renderHealthBar(entity.x, entity.y - 30.0f, entity.health, 100);
    }

    // Optional: Draw player name or ID
    if (isLocalPlayer) {
        _graphics.DrawText(-1, "YOU", static_cast<int>(entity.x - 15), static_cast<int>(entity.y - 50), 12,
                           0xFFFFFFFF);
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
    // Small bullet visualization

    uint32_t color;
    if (entity.type == RType::Messages::Shared::EntityType::PlayerBullet) {
        color = 0xFFFF00FF;  // Yellow
    } else {
        color = 0xFF00FFFF;  // Magenta
    }  // Placeholder: Small 8x8 rectangle
    float halfSize = 4.0f;

    _graphics.DrawRectFilled(static_cast<int>(entity.x - halfSize), static_cast<int>(entity.y - halfSize), 8,
                             8, color);

    // Projectiles typically don't have health bars
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
