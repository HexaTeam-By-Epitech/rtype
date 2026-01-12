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

# Lobby messages
struct ListRooms {
  # Request list of available rooms
}

struct CreateRoom {
  roomName @0 :Text;
  maxPlayers @1 :UInt32;
  isPrivate @2 :Bool;
}

struct JoinRoom {
  roomId @0 :Text;
}

struct StartMatchmaking {
  # Request to enter matchmaking queue
}

struct CancelMatchmaking {
  # Request to leave matchmaking queue
}
