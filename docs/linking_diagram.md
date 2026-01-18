# Linking Diagram (UML)

```mermaid
classDiagram
    %% High-level linking between major subsystems
    class Client {
        +GameLoop
        +Rendering
        +Replicator
        +EventBus
        +MenuSystem
    }
    class Server {
        +GameLogic
        +ECSWorld
        +EventBus
        +NetworkHost
    }
    class Replicator {
        +sendPacket()
        +receivePacket()
        +processMessages()
        +publish(NetworkEvent)
    }
    class GameLoop {
        +processInput()
        +render()
        +update()
        +handleNetworkMessage()
    }
    class GameLogic {
        +update()
        +spawnPlayer()
        +processPlayerInput()
        +_executeSystems()
    }
    class ECSWorld {
        +createEntity()
        +registerSystem()
        +updateSystem()
        +query()
        +forEach()
    }
    class EventBus {
        +subscribe()
        +publish()
    }
    class NetworkHost {
        +handlePacket()
        +sendPacket()
        +disconnectPeer()
    }

    %% Relationships
    Client "1" -- "1" Replicator : uses
    Client "1" -- "1" GameLoop : owns
    Client "1" -- "1" EventBus : owns
    Client "1" -- "1" MenuSystem : owns
    Replicator "1" -- "1" EventBus : publishes
    GameLoop "1" -- "1" EventBus : subscribes
    GameLoop "1" -- "1" Replicator : uses
    Server "1" -- "1" GameLogic : owns
    Server "1" -- "1" ECSWorld : owns
    Server "1" -- "1" EventBus : owns
    Server "1" -- "1" NetworkHost : owns
    GameLogic "1" -- "1" ECSWorld : uses
    GameLogic "1" -- "1" EventBus : publishes
    NetworkHost "1" -- "1" ECSWorld : uses
    NetworkHost "1" -- "1" GameLogic : uses
```

---

## Legend
- **Client**: Game client, owns GameLoop, Replicator, EventBus, MenuSystem
- **Server**: Game server, owns GameLogic, ECSWorld, EventBus, NetworkHost
- **Replicator**: Handles network packets, publishes events
- **GameLoop**: Main loop, processes input, rendering, network
- **GameLogic**: Server-side game logic, manages ECS
- **ECSWorld**: ECS manager, entity/system registry
- **EventBus**: Event system for decoupled communication
- **NetworkHost**: Network server, handles packets and connections
