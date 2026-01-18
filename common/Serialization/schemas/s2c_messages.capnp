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
  lastProcessedInput @9 :UInt32; # Sequence ID of the last input processed for this entity (for prediction)
  velocity @10 :Vec2;         # Current velocity (for client-side extrapolation)
  rotation @11 :Float32;      # Current rotation angle (for interpolation)
}

struct GameState {
  serverTick @0 :UInt32;
  entities @1 :List(EntityState);
  serverTimestamp @2 :UInt64;  # Server timestamp in milliseconds (for interpolation)
}

struct MapConfig {
  background @0 :Text;           # Path to main background texture (e.g., "backgrounds/bg-full.png")
  parallaxBackground @1 :Text;   # Path to parallax layer texture (empty = none)
  scrollSpeed @2 :Float32;       # Background scroll speed in pixels/second
  parallaxSpeedFactor @3 :Float32; # Parallax layer speed factor (0.3 = 30% of main speed)
}

struct GameStart {
  yourEntityId @0 :UInt32;
  initialState @1 :GameState;
  mapConfig @2 :MapConfig;       # Map background configuration
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
  isSpectator @3 :Bool;  # True if joined as spectator (auto-spectator for IN_PROGRESS games)
}

struct PlayerData {
  playerId @0 :UInt32;
  playerName @1 :Text;
  isHost @2 :Bool;
  isSpectator @3 :Bool;
}

struct RoomState {
  roomId @0 :Text;
  roomName @1 :Text;
  currentPlayers @2 :UInt32;
  maxPlayers @3 :UInt32;
  players @4 :List(PlayerData);
  state @5 :UInt8;  # 0=WAITING, 1=STARTING, 2=IN_PROGRESS, 3=FINISHED
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

# Authentication responses
struct RegisterResponse {
  success @0 :Bool;
  message @1 :Text;  # Error message if failed
}

struct LoginResponse {
  success @0 :Bool;
  message @1 :Text;  # Error message if failed
  sessionToken @2 :Text;  # Token if successful
  autoMatchmaking @3 :Bool;  # User's auto-matchmaking preference
}
struct S2CChatMessage {
  playerId @0 :UInt32;
  playerName @1 :Text;
  message @2 :Text;
  timestamp @3 :UInt64;
}

enum LeftRoomReason {
  voluntaryLeave @0;  # Player pressed "back" or left voluntarily
  kicked @1;          # Player was kicked by host
  roomClosed @2;      # Room was closed/deleted
  serverShutdown @3;  # Server is shutting down
}

struct LeftRoom {
  playerId @0 :UInt32;
  reason @1 :LeftRoomReason;
  message @2 :Text;  # Optional message (e.g., "Kicked by host")
}
