# Networking Module

This module provides a backend-agnostic networking interface (`IHost`, `IPeer`, 
`IPacket`, `IAddress`) and a concrete implementation based on ENet. The goal is 
to let the client and server exchange data over the network without knowing the 
details of the underlying networking library.

## Main Interfaces

### `IAddress`

Represents an abstract network address (IP address + port). Concrete 
implementations are provided by the backend (for example `ENetAddressWrapper`). 
Addresses are always handled through `IAddress` pointers.

**Key methods:**
- `getHost()` – Get the hostname or IP address as a string.
- `getPort()` – Get the port number.
- `setHost(host)` – Set the hostname or IP address.
- `setPort(port)` – Set the port number.

### `IPacket`

Represents an abstract network packet containing data to send or receive. 
Supports various delivery flags (reliable, unsequenced, etc.).

**Key methods:**
- `getData()` – Get the packet data as a byte buffer.
- `getSize()` – Get the size of the packet in bytes.
- `getFlags()` – Get the packet flags (reliable, unsequenced, etc.).
- `setData(data)` – Set new data for the packet.

**Packet flags:**
- `RELIABLE` – Packet must be received and resent if dropped.
- `UNSEQUENCED` – Packet will not be sequenced with other packets.
- `NO_ALLOCATE` – Packet will not allocate data.
- `UNRELIABLE_FRAGMENT` – Packet will be fragmented using unreliable sends.

### `IPeer`

Represents an abstract connection to a remote peer (client or server endpoint).

**Key methods:**
- `send(packet, channelID)` – Send a packet to this peer.
- `disconnect(data)` – Disconnect from this peer.
- `disconnectNow(data)` – Force immediate disconnect.
- `disconnectLater(data)` – Disconnect after all queued packets are sent.
- `getState()` – Get the current connection state.
- `getAddress()` – Get the peer's address.
- `getID()` – Get a unique identifier for this peer.
- `getRoundTripTime()` – Get the ping to this peer in milliseconds.
- `setData(data)` / `getData()` – Store/retrieve application-specific data.

**Peer states:**
- `DISCONNECTED`, `CONNECTING`, `CONNECTED`, `DISCONNECTING`, etc.

### `IHost`

Represents an abstract network host that manages connections and communication. 
Can act as a server accepting connections or as a client connecting to servers.

**Key methods:**
- `connect(address, channelCount, data)` – Connect to a remote host.
- `service(timeout)` – Process network events (returns `NetworkEvent`).
- `broadcast(packet, channelID)` – Broadcast a packet to all connected peers.
- `flush()` – Send all queued packets immediately.
- `getPeerCount()` – Get the number of connected peers.
- `getAddress()` – Get the address this host is bound to.

**Network events:**
- `CONNECT` – A peer has connected.
- `DISCONNECT` – A peer has disconnected.
- `RECEIVE` – A packet was received.
- `NONE` – No event occurred.

### Factory Functions

Utility functions exposed in `NetworkFactory.hpp` that construct networking 
objects. Currently they return ENet-backed implementations, but the 
implementation can be swapped without changing client or server code.

- `initializeNetworking()` – Initialize the networking subsystem (must be 
  called before using any networking functions).
- `deinitializeNetworking()` – Cleanup the networking subsystem.
- `createServerHost(address, maxClients, ...)` – Create a server host that 
  listens for incoming connections.
- `createClientHost(channelLimit, ...)` – Create a client host that can 
  connect to servers.
- `createPacket(data, flags)` – Create a network packet with the given data 
  and flags.
- `createAddress(host, port)` – Create a network address.

## ENet Implementation (Internal)

The `enet/` folder contains the concrete implementation based on ENet:

- `ENetHostWrapper` – Implements `IHost` using ENet.
- `ENetPeerWrapper` – Implements `IPeer` wrapping an `ENetPeer`.
- `ENetPacketWrapper` – Implements `IPacket` wrapping an `ENetPacket`.
- `ENetAddressWrapper` – Implements `IAddress` wrapping an `ENetAddress`.

