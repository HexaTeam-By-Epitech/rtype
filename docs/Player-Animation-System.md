# Player-Animation System Documentation

## Overview

The R-Type project uses an Entity Component System (ECS) architecture where player animations are managed through three main components: `AnimationSet`, `Animation`, and `Sprite`. This document explains how player entities are linked to animations from creation through runtime updates to rendering.

---

## Architecture Components

### 1. AnimationSet Component
**Location:** `common/ECS/Components/AnimationSet.hpp`

Stores the collection of all available animation clips for an entity.

```cpp
class AnimationSet : public IComponent {
    std::string _textureKey;  // Texture identifier (e.g., "PlayerShip")
    std::unordered_map<std::string, AnimationClip> _clips;  // Named animation clips
};
```

**AnimationClip Structure:**
```cpp
struct AnimationClip {
    std::vector<Rectangle> frames;  // Frame rectangles in sprite sheet
    float frameDuration;            // Seconds per frame
    bool loop;                      // Loop the animation?
    std::string nextClip;           // Optional transition after completion
};
```

### 2. Animation Component
**Location:** `common/ECS/Components/Animation.hpp`

Tracks the current playback state of an animation.

```cpp
class Animation : public IComponent {
    std::string _currentClipName;  // Current animation clip being played
    float _timer;                  // Playback timer
    int _currentFrameIndex;        // Current frame in the clip
    bool _isPlaying;               // Play/pause state
    bool _loop;                    // Loop playback?
};
```

### 3. Sprite Component
**Location:** `common/ECS/Components/Sprite.hpp`

Contains visual rendering data that gets updated by the animation system.

```cpp
class Sprite : public IComponent {
    std::string textureKey;  // Texture resource identifier
    Rectangle sourceRect;    // Current frame rectangle from animation
    // ... color, rotation, scale, etc.
};
```

---

## Animation Definition

### AnimationDatabase
**Location:** `common/Animation/AnimationDatabase.hpp`

Factory functions create predefined animation sets for different entity types.

**Player Animations:**
```cpp
static AnimationSet createPlayerAnimations() {
    AnimationSet animSet("PlayerShip");
    
    // Idle animation: single frame
    animSet.addClip("player_idle", AnimationClip(
        {{1, 69, 33, 14}},  // Single frame at (1,69) size 33x14
        0.2f,               // Duration (unused for single frame)
        true                // Loop enabled
    ));
    
    // Movement animation: 5 frames
    animSet.addClip("player_movement", AnimationClip(
        {
            {1, 69, 33, 14},    // Frame 0
            {34, 69, 33, 14},   // Frame 1
            {67, 69, 33, 14},   // Frame 2
            {100, 69, 33, 14},  // Frame 3
            {133, 69, 33, 14}   // Frame 4
        },
        0.2f,  // 200ms per frame = 1 second full cycle
        true   // Loop enabled
    ));
    
    return animSet;
}
```

---

## Player Entity Creation

### Server-Side Spawn
**Location:** `server/Game/Logic/GameLogic.cpp` (lines 178-191)

When a player joins, the server creates an entity with animation components:

```cpp
// Create the animation set with predefined player animations
ecs::AnimationSet playerAnimations = AnimDB::createPlayerAnimations();

// Create the player entity with all components
ecs::wrapper::Entity playerEntity =
    _world->createEntity()
        .with(playerAnimations)                   // Add animation definitions
        .with(ecs::Animation("player_idle"));     // Start with idle animation
```

**Key Points:**
- Player starts with `"player_idle"` animation
- `AnimationSet` contains all available clips
- `Animation` component tracks current playback state

---

## Runtime Animation Updates

### Animation Switching Logic
**Location:** `server/Game/Logic/GameLogic.cpp` (lines 308-346)

The server updates player animations based on input each frame:

```cpp
// Get the Animation component
auto &anim = playerEntity.getComponent<ecs::Animation>();

// Check if player is moving
bool isMoving = (input.movementX != 0 || input.movementY != 0);

if (isMoving) {
    // Switch to movement animation if not already playing
    if (anim.getCurrentClipName() != "player_movement") {
        anim.setCurrentClipName("player_movement");
        anim.setCurrentFrameIndex(0);  // Reset to first frame
        anim.setTimer(0.0f);           // Reset timer
    }
} else {
    // Switch to idle animation when stationary
    if (anim.getCurrentClipName() != "player_idle") {
        anim.setCurrentClipName("player_idle");
        anim.setCurrentFrameIndex(0);
        anim.setTimer(0.0f);
    }
}
```

