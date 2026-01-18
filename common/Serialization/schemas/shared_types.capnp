@0xf1e2d3c4b5a69701;

# Shared types used across all game messages
# These are the fundamental building blocks

struct Vec2 {
  x @0 :Float32;
  y @1 :Float32;
}

enum Action {
  moveUp @0;
  moveDown @1;
  moveLeft @2;
  moveRight @3;
  shoot @4;
}

enum EntityType {
  player @0;
  enemyType1 @1;
  playerBullet @2;
  enemyBullet @3;
  wall @4;
  orbitalModule @5;
}

enum DestroyReason {
  killedByPlayer @0;
  outOfBounds @1;
  collision @2;
}
