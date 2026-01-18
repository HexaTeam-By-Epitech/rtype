# Animation Bug Report: Walking Enemy Animation Not Working

## 📋 Problème

L'animation fonctionne pour le joueur mais **pas pour le mob "homing/walking"**.

---

## 🔍 Analyse

### Comment le joueur charge son animation

**Fichier:** `server/Game/Logic/GameLogic.cpp` (lignes 178-192)

```cpp
ecs::AnimationSet playerAnimations = AnimDB::createPlayerAnimations();
ecs::wrapper::Entity playerEntity =
    _world->createEntity()
        // ... autres composants ...
        .with(ecs::Sprite("PlayerShips.gif", {1, 69, 33, 14}, 3.0f, 0.0f, false, false, 0))  // ✅ SPRITE
        .with(playerAnimations)                                                              // ✅ ANIMATION SET
        .with(ecs::Animation("player_idle"));                                                // ✅ ANIMATION
```

**Le joueur a les 3 composants requis:**
1. ✅ `Sprite` - avec texture "PlayerShips.gif" et rectangle source initial
2. ✅ `AnimationSet` - contient les clips "player_idle" et "player_movement"
3. ✅ `Animation` - état de lecture actuel ("player_idle")

---

### Comment le walking mob charge son animation

**Fichier:** `common/ECS/Systems/SpawnSystem/SpawnSystem.cpp` (lignes 97-175)

Le `SpawnSystem::_spawnEnemy()` crée les ennemis **directement** sans utiliser le `PrefabFactory`:

```cpp
void SpawnSystem::_spawnEnemy(Registry &registry, const SpawnRequest &request) {
    Address enemy = registry.newEntity();
    
    registry.setComponent<Transform>(enemy, Transform(request.x, request.y));
    registry.setComponent<Velocity>(enemy, Velocity(...));
    registry.setComponent<Health>(enemy, Health(...));
    registry.setComponent<Enemy>(enemy, Enemy(...));
    registry.setComponent<Collider>(enemy, Collider(...));
    registry.setComponent<Weapon>(enemy, Weapon(...));
    registry.setComponent<LuaScript>(enemy, LuaScript(...));
    
    // ❌ PAS de Sprite
    // ❌ PAS d'AnimationSet  
    // ❌ PAS d'Animation
}
```

**Le PrefabFactory ajoute les composants d'animation mais il n'est PAS utilisé:**

```cpp
// Dans PrefabFactory::createEnemy() - NON APPELÉ par SpawnSystem
if (enemyType == 1) {  // Homing/walking enemy
    registry.setComponent(enemy, AnimDB::createEnemyWalkingAnimations());  // AnimationSet
    registry.setComponent(enemy, Animation("walk_left"));                   // Animation
    // ❌ MANQUE TOUJOURS le Sprite !
}
```

---

### Requirements de l'AnimationSystem

**Fichier:** `common/ECS/Systems/AnimationSystem/AnimationSystem.cpp` (ligne 81-83)

```cpp
ComponentMask AnimationSystem::getComponentMask() const {
    return (1ULL << getComponentType<Animation>()) | 
           (1ULL << getComponentType<AnimationSet>()) |
           (1ULL << getComponentType<Sprite>());  // ⚠️ SPRITE REQUIS !
}
```

L'`AnimationSystem` **requiert les 3 composants**:
- `Animation`
- `AnimationSet`
- `Sprite`

Sans le composant `Sprite`, l'entité n'est pas traitée par le système d'animation.

---

## 🎯 Cause Racine

**2 problèmes identifiés:**

| # | Problème | Impact |
|---|----------|--------|
| 1 | `SpawnSystem::_spawnEnemy()` ne passe pas par `PrefabFactory` | Les composants d'animation ne sont jamais ajoutés |
| 2 | Même le `PrefabFactory` n'ajoute pas de `Sprite` pour les ennemis avec animation | Même si utilisé, l'`AnimationSystem` ignorerait l'entité |

---

## ✅ Solution

Modifier `SpawnSystem::_spawnEnemy()` pour ajouter les 3 composants requis quand l'ennemi est de type "homing":

1. `AnimationSet` - via `AnimDB::createEnemyWalkingAnimations()`
2. `Animation` - avec le clip initial "walk_left"
3. `Sprite` - avec la texture "WalkingEnemy.gif" et le premier frame du clip

---

## 📊 Comparaison

| Composant | Joueur | Walking Mob (avant fix) |
|-----------|--------|------------------------|
| `Transform` | ✅ | ✅ |
| `Velocity` | ✅ | ✅ |
| `Health` | ✅ | ✅ |
| `Collider` | ✅ | ✅ |
| `Sprite` | ✅ PlayerShips.gif | ❌ **MANQUANT** |
| `AnimationSet` | ✅ player_idle/movement | ❌ **MANQUANT** |
| `Animation` | ✅ "player_idle" | ❌ **MANQUANT** |

**Après le fix:**

| Composant | Joueur | Walking Mob |
|-----------|--------|-------------|
| `Sprite` | ✅ PlayerShips.gif | ✅ WalkingEnemy.gif |
| `AnimationSet` | ✅ | ✅ walk_left/right, fly_left/right |
| `Animation` | ✅ "player_idle" | ✅ "walk_left" |
