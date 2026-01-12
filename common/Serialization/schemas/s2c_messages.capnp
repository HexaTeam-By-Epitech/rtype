@0xf1e2d3c4b5a69703;

using import "shared_types.capnp".Vec2;
using import "shared_types.capnp".EntityType;
using import "shared_types.capnp".DestroyReason;

# Server to Client messages
# All messages that flow from server to client

struct EntityState {
  entityId @0 :UInt32;
  type @1 :EntityType;
  position @2 :Vec2;
  health @3 :Int32;           # Use -1 for "no health" (bullets, etc.)
  currentAnimation @4 :Text;  # Current animation clip name (e.g., "idle", "shoot")
  spriteX @5 :Int32;          # Sprite source X on spritesheet
  spriteY @6 :Int32;          # Sprite source Y on spritesheet
  spriteW @7 :Int32;          # Sprite width
  spriteH @8 :Int32;          # Sprite height
}

struct GameState {
  serverTick @0 :UInt32;
  entities @1 :List(EntityState);
}

struct GameStart {
  yourEntityId @0 :UInt32;
  initialState @1 :GameState;
}

struct EntityDestroyed {
  entityId @0 :UInt32;
  reason @1 :DestroyReason;
}

struct GameOver {
  reason @0 :Text;
}

