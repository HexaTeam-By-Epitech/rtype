# 🗺️ Roadmap Système Lua Scripting - R-Type Server

## 📊 Vue d'ensemble

**Objectif** : Intégrer un système de scripting Lua côté serveur via sol2 pour permettre des comportements d'entités scriptables.

**Architecture** :
- `common/ECS/Components/LuaScript.hpp` ✅ (Fait)
- `common/ECS/Systems/LuaSystem/` ✅ (Fait)
- `server/Scripting/LuaEngine` (En cours)
- `server/Scripting/LuaBindings/` (À faire)
- `server/Scripting/scripts/` (À faire)

**Dépendances** :
- ✅ `vcpkg.json` - sol2 et lua ajoutés
- ✅ `server/CMakeLists.txt` - find_package configuré
- ✅ Compilation de base OK

---

## 📋 Tâches Détaillées

### **BLOC A - LuaEngine Core** (Critique - 2-3h)

#### **A1. Implémenter `bindRegistry()` dans LuaEngine.cpp**
**Fichier** : `server/Scripting/LuaEngine.cpp`  
**Priorité** : 🔴 CRITIQUE  
**Temps estimé** : 45min  
**Dépendances** : Aucune

**Détails** :
```cpp
void LuaEngine::bindRegistry() {
    // Fonction 1 : getComponent(entityAddr, componentType) -> Component
    _lua.set_function("getComponent", [this](ecs::Address entity, const std::string& type) -> sol::object {
        if (!_registry) return sol::nil;
        
        // Switch sur les types de composants
        if (type == "Transform") {
            return sol::make_object(_lua, std::ref(_registry->getComponent<ecs::Transform>(entity)));
        } else if (type == "Velocity") {
            return sol::make_object(_lua, std::ref(_registry->getComponent<ecs::Velocity>(entity)));
        } else if (type == "Health") {
            return sol::make_object(_lua, std::ref(_registry->getComponent<ecs::Health>(entity)));
        }
        return sol::nil;
    });
    
    // Fonction 2 : destroyEntity(entityAddr)
    _lua.set_function("destroyEntity", [this](ecs::Address entity) {
        if (_registry) {
            _registry->killEntity(entity);
        }
    });
}
```

**Tests à faire** :
- Vérifier que `getComponent` retourne bien un composant modifiable
- Tester avec un composant inexistant (doit retourner nil)
- Vérifier que `destroyEntity` ne crash pas

---

#### **A2. Implémenter `loadScript()` dans LuaEngine.cpp**
**Fichier** : `server/Scripting/LuaEngine.cpp`  
**Priorité** : 🔴 CRITIQUE  
**Temps estimé** : 30min  
**Dépendances** : A1 (pour les tests)

**Détails** :
```cpp
bool LuaEngine::loadScript(const std::string& scriptPath) {
    std::string fullPath = _scriptsPath + "/" + scriptPath;
    
    // 1. Vérifier existence du fichier
    if (!std::filesystem::exists(fullPath)) {
        LOG_ERROR("Lua script not found: " + fullPath);
        return false;
    }
    
    // 2. Charger et exécuter le script
    try {
        sol::table script = _lua.script_file(fullPath);
        _scriptCache[scriptPath] = script;
        LOG_INFO("Loaded Lua script: " + scriptPath);
        return true;
    } catch (const sol::error& e) {
        LOG_ERROR("Lua error loading " + scriptPath + ": " + std::string(e.what()));
        return false;
    }
}
```

**Tests à faire** :
- Charger un script valide → doit retourner true
- Charger un script inexistant → doit retourner false + log
- Charger un script avec erreur syntaxe → doit catch + log

---

#### **A3. Implémenter `executeUpdate()` dans LuaEngine.cpp**
**Fichier** : `server/Scripting/LuaEngine.cpp`  
**Priorité** : 🔴 CRITIQUE  
**Temps estimé** : 45min  
**Dépendances** : A1, A2

**Détails** :
```cpp
void LuaEngine::executeUpdate(const std::string& scriptPath, ecs::Address entityAddress, float deltaTime) {
    if (!_registry) {
        LOG_ERROR("Registry not set in LuaEngine");
        return;
    }
    
    // 1. Charger script si pas en cache
    if (_scriptCache.find(scriptPath) == _scriptCache.end()) {
        if (!loadScript(scriptPath)) {
            return;
        }
    }
    
    // 2. Exécuter onUpdate()
    try {
        sol::table script = _scriptCache[scriptPath];
        sol::optional<sol::function> onUpdate = script["onUpdate"];
        
        if (onUpdate) {
            onUpdate.value()(entityAddress, deltaTime);
        } else {
            LOG_WARNING("Script " + scriptPath + " has no onUpdate function");
        }
    } catch (const sol::error& e) {
        LOG_ERROR("Lua runtime error in " + scriptPath + ": " + std::string(e.what()));
    }
}
```

