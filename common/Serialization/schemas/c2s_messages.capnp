@0xf1e2d3c4b5a69702;

using import "shared_types.capnp".Action;

# Client to Server messages
# All messages that flow from client to server

struct PlayerInput {
  sequenceId @0 :UInt32;
  actions @1 :List(Action);
}

struct JoinGame {
  playerName @0 :Text;
}

