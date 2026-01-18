# ECS UML Diagram

```mermaid
classDiagram
    %% Core ECS
    class Registry {
        +setComponent()
        +getComponent()
        +hasComponent()
        +removeComponent()
        +view()
        +newEntity()
        +destroyEntity()
    }
    class IComponent {
        <<interface>>
        +getType()
    }
    class ISystem {
        <<interface>>
        +update(registry, deltaTime)
        +getComponentMask()
    }
    class PrefabFactory {
        +createPlayer()
        +createEnemy()
        +createProjectile()
        +createWall()
        +createPowerUp()
        +createHealthPack()
    }
    class ECSWorld {
        +createEntity()
        +registerSystem()
        +updateSystem()
        +query()
        +forEach()
        +clear()
        +getSystemCount()
    }
    class Entity {
        +with()
        +get()
        +has()
        +remove()
        +isValid()
    }

    %% Components
    class Transform
    class Velocity
    class Health
    class Weapon
    class Collider
    class Player
    class Enemy
    class Projectile
    class Wall
    class Buff
    class LuaScript
    class PendingDestroy
    class Animation
    class AnimationSet
    class Sprite
    class Spawner
    class Collectible
    class MapData

    %% Systems
    class MovementSystem
    class CollisionSystem
    class HealthSystem
    class MapSystem
    class WeaponSystem
    class SpawnSystem
    class AISystem
    class AnimationSystem
    class BuffSystem
    class BoundarySystem

    %% Relationships
    Registry "1" -- "*" Entity : manages
    Entity "1" -- "*" IComponent : has
    Registry "1" -- "*" ISystem : manages
    PrefabFactory "1" -- "*" Registry : uses
    ECSWorld "1" -- "*" Entity : manages
    ECSWorld "1" -- "*" ISystem : manages
    ECSWorld "1" -- "*" Registry : uses
    ISystem <|-- MovementSystem
    ISystem <|-- CollisionSystem
    ISystem <|-- HealthSystem
    ISystem <|-- MapSystem
    ISystem <|-- WeaponSystem
    ISystem <|-- SpawnSystem
    ISystem <|-- AISystem
    ISystem <|-- AnimationSystem
    ISystem <|-- BuffSystem
    ISystem <|-- BoundarySystem
    IComponent <|-- Transform
    IComponent <|-- Velocity
    IComponent <|-- Health
    IComponent <|-- Weapon
    IComponent <|-- Collider
    IComponent <|-- Player
    IComponent <|-- Enemy
    IComponent <|-- Projectile
    IComponent <|-- Wall
    IComponent <|-- Buff
    IComponent <|-- LuaScript
    IComponent <|-- PendingDestroy
    IComponent <|-- Animation
    IComponent <|-- AnimationSet
    IComponent <|-- Sprite
    IComponent <|-- Spawner
    IComponent <|-- Collectible
    IComponent <|-- MapData
```

---

## Legend
- **Registry**: Manages entities and components
- **Entity**: Holds components
- **IComponent**: Base interface for all components
- **ISystem**: Base interface for all systems
- **PrefabFactory**: Creates entities with predefined components
- **ECSWorld**: High-level ECS manager (server-side)
- **Systems**: Operate on entities with specific component combinations
- **Components**: Data attached to entities
