@0xf1e2d3c4b5a69702;

using import "shared_types.capnp".Action;

# Client to Server messages
# All messages that flow from client to server

struct InputSnapshot {
  sequenceId @0 :UInt32;
  actions @1 :List(Action);
}

struct PlayerInput {
  inputs @0 :List(InputSnapshot);
}

struct JoinGame {
  playerName @0 :Text;
}

# Authentication messages
struct RegisterAccount {
  username @0 :Text;
  password @1 :Text;
}

struct LoginAccount {
  username @0 :Text;
  password @1 :Text;
}

# Lobby messages
struct ListRooms {
  # Request list of available rooms
}

struct CreateRoom {
  roomName @0 :Text;
  maxPlayers @1 :UInt32;
  isPrivate @2 :Bool;
  gameSpeedMultiplier @3 :Float32;  # Game speed multiplier (0.25 to 1.0, default 1.0)
}

struct JoinRoom {
  roomId @0 :Text;
}

struct LeaveRoom {
  # Request to leave current room
}

struct AutoMatchmaking {
  # Used with different message types for different behaviors:
  # - With C2S_AUTO_MATCHMAKING: request to join a room automatically;
  #   server will find an available room or create one (and may persist this preference).
  # - With C2S_UPDATE_AUTO_MM_PREF: only updates the user's auto-matchmaking
  #   preference without triggering matchmaking.
  enabled @0 :Bool;  # Whether auto-matchmaking is enabled
}

struct StartMatchmaking {
  # Request to enter matchmaking queue
}

struct CancelMatchmaking {
  # Request to leave matchmaking queue
}

struct C2SChatMessage {
  message @0 :Text;
}