These classes must not be used directly by the `client/` or `server/` modules 
in order to keep a strong separation between gameplay and the networking backend.

## Usage Examples

### Initialization

```cpp
#include "Networking/NetworkFactory.hpp"

// Initialize networking subsystem (call once at startup)
if (!initializeNetworking()) {
    // Handle error
}

// ... use networking ...

// Cleanup (call once at shutdown)
deinitializeNetworking();
```

### Creating a Server

```cpp
#include "Networking/NetworkFactory.hpp"
#include "Networking/IHost.hpp"

// Create server address
auto address = createAddress("0.0.0.0", 7777);

// Create server host
auto server = createServerHost(*address, 32); // max 32 clients

// Main loop
while (running) {
    auto event = server->service(1000); // Wait up to 1 second
    
    if (event) {
        switch (event->type) {
            case EventType::CONNECT:
                std::cout << "Client connected\n";
                break;
                
            case EventType::DISCONNECT:
                std::cout << "Client disconnected\n";
                break;
                
            case EventType::RECEIVE:
                // Process received packet
                auto& data = event->packet->getData();
                // ... handle data ...
                break;
        }
    }
}
```

### Creating a Client

```cpp
#include "Networking/NetworkFactory.hpp"
#include "Networking/IHost.hpp"

// Create client host
auto client = createClientHost();

// Create server address
auto serverAddr = createAddress("127.0.0.1", 7777);

// Connect to server
IPeer* peer = client->connect(*serverAddr);

if (peer) {
    // Wait for connection to establish
    while (running) {
        auto event = client->service(1000);
        
        if (event && event->type == EventType::CONNECT) {
            std::cout << "Connected to server\n";
            break;
        }
    }
}
```

### Sending Data

```cpp
// Create packet with data
std::vector<uint8_t> data = {1, 2, 3, 4, 5};
auto packet = createPacket(data, static_cast<uint32_t>(PacketFlag::RELIABLE));

// Send to specific peer
peer->send(std::move(packet), 0); // channel 0

// Or broadcast to all peers (server only)
auto broadcastPacket = createPacket(data, static_cast<uint32_t>(PacketFlag::RELIABLE));
server->broadcast(std::move(broadcastPacket), 0);
```

### Receiving Data

```cpp
auto event = host->service(1000);

if (event && event->type == EventType::RECEIVE) {
    const auto& data = event->packet->getData();
    size_t size = event->packet->getSize();
    
    // Process the received data
    // ...
}
```

## Integration with Serialization Module

The Networking module works seamlessly with the Serialization module:

```cpp
#include "Networking/NetworkFactory.hpp"
#include "Serialization/GameCodec.hpp"
#include "Serialization/SerializerFactory.hpp"

// Create codec and networking
auto codec = createDefaultGameCodec();
auto server = createServerHost(*createAddress("0.0.0.0", 7777), 32);

// Encode and send a game message
SpawnEntityMessage msg;
msg.id = 42;
msg.x = 100.f;
msg.y = 200.f;

std::vector<uint8_t> bytes = codec->encodeSpawn(msg);
auto packet = createPacket(bytes, static_cast<uint32_t>(PacketFlag::RELIABLE));
server->broadcast(std::move(packet), 0);

// Receive and decode
auto event = server->service(1000);
if (event && event->type == EventType::RECEIVE) {
    const auto& data = event->packet->getData();
    auto gameMsg = codec->decodeSpawn(data);
    // Use gameMsg...
}
```

## Benefits

- **Backend Independence**: Switch from ENet to another library without 
  changing client/server code.
- **Type Safety**: Strong typing through interfaces prevents many common 
  networking bugs.
- **Memory Safety**: Smart pointers ensure proper resource management.
- **Testability**: Easy to mock interfaces for unit testing.
- **Maintainability**: Clear separation between game logic and networking 
  implementation.
