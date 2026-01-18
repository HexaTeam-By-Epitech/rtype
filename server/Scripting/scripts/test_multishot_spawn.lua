--
-- TEST SCRIPT: Spawn multishot collectibles for testing
-- This script spawns various multishot power-ups when the game starts
--

-- Track if we've already spawned test items
local hasSpawnedTest = false
local spawnDelay = 2.0  -- Wait 2 seconds before spawning
local timer = 0.0

function onUpdate(entity, deltaTime)
    if hasSpawnedTest then
        return  -- Already spawned, nothing to do
    end
    
    timer = timer + deltaTime
    
    if timer >= spawnDelay then
        print("========================================")
        print("[TEST] Spawning multishot test collectibles!")
        print("========================================")
        
        -- Spawn DoubleShot upgrade at (300, 300)
        spawnUpgrade("doubleshot", 1.0, 300.0, 300.0)
        print("[TEST] ✓ DoubleShot spawned at (300, 300)")
        
        -- Spawn TripleShot upgrade at (400, 300)
        spawnUpgrade("tripleshot", 1.0, 400.0, 300.0)
        print("[TEST] ✓ TripleShot spawned at (400, 300)")
        
        -- Spawn MultiShot upgrade at (500, 300)
        spawnUpgrade("multishot", 1.0, 500.0, 300.0)
        print("[TEST] ✓ MultiShot spawned at (500, 300)")
        
        print("========================================")
        print("[TEST] All test collectibles spawned!")
        print("[TEST] Collect them to test multishot!")
        print("========================================")
        
        hasSpawnedTest = true
    end
end
