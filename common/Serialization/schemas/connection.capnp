@0xf1e2d3c4b5a69788;

# Connection protocol messages for R-Type
# Handles client authentication, connection lifecycle, and kicks

struct HandshakeRequest {
  clientVersion @0 :Text;
  playerName @1 :Text;
  timestamp @2 :UInt64;
}

struct HandshakeResponse {
  accepted @0 :Bool;
  sessionId @1 :Text;
  serverId @2 :Text;
  message @3 :Text;           # Welcome message or rejection reason
  serverVersion @4 :Text;
  playerId @5 :UInt32;        # Player ID assigned by server
}

struct DisconnectNotification {
  reason @0 :DisconnectReason;
  message @1 :Text;           # Optional human-readable reason
  timestamp @2 :UInt64;
  
  enum DisconnectReason {
    clientRequest @0;         # Client requested disconnect
    serverShutdown @1;        # Server is shutting down
    timeout @2;               # Connection timeout
    kicked @3;                # Player was kicked
    error @4;                 # Protocol error
  }
}

struct KickMessage {
  reason @0 :Text;            # Reason for kick
  duration @1 :UInt64;        # Ban duration in seconds (0 = permanent)
  timestamp @2 :UInt64;
}

struct PingMessage {
  timestamp @0 :UInt64;
  sequenceNumber @1 :UInt32;
}

struct PongMessage {
  clientTimestamp @0 :UInt64;
  serverTimestamp @1 :UInt64;
  sequenceNumber @2 :UInt32;
}