**Tests à faire** :
- Exécuter script avec `onUpdate()` valide
- Tester avec script sans `onUpdate()` → log warning
- Tester avec erreur runtime Lua → catch + log

---

#### **A4. Implémenter template `callFunction()` (Optionnel)**
**Fichier** : `server/Scripting/LuaEngine.cpp`  
**Priorité** : 🟡 OPTIONNEL  
**Temps estimé** : 20min  
**Dépendances** : A2

**Détails** :
Permet d'appeler n'importe quelle fonction Lua avec arguments variés.

```cpp
template<typename... Args>
void LuaEngine::callFunction(const std::string& scriptPath, 
                             const std::string& functionName, 
                             Args&&... args) {
    if (_scriptCache.find(scriptPath) == _scriptCache.end()) {
        if (!loadScript(scriptPath)) return;
    }
    
    try {
        sol::table script = _scriptCache[scriptPath];
        sol::optional<sol::function> func = script[functionName];
        
        if (func) {
            func.value()(std::forward<Args>(args)...);
        }
    } catch (const sol::error& e) {
        LOG_ERROR("Lua error calling " + functionName + ": " + e.what());
    }
}
```

---

### **BLOC B - LuaBindings Extensions** (Moyen - 2-3h)

#### **B1. EntityBindings.cpp - Helpers entités**
**Fichier** : `server/Scripting/LuaBindings/EntityBindings.cpp` (À créer)  
**Priorité** : 🟠 IMPORTANT  
**Temps estimé** : 1h  
**Dépendances** : A1

**Fonctions à implémenter** :
```cpp
#include "EntityBindings.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Components/Velocity.hpp"

namespace scripting {
    void bindEntityHelpers(sol::state& lua, ecs::Registry* registry) {
        // 1. createEntity() -> Address
        lua.set_function("createEntity", [registry]() -> ecs::Address {
            return registry->spawnEntity();
        });
        
        // 2. hasComponent(addr, "Type") -> bool
        lua.set_function("hasComponent", [registry](ecs::Address addr, const std::string& type) -> bool {
            if (type == "Transform") {
                return registry->hasComponent<ecs::Transform>(addr);
            } else if (type == "Velocity") {
                return registry->hasComponent<ecs::Velocity>(addr);
            } else if (type == "Health") {
                return registry->hasComponent<ecs::Health>(addr);
            }
            return false;
        });
        
        // 3. entityExists(addr) -> bool
        lua.set_function("entityExists", [registry](ecs::Address addr) -> bool {
            return registry->isEntityAlive(addr);
        });
    }
}
```

**Tests Lua** :
```lua
local entity = createEntity()
print("Entity created: " .. entity)
print("Has Transform: " .. tostring(hasComponent(entity, "Transform")))
```

---

#### **B2. RegistryBindings.cpp - Opérations Registry avancées**
**Fichier** : `server/Scripting/LuaBindings/RegistryBindings.cpp` (À créer)  
**Priorité** : 🟠 IMPORTANT  
**Temps estimé** : 1h  
**Dépendances** : A1, B1

**Fonctions à implémenter** :
```cpp
#include "RegistryBindings.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Components/Velocity.hpp"
#include "common/ECS/Components/Health.hpp"

namespace scripting {
    void bindRegistryOperations(sol::state& lua, ecs::Registry* registry) {
        // 1. addTransform(addr, x, y)
        lua.set_function("addTransform", [registry](ecs::Address addr, float x, float y) {
            registry->addComponent<ecs::Transform>(addr, ecs::Transform(x, y));
        });
        
        // 2. addVelocity(addr, dirX, dirY, speed)
        lua.set_function("addVelocity", [registry](ecs::Address addr, float dx, float dy, float speed) {
            registry->addComponent<ecs::Velocity>(addr, ecs::Velocity(dx, dy, speed));
        });
        
        // 3. addHealth(addr, maxHealth)
        lua.set_function("addHealth", [registry](ecs::Address addr, int maxHealth) {
            registry->addComponent<ecs::Health>(addr, ecs::Health(maxHealth));
        });
        
        // 4. removeComponent(addr, "Type")
        lua.set_function("removeComponent", [registry](ecs::Address addr, const std::string& type) {
            if (type == "Transform") {
                registry->removeComponent<ecs::Transform>(addr);
            } else if (type == "Velocity") {
                registry->removeComponent<ecs::Velocity>(addr);
            }
        });
    }
}
```

