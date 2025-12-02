# Serialization module

This module provides a backend-agnostic binary serialization interface
(`IMessage`, `ISerializer`) and a concrete implementation based on Cap'n Proto.
The goal is to let the client and the server exchange data without knowing the
details of the underlying serialization library.

## Main interfaces

### `IMessage`

Represents an abstract serializable message. Concrete implementations are
provided either by the backend (for example `CapnpMessage`) or by the gameplay
layer. Messages are always handled through `IMessage` pointers.

### `ISerializer`

Generic interface that can:

- `encode(const IMessage&)` – encode a message into a `std::vector<uint8_t>`.
- `decode(const std::vector<uint8_t>&)` – rebuild an `IMessage` from a binary
  buffer.

Gameplay code depends only on this interface and standard STL types.

### `createDefaultSerializer()`

Utility function exposed in `SerializerFactory.hpp` that constructs the
engine's default serializer. Currently it returns a `CapnpSerializer`, but
the implementation can be swapped (different backend) without changing client
or server code.

### `IGameCodec` and `SpawnEntityMessage`

On top of the low-level `ISerializer` interface, the module also exposes a
high-level gameplay-oriented codec:

- `SpawnEntityMessage` – an example DTO that derives from `IMessage` and
  represents a gameplay intent (spawn an entity with an `id`, `x`, `y`).
- `IGameCodec` – an interface that knows how to encode/decode such gameplay
  messages to/from raw bytes.
- `createDefaultGameCodec()` – factory that returns a Cap'n Proto-backed
  implementation (`CapnpGameCodec`).

Gameplay code can interact only with these types and remain agnostic of
Cap'n Proto entirely.

## Cap'n Proto implementation (internal)

The `capnp/` folder contains the concrete implementation based on Cap'n Proto:

- `CapnpSerializer` – implements `ISerializer` using Cap'n Proto.
- `CapnpMessage` – `IMessage` implementation that stores an already-serialized
  binary buffer.
- `CapnpBuilderMessage` – internal implementation that builds a Cap'n Proto
  message via a `capnp::MallocMessageBuilder`.
- `CapnpGameCodec` – implementation of `IGameCodec` that bridges
  `SpawnEntityMessage` and the Cap'n Proto backend.

These classes must not be used directly by the `client/` or `server/` modules
in order to keep a strong separation between gameplay and the serialization
backend.

## Usage example

### Creating a serializer

```cpp
#include "serialization/SerializerFactory.hpp"
#include "serialization/ISerializer.hpp"

// ...

std::unique_ptr<ISerializer> serializer = createDefaultSerializer();
```

### Creating a gameplay codec

```cpp
#include "serialization/GameCodec.hpp"
#include "serialization/SerializerFactory.hpp"

// ...

std::unique_ptr<IGameCodec> codec = createDefaultGameCodec();
```

### Encoding a gameplay message via `IGameCodec`

```cpp
SpawnEntityMessage msg;
msg.id = 42;
msg.x  = 100.f;
msg.y  = 200.f;

std::vector<uint8_t> bytes = codec->encodeSpawn(msg);
// socketSend(bytes.data(), bytes.size());
```

### Decoding on receive via `IGameCodec`

```cpp
std::vector<uint8_t> received = /* data received from the network */;
SpawnEntityMessage msg = codec->decodeSpawn(received);

// Use msg.id, msg.x, msg.y in gameplay systems
```

## Adding a new message type

To add a new gameplay message (for example `MoveEntityMessage`), the steps are:

1. **Define the DTO**

   Create a new class that derives from `IMessage` in `GameMessages.hpp`:

   ```cpp
   class MoveEntityMessage : public IMessage {
   public:
       uint32_t id;
       float x;
       float y;
   };
   ```

2. **Extend the codec interface**

   Add encode/decode methods to `IGameCodec`:

   ```cpp
   virtual std::vector<uint8_t> encodeMove(const MoveEntityMessage& message) = 0;
   virtual MoveEntityMessage decodeMove(const std::vector<uint8_t>& data) = 0;
   ```

3. **Implement the mapping in `CapnpGameCodec`**

   In `CapnpGameCodec.cpp`, use Cap'n Proto schemas and `CapnpBuilderMessage`
   to bridge between `MoveEntityMessage` and the Cap'n Proto representation:

   ```cpp
   std::vector<uint8_t> CapnpGameCodec::encodeMove(const MoveEntityMessage& message) {
       auto iMsg = makeCapnpBuilderMessage([
           id = message.id,
           x  = message.x,
           y  = message.y
       ](capnp::MallocMessageBuilder& builder) {
           // auto root = builder.initRoot<game::MoveEntity>();
           // root.setId(id);
           // root.setX(x);
           // root.setY(y);
       });
       return _serializer.encode(*iMsg);
   }

   MoveEntityMessage CapnpGameCodec::decodeMove(const std::vector<uint8_t>& data) {
       // Example pattern using a Cap'n Proto reader:
       //
       // kj::ArrayPtr<const capnp::word> words(
       //     reinterpret_cast<const capnp::word*>(data.data()),
       //     data.size() / sizeof(capnp::word));
       // capnp::FlatArrayMessageReader reader(words);
       // auto root = reader.getRoot<game::MoveEntity>();
       //
       // MoveEntityMessage msg;
       // msg.id = root.getId();
       // msg.x  = root.getX();
       // msg.y  = root.getY();
       // return msg;
       MoveEntityMessage msg;
       (void)data;
       return msg;
   }
   ```

4. **Keep Cap'n Proto internal**

   All Cap'n Proto headers and schemas are included only inside the
   `serialization` module (e.g. `CapnpGameCodec.cpp`). Client and server code
   only ever depend on `IMessage`, `ISerializer`, `IGameCodec` and the DTO
   classes in `GameMessages.hpp`.

By following this pattern, you can grow the set of gameplay messages without
exposing Cap'n Proto outside of the serialization module, while reusing the
same encode/decode patterns across all message types.
