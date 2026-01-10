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

# Lobby messages
struct RoomInfo {
  roomId @0 :Text;
  roomName @1 :Text;
  playerCount @2 :UInt32;
  maxPlayers @3 :UInt32;
  isPrivate @4 :Bool;
  state @5 :UInt8;  # 0=WAITING, 1=STARTING, 2=IN_PROGRESS, 3=FINISHED
}

struct RoomList {
  rooms @0 :List(RoomInfo);
}

struct RoomCreated {
  roomId @0 :Text;
  success @1 :Bool;
  errorMessage @2 :Text;
}

struct JoinedRoom {
  roomId @0 :Text;
  success @1 :Bool;
  errorMessage @2 :Text;
}

struct MatchmakingStarted {
  success @0 :Bool;
  errorMessage @1 :Text;
}

struct MatchFound {
  roomId @0 :Text;
}

struct Gamerule {
  key @0 :Text;
  value @1 :Float32;
}

struct GamerulePacket {
  gamerules @0 :List(Gamerule);
}