**Tests Lua** :
```lua
local entity = createEntity()
addTransform(entity, 100, 200)
addVelocity(entity, 1, 0, 50)
addHealth(entity, 100)
```

---

#### **B3. ServerGameBindings.cpp - Logique serveur custom**
**Fichier** : `server/Scripting/LuaBindings/ServerGameBindings.cpp` (À créer)  
**Priorité** : 🟡 MOYEN  
**Temps estimé** : 1h  
**Dépendances** : B1, B2

**Fonctions à implémenter** :
```cpp
#include "ServerGameBindings.hpp"
#include "common/ECS/Components/Projectile.hpp"
#include "common/Logger/Logger.hpp"

namespace scripting {
    void bindServerGameLogic(sol::state& lua, ecs::Registry* registry) {
        // 1. spawnProjectile(x, y, dirX, dirY, speed)
        lua.set_function("spawnProjectile", [registry](float x, float y, float dx, float dy, float speed) -> ecs::Address {
            auto addr = registry->spawnEntity();
            registry->addComponent<ecs::Transform>(addr, ecs::Transform(x, y));
            registry->addComponent<ecs::Velocity>(addr, ecs::Velocity(dx, dy, speed));
            registry->addComponent<ecs::Projectile>(addr, ecs::Projectile(10)); // dmg = 10
            LOG_INFO("Projectile spawned from Lua at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
            return addr;
        });
        
        // 2. log(message) - Debug Lua
        lua.set_function("log", [](const std::string& msg) {
            LOG_INFO("[LUA] " + msg);
        });
        
        // 3. random(min, max) - Random number
        lua.set_function("random", [](float min, float max) -> float {
            return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
        });
    }
}
```

**Tests Lua** :
```lua
log("Spawning projectile from Lua!")
local proj = spawnProjectile(100, 100, 1, 0, 200)
log("Projectile entity: " .. proj)
```

---

#### **B4. Appeler les bindings dans LuaEngine**
**Fichier** : `server/Scripting/LuaEngine.cpp`  
**Priorité** : 🟠 IMPORTANT  
**Temps estimé** : 15min  
**Dépendances** : B1, B2, B3

**Modifications** :
```cpp
#include "LuaBindings/EntityBindings.hpp"
#include "LuaBindings/RegistryBindings.hpp"
#include "LuaBindings/ServerGameBindings.hpp"

void LuaEngine::initializeBindings() {
    bindComponents();
    bindRegistry();
    
    // Ajouter les nouveaux bindings
    bindEntityHelpers(_lua, _registry);
    bindRegistryOperations(_lua, _registry);
    bindServerGameLogic(_lua, _registry);
}
```

---

### **BLOC C - Scripts Lua de Test** (Facile - 1h)

#### **C1. test_movement.lua - Déplacement simple**
**Fichier** : `server/Scripting/scripts/test_movement.lua` (À créer)  
**Priorité** : 🟠 IMPORTANT  
**Temps estimé** : 15min  
**Dépendances** : A3

```lua
-- Script de test : Déplace une entité vers la droite

function onUpdate(entity, deltaTime)
    local transform = getComponent(entity, "Transform")
    local velocity = getComponent(entity, "Velocity")
    
    if transform and velocity then
        -- Déplacement simple
        transform.x = transform.x + velocity.dirX * velocity.speed * deltaTime
        transform.y = transform.y + velocity.dirY * velocity.speed * deltaTime
        
        -- Détruire si hors écran
        if transform.x > 800 then
            log("Entity out of bounds, destroying...")
            destroyEntity(entity)
        end
    end
end
```

---

#### **C2. enemy_simple.lua - IA ennemi basique**
**Fichier** : `server/Scripting/scripts/enemy_simple.lua` (À créer)  
**Priorité** : 🟡 MOYEN  
**Temps estimé** : 20min  
**Dépendances** : A3, B3