**State Machine:**
```
    No Input              Movement Input
┌─────────────┐        ┌──────────────────┐
│ player_idle │◄──────►│ player_movement  │
│  (1 frame)  │        │   (5 frames)     │
└─────────────┘        └──────────────────┘
```

### AnimationSystem Updates
**Location:** `common/ECS/Systems/AnimationSystem/AnimationSystem.hpp`

The `AnimationSystem` automatically advances animation playback:

```cpp
void AnimationSystem::update(Registry &registry, float deltaTime) {
    // For each entity with Animation, AnimationSet, and Sprite components:
    // 1. Advance timer by deltaTime
    // 2. If timer exceeds frame duration, advance to next frame
    // 3. Handle looping/stopping at end of animation
    // 4. Update Sprite.sourceRect with current frame rectangle
}
```

**Frame Advancement:**
```
Timer: 0.0s → 0.2s → 0.4s → 0.6s → 0.8s → 1.0s (loop back to frame 0)
Frame:   0  →   1  →   2  →   3  →   4  →   0
```

---

## Client-Side Rendering

### EntityRenderer
**Location:** `client/Rendering/EntityRenderer.cpp` (lines 169-180)

The client renders players using animation frame data from the `Sprite` component:

```cpp
void EntityRenderer::renderPlayer(const ecs::Entity &entity, float lerpFactor) {
    // Animation system has already updated Sprite.sourceRect with current frame
    
    // Get frame position and size from entity fields
    // (these are updated by AnimationSystem based on Animation component)
    int srcX = entity.startPixelX;      // From current animation frame
    int srcY = entity.startPixelY;      // From current animation frame
    int srcWidth = entity.spriteSizeX;  // From current animation frame
    int srcHeight = entity.spriteSizeY; // From current animation frame
    
    Rectangle src = {
        static_cast<float>(srcX),
        static_cast<float>(srcY),
        static_cast<float>(srcWidth),
        static_cast<float>(srcHeight)
    };
    
    // Render scaled 3x for visibility
    Rectangle dest = {
        /* ... interpolated position ... */
        static_cast<float>(srcWidth * 3),
        static_cast<float>(srcHeight * 3)
    };
    
    _graphics->DrawTextureSection(
        entity.textureKey,  // "PlayerShip"
        src,                // Current animation frame rectangle
        dest,               // Screen position and size
        /* ... rotation, color ... */
    );
}
```

---

## Data Flow Summary

```
1. DEFINITION (AnimationDatabase)
   └─> createPlayerAnimations() defines "player_idle" and "player_movement"

2. CREATION (GameLogic::SpawnPlayer)
   └─> Entity created with AnimationSet + Animation("player_idle")

3. UPDATE (GameLogic + AnimationSystem)
   ├─> GameLogic switches animation based on input
   │   └─> Sets Animation.currentClipName
   └─> AnimationSystem advances frames
       └─> Updates Sprite.sourceRect from AnimationClip.frames[currentFrameIndex]

4. RENDER (EntityRenderer)
   └─> Reads entity.startPixelX/Y (from Sprite.sourceRect)
   └─> Draws current animation frame to screen
```

---

## Key Takeaways

1. **Separation of Concerns:** 
   - `AnimationSet` = available animations (what *can* play)
   - `Animation` = current playback state (what *is* playing)
   - `Sprite` = visual data (what *to render*)

2. **Server Authority:**
   - Server controls animation state based on game logic
   - Clients receive animation state through entity synchronization

3. **Automatic Frame Advancement:**
   - `AnimationSystem` handles timing and frame progression
   - Game logic only needs to change `currentClipName` to switch animations

4. **Sprite Sheet Mapping:**
   - Each animation clip stores source rectangles (x, y, width, height)
   - Frames reference positions in the "PlayerShip" texture
   - All player animations share the same sprite sheet texture

5. **Performance:**
   - Frame data (rectangles) defined once at initialization
   - Runtime only updates indices and timers
   - No texture reloading or asset management during gameplay
