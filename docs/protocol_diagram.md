# Protocol UML Diagrams

## 1. Connection Setup (Handshake)

```mermaid
sequenceDiagram
    participant Client
    participant ENet
    participant Server

    Client->>ENet: Connect (ENetPeer)
    ENet->>Server: Connection request
    Client->>Server: HANDSHAKE_REQUEST (playerName, username, password, timestamp)
    Server->>Client: HANDSHAKE_RESPONSE (success/failure, reason)
    Client->>Server: REGISTER_REQUEST / LOGIN_REQUEST (optional)
    Server->>Client: REGISTER_RESPONSE / LOGIN_RESPONSE
    Client->>Server: JOIN_ROOM / CREATE_ROOM / LIST_ROOMS
    Server->>Client: ROOM_LIST / ROOM_CREATED / JOINED_ROOM
```

---

## 2. Runtime Exchanges (Game Loop)

```mermaid
sequenceDiagram
    participant Client
    participant ENet
    participant Server

    loop Game Loop
        Client->>Server: C2S_PLAYER_INPUT (InputSnapshot)
        Server->>Client: S2C_GAME_STATE (EntityState, tick)
        Server->>Client: S2C_ENTITY_DESTROYED (entityId)
        Server->>Client: S2C_GAME_START (yourEntityId, initialState)
        Client->>Server: C2S_CHAT_MESSAGE (message)
        Server->>Client: S2C_CHAT_MESSAGE (message)
        Client->>Server: C2S_LEAVE_ROOM
        Server->>Client: S2C_LEFT_ROOM
        Client->>Server: C2S_AUTO_MATCHMAKING
        Server->>Client: S2C_ROOM_LIST
        Client->>Server: C2S_GAMERULE_UPDATE
        Server->>Client: S2C_GAMERULE_UPDATE
    end
```

---

## Legend
- **C2S**: Client to Server
- **S2C**: Server to Client
- **ENet**: Network transport layer
- **Handshake**: Initial authentication and setup
- **Game Loop**: Real-time gameplay packet exchange
- **Room Management**: Room creation, joining, listing
- **Chat**: In-game chat messages
- **Gamerule**: Game rule updates