```lua
-- IA simple : Mouvement sinusoïdal + tir périodique

local shootTimer = 0
local shootInterval = 1.5  -- Tire toutes les 1.5 secondes

function onUpdate(entity, deltaTime)
    local transform = getComponent(entity, "Transform")
    local velocity = getComponent(entity, "Velocity")
    
    if not transform or not velocity then return end
    
    -- Mouvement sinusoïdal
    local time = transform.x * 0.01
    transform.y = transform.y + math.sin(time) * 50 * deltaTime
    
    -- Déplacement horizontal
    transform.x = transform.x + velocity.dirX * velocity.speed * deltaTime
    
    -- Système de tir
    shootTimer = shootTimer + deltaTime
    if shootTimer >= shootInterval then
        shootTimer = 0
        spawnProjectile(transform.x, transform.y, -1, 0, 150)
        log("Enemy fired projectile!")
    end
end
```

---

#### **C3. auto_destroy.lua - Test destruction temporisée**
**Fichier** : `server/Scripting/scripts/auto_destroy.lua` (À créer)  
**Priorité** : 🟢 FAIBLE  
**Temps estimé** : 10min  
**Dépendances** : A3

```lua
-- Script de test : Se détruit après 5 secondes

local lifetime = 5.0
local elapsed = 0

function onUpdate(entity, deltaTime)
    elapsed = elapsed + deltaTime
    
    if elapsed >= lifetime then
        log("Auto-destroy after " .. elapsed .. " seconds")
        destroyEntity(entity)
    end
end
```

---

#### **C4. health_regen.lua - Régénération de vie**
**Fichier** : `server/Scripting/scripts/health_regen.lua` (À créer)  
**Priorité** : 🟢 FAIBLE  
**Temps estimé** : 15min  
**Dépendances** : A3

```lua
-- Régénération de vie progressive

local regenRate = 5  -- HP par seconde

function onUpdate(entity, deltaTime)
    local health = getComponent(entity, "Health")
    
    if health then
        local current = health:getCurrentHealth()
        local max = health:getMaxHealth()
        
        if current < max then
            local newHealth = math.min(current + regenRate * deltaTime, max)
            health:setCurrentHealth(math.floor(newHealth))
            
            if newHealth >= max then
                log("Health fully regenerated!")
            end
        end
    end
end
```

---

### **BLOC D - Intégration Serveur** (Critique - 1h)

#### **D1. Instancier LuaEngine dans le serveur**
**Fichier** : `server/Server/Server.cpp` ou équivalent  
**Priorité** : 🔴 CRITIQUE  
**Temps estimé** : 20min  
**Dépendances** : A1-A3

**Détails** :
```cpp
// Dans Server.hpp
#include "server/Scripting/LuaEngine.hpp"

class Server {
private:
    std::unique_ptr<scripting::LuaEngine> _luaEngine;
    ecs::Registry _registry;
    // ...
};

// Dans Server.cpp (constructeur)
Server::Server() {
    _luaEngine = std::make_unique<scripting::LuaEngine>("server/Scripting/scripts");
    _luaEngine->setRegistry(&_registry);
}
```

---

#### **D2. Ajouter LuaSystem au SystemScheduler**
**Fichier** : `server/Server/Server.cpp` ou `Game/GameLogic.cpp`  
**Priorité** : 🔴 CRITIQUE  
**Temps estimé** : 20min  
**Dépendances** : D1, A3

**Détails** :
```cpp
// Dans l'init du serveur
#include "common/ECS/Systems/LuaSystem/LuaSystem.hpp"

// Ajouter le système
auto luaSystem = std::make_shared<ecs::LuaSystem>();
_world->registerSystem(ecs::SystemId::Lua, luaSystem);

// Dans SystemScheduler
_scheduler.registerSystem("LuaSystem", 10)  // Priorité 10
         .runAfter("LuaSystem", "MovementSystem");
```

---

#### **D3. Connecter LuaSystem avec LuaEngine**
**Fichier** : `common/ECS/Systems/LuaSystem/LuaSystem.cpp`  
**Priorité** : 🔴 CRITIQUE  
**Temps estimé** : 20min  
**Dépendances** : D1, D2

**Modifications** :
```cpp
// Option 1 : Injection via constructeur
class LuaSystem : public ISystem {
private:
    scripting::LuaEngine* _luaEngine;
public:
    explicit LuaSystem(scripting::LuaEngine* engine) : _luaEngine(engine) {}
    
    void update(Registry& registry, float deltaTime) override {
        auto entities = registry.getEntitiesWithMask(getComponentMask());
        for (const auto& entity : entities) {
            auto& script = registry.getComponent<LuaScript>(entity);
            _luaEngine->executeUpdate(script.getScriptPath(), entity, deltaTime);
        }
    }
};

// Instanciation
auto luaSystem = std::make_shared<ecs::LuaSystem>(_luaEngine.get());
```

