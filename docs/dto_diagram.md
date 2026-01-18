# DTO (Data Transfer Object) Diagram

```mermaid
classDiagram
    %% DTOs for protocol messages
    class PlayerInput {
        +sequenceId : uint32_t
        +actions : vector<Action>
        +serialize()
        +deserialize()
    }
    class Action {
        +type : enum
    }
    class GameState {
        +entities : vector<EntityState>
        +serverTick : uint32_t
        +serialize()
        +deserialize()
    }
    class EntityState {
        +entityId : uint32_t
        +type : enum
        +x, y : float
        +health : int
        +currentAnimation : string
        +spriteRect : Rectangle
        +serialize()
        +deserialize()
    }
    class ChatMessage {
        +message : string
        +sender : string
        +serialize()
        +deserialize()
    }
    class RoomList {
        +rooms : vector<RoomInfo>
        +serialize()
        +deserialize()
    }
    class RoomInfo {
        +roomId : string
        +name : string
        +maxPlayers : uint32_t
        +isPrivate : bool
    }
    class RegisterAccount {
        +username : string
        +password : string
        +serialize()
        +deserialize()
    }
    class LoginRequest {
        +username : string
        +password : string
        +serialize()
        +deserialize()
    }
    class RegisterResponse {
        +success : bool
        +reason : string
        +serialize()
        +deserialize()
    }
    class LoginResponse {
        +success : bool
        +reason : string
        +serialize()
        +deserialize()
    }
    class GameStart {
        +yourEntityId : uint32_t
        +initialState : GameState
        +serialize()
        +deserialize()
    }
    class EntityDestroyed {
        +entityId : uint32_t
        +serialize()
        +deserialize()
    }
    class GamerulePacket {
        +rules : map<string, value>
        +serialize()
        +deserialize()
    }

    %% Relationships
    GameState "1" -- "*" EntityState : contains
    RoomList "1" -- "*" RoomInfo : contains
    GameStart "1" -- "1" GameState : contains
```

---

## Legend
- **DTOs**: Data structures for protocol messages
- **serialize/deserialize**: Methods for binary conversion
- **Relationships**: Aggregation (contains)
