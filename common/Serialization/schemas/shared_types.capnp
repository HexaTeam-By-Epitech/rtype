@0xf1e2d3c4b5a69701;

# Shared types used across all game messages
# These are the fundamental building blocks

struct Vec2 {
  x @0 :Float32;
  y @1 :Float32;
}

enum Action {
  MoveUp @0;
  MoveDown @1;
  MoveLeft @2;
  MoveRight @3;
  Shoot @4;
}

enum EntityType {
  Player @0;
  EnemyType1 @1;
  PlayerBullet @2;
  EnemyBullet @3;
}

enum DestroyReason {
  KilledByPlayer @0;
  OutOfBounds @1;
  Collision @2;
}