---

### **BLOC E - Tests & Validation** (1h)

#### **E1. Test entité scriptée basique**
**Priorité** : 🔴 CRITIQUE  
**Temps estimé** : 20min  

```cpp
// Dans le code serveur (après init)
auto testEntity = _registry.spawnEntity();
_registry.addComponent<ecs::Transform>(testEntity, ecs::Transform(100, 100));
_registry.addComponent<ecs::Velocity>(testEntity, ecs::Velocity(1, 0, 50));
_registry.addComponent<ecs::LuaScript>(testEntity, ecs::LuaScript("test_movement.lua"));
```

**Vérifications** :
- [ ] L'entité se déplace correctement
- [ ] Logs Lua apparaissent dans la console
- [ ] Pas de crash au runtime

---

#### **E2. Test gestion erreurs Lua**
**Priorité** : 🟠 IMPORTANT  
**Temps estimé** : 15min  

Créer un script avec erreur volontaire :
```lua
-- bad_script.lua
function onUpdate(entity, deltaTime)
    thisVariableDoesNotExist.x = 10  -- ERREUR
end
```

**Vérifications** :
- [ ] Erreur catchée proprement
- [ ] Log d'erreur clair
- [ ] Serveur ne crash pas

---

#### **E3. Test hot-reload**
**Priorité** : 🟡 MOYEN  
**Temps estimé** : 15min  

**Procédure** :
1. Lancer serveur avec entité scriptée
2. Modifier le script Lua pendant runtime
3. Recharger le script (invalider cache ou auto-reload)

**Vérifications** :
- [ ] Modifications prises en compte sans redémarrage

---

#### **E4. Test performance (10+ entités scriptées)**
**Priorité** : 🟢 FAIBLE  
**Temps estimé** : 10min  

Créer 50 entités avec scripts :
```cpp
for (int i = 0; i < 50; i++) {
    auto entity = _registry.spawnEntity();
    _registry.addComponent<ecs::LuaScript>(entity, ecs::LuaScript("enemy_simple.lua"));
    // ...
}
```

**Vérifications** :
- [ ] FPS acceptable (>30)
- [ ] Pas de leak mémoire

---

## 🎯 Milestones

### **Milestone 1 : LuaEngine Fonctionnel** ✅
- [x] Composant LuaScript créé
- [x] LuaSystem interface créée
- [x] Bindings composants de base
- [ ] `bindRegistry()` implémenté
- [ ] `loadScript()` implémenté
- [ ] `executeUpdate()` implémenté

### **Milestone 2 : Extensions & Scripts**
- [ ] Bindings entités (B1)
- [ ] Bindings registry avancés (B2)
- [ ] Bindings serveur custom (B3)
- [ ] 2+ scripts Lua de test

### **Milestone 3 : Intégration Complète**
- [ ] LuaEngine instancié dans serveur
- [ ] LuaSystem ajouté au scheduler
- [ ] Test entité scriptée qui fonctionne

### **Milestone 4 : Production Ready**
- [ ] Gestion erreurs robuste
- [ ] Documentation API Lua
- [ ] Tests de performance OK

---

## 📦 Répartition Recommandée

### **Profil 1 - C++ Core Developer**
- BLOC A (LuaEngine Core)
- BLOC D (Intégration Serveur)
- E1-E2 (Tests critiques)

### **Profil 2 - Scripting/Gameplay Developer**
- BLOC B (LuaBindings)
- BLOC C (Scripts Lua)
- E3-E4 (Tests avancés)

---

## ⏱️ Estimation Totale

| Bloc | Temps | Critique |
|------|-------|----------|
| A - LuaEngine Core | 2-3h | 🔴 |
| B - LuaBindings | 2-3h | 🟠 |
| C - Scripts Lua | 1h | 🟡 |
| D - Intégration | 1h | 🔴 |
| E - Tests | 1h | 🟠 |
| **TOTAL** | **7-10h** | - |

---

## 🚀 Quick Start

1. **Lecture initiale** : 15min
2. **Choix de répartition** : 5min
3. **Phase 1 parallèle** : 2-3h (A + B/C)
4. **Sync & intégration** : 1h (D)
5. **Tests** : 1h (E)

**Objectif Jour 1** : Milestones 1 & 2 ✅  
**Objectif Jour 2** : Milestones 3 & 4 ✅
