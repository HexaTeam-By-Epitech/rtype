--[[
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** wave_manager.lua - Orchestrates game waves using SpawnSystem
]]

local waveSystem = {
	currentWave = 0,
	waveActive = false,
	waveStartTime = 0,
	enemiesSpawned = 0,
	totalEnemiesInWave = 0,
	spawnerEntity = nil,
	waves = {},
	isWaveSetup = false,
}

-- Define waves with spawn timings and enemy configurations
waveSystem.waves[1] = {
	name = "Wave 1: Basics",
	duration = 10,
	enemyConfigs = {
		{ delay = 0.0, type = "basic", x = 1200, y = 200, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.0, type = "basic", x = 1200, y = 400, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.0, type = "basic", x = 1200, y = 600, health = 50, script = "enemy_basic.lua" },
	},
}

waveSystem.waves[2] = {
	name = "Wave 2: Formation",
	duration = 15,
	enemyConfigs = {
		{ delay = 0.5, type = "basic", x = 1200, y = 150, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "basic", x = 1200, y = 350, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.5, type = "basic", x = 1200, y = 550, health = 50, script = "enemy_basic.lua" },
		{ delay = 2.0, type = "basic", x = 1250, y = 250, health = 50, script = "enemy_basic.lua" },
		{ delay = 2.5, type = "basic", x = 1250, y = 450, health = 50, script = "enemy_basic.lua" },
	},
}

-- Wave 3: Challenge
waveSystem.waves[3] = {
	name = "Wave 3: Challenge",
	duration = 15,
	enemyConfigs = {
		{ delay = 0.0, type = "basic", x = 1200, y = 100, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.3, type = "basic", x = 1200, y = 300, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.6, type = "basic", x = 1200, y = 500, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.9, type = "basic", x = 1200, y = 700, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.5, type = "basic", x = 1250, y = 200, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.5, type = "basic", x = 1250, y = 600, health = 50, script = "enemy_basic.lua" },
	},
}

-- Wave 4: Double Line
waveSystem.waves[4] = {
	name = "Wave 4: Double Line",
	duration = 15,
	enemyConfigs = {
		{ delay = 0.0, type = "basic", x = 1200, y = 150, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.0, type = "basic", x = 1200, y = 650, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.5, type = "basic", x = 1250, y = 250, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.5, type = "basic", x = 1250, y = 550, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "basic", x = 1300, y = 350, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "basic", x = 1300, y = 450, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.5, type = "basic", x = 1350, y = 200, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.5, type = "basic", x = 1350, y = 600, health = 50, script = "enemy_basic.lua" },
	},
}

-- Wave 5: Formation
waveSystem.waves[5] = {
	name = "Wave 5: Formation",
	duration = 18,
	enemyConfigs = {
		{ delay = 0.0, type = "basic", x = 1200, y = 200, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.0, type = "basic", x = 1200, y = 400, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.0, type = "basic", x = 1200, y = 600, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "basic", x = 1250, y = 150, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "basic", x = 1250, y = 350, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "basic", x = 1250, y = 550, health = 50, script = "enemy_basic.lua" },
		{ delay = 2.0, type = "basic", x = 1300, y = 300, health = 50, script = "enemy_basic.lua" },
		{ delay = 2.0, type = "basic", x = 1300, y = 500, health = 50, script = "enemy_basic.lua" },
	},
}

-- Wave 6: Cascade
waveSystem.waves[6] = {
	name = "Wave 6: Cascade",
	duration = 20,
	enemyConfigs = {
		{ delay = 0.0, type = "basic", x = 1200, y = 100, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.3, type = "basic", x = 1200, y = 200, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.6, type = "basic", x = 1200, y = 300, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.9, type = "basic", x = 1200, y = 400, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.2, type = "basic", x = 1200, y = 500, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.5, type = "basic", x = 1200, y = 600, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.8, type = "basic", x = 1200, y = 700, health = 50, script = "enemy_basic.lua" },
		{ delay = 2.5, type = "basic", x = 1300, y = 150, health = 50, script = "enemy_basic.lua" },
		{ delay = 2.5, type = "basic", x = 1300, y = 400, health = 50, script = "enemy_basic.lua" },
		{ delay = 2.5, type = "basic", x = 1300, y = 650, health = 50, script = "enemy_basic.lua" },
	},
}

-- Wave 7: Swarm
waveSystem.waves[7] = {
	name = "Wave 7: Swarm",
	duration = 20,
	enemyConfigs = {
		{ delay = 0.0, type = "basic", x = 1200, y = 100, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.2, type = "basic", x = 1200, y = 200, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.4, type = "basic", x = 1200, y = 300, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.6, type = "basic", x = 1200, y = 400, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.8, type = "basic", x = 1200, y = 500, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "basic", x = 1200, y = 600, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.2, type = "basic", x = 1200, y = 700, health = 50, script = "enemy_basic.lua" },
		{ delay = 2.0, type = "basic", x = 1300, y = 150, health = 50, script = "enemy_basic.lua" },
		{ delay = 2.2, type = "basic", x = 1300, y = 250, health = 50, script = "enemy_basic.lua" },
		{ delay = 2.4, type = "basic", x = 1300, y = 350, health = 50, script = "enemy_basic.lua" },
		{ delay = 2.6, type = "basic", x = 1300, y = 450, health = 50, script = "enemy_basic.lua" },
		{ delay = 2.8, type = "basic", x = 1300, y = 550, health = 50, script = "enemy_basic.lua" },
		{ delay = 3.0, type = "basic", x = 1300, y = 650, health = 50, script = "enemy_basic.lua" },
	},
}

-- Wave 8: Reinforced
waveSystem.waves[8] = {
	name = "Wave 8: Reinforced",
	duration = 22,
	enemyConfigs = {
		{ delay = 0.0, type = "basic", x = 1200, y = 200, health = 80, script = "enemy_basic.lua" },
		{ delay = 0.0, type = "basic", x = 1200, y = 400, health = 80, script = "enemy_basic.lua" },
		{ delay = 0.0, type = "basic", x = 1200, y = 600, health = 80, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "basic", x = 1250, y = 100, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "basic", x = 1250, y = 700, health = 50, script = "enemy_basic.lua" },
		{ delay = 2.0, type = "basic", x = 1300, y = 300, health = 80, script = "enemy_basic.lua" },
		{ delay = 2.0, type = "basic", x = 1300, y = 500, health = 80, script = "enemy_basic.lua" },
		{ delay = 3.0, type = "basic", x = 1350, y = 200, health = 50, script = "enemy_basic.lua" },
		{ delay = 3.0, type = "basic", x = 1350, y = 600, health = 50, script = "enemy_basic.lua" },
	},
}

-- Wave 9: Final Assault
waveSystem.waves[9] = {
	name = "Wave 9: Final Assault",
	duration = 25,
	enemyConfigs = {
		{ delay = 0.0, type = "basic", x = 1200, y = 150, health = 60, script = "enemy_basic.lua" },
		{ delay = 0.0, type = "basic", x = 1200, y = 400, health = 60, script = "enemy_basic.lua" },
		{ delay = 0.0, type = "basic", x = 1200, y = 650, health = 60, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "basic", x = 1250, y = 100, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.2, type = "basic", x = 1250, y = 250, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.4, type = "basic", x = 1250, y = 400, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.6, type = "basic", x = 1250, y = 550, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.8, type = "basic", x = 1250, y = 700, health = 50, script = "enemy_basic.lua" },
		{ delay = 3.0, type = "basic", x = 1300, y = 200, health = 60, script = "enemy_basic.lua" },
		{ delay = 3.0, type = "basic", x = 1300, y = 350, health = 60, script = "enemy_basic.lua" },
		{ delay = 3.0, type = "basic", x = 1300, y = 500, health = 60, script = "enemy_basic.lua" },
		{ delay = 3.0, type = "basic", x = 1300, y = 650, health = 60, script = "enemy_basic.lua" },
		{ delay = 4.5, type = "basic", x = 1400, y = 300, health = 80, script = "enemy_basic.lua" },
		{ delay = 4.5, type = "basic", x = 1400, y = 500, health = 80, script = "enemy_basic.lua" },
	},
}

-- Wave 10: Endurance
waveSystem.waves[10] = {
	name = "Wave 10: Endurance",
	duration = 30,
	enemyConfigs = {
		{ delay = 0.0, type = "basic", x = 1200, y = 400, health = 100, script = "enemy_basic.lua" },
		{ delay = 0.5, type = "basic", x = 1200, y = 200, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.5, type = "basic", x = 1200, y = 600, health = 50, script = "enemy_basic.lua" },
		{ delay = 2.0, type = "basic", x = 1300, y = 150, health = 50, script = "enemy_basic.lua" },
		{ delay = 2.0, type = "basic", x = 1300, y = 400, health = 80, script = "enemy_basic.lua" },
		{ delay = 2.0, type = "basic", x = 1300, y = 650, health = 50, script = "enemy_basic.lua" },
		{ delay = 4.0, type = "basic", x = 1400, y = 100, health = 50, script = "enemy_basic.lua" },
		{ delay = 4.0, type = "basic", x = 1400, y = 300, health = 60, script = "enemy_basic.lua" },
		{ delay = 4.0, type = "basic", x = 1400, y = 500, health = 60, script = "enemy_basic.lua" },
		{ delay = 4.0, type = "basic", x = 1400, y = 700, health = 50, script = "enemy_basic.lua" },
		{ delay = 6.0, type = "basic", x = 1500, y = 200, health = 80, script = "enemy_basic.lua" },
		{ delay = 6.0, type = "basic", x = 1500, y = 400, health = 100, script = "enemy_basic.lua" },
		{ delay = 6.0, type = "basic", x = 1500, y = 600, health = 80, script = "enemy_basic.lua" },
	},
}

-- Waves intervals configuration
waveSystem.wavesIntervals = {
	5,  -- Interval after Wave 1
	7,  -- Interval after Wave 2
	8,  -- Interval after Wave 3
	8,  -- Interval after Wave 4
	10, -- Interval after Wave 5
	10, -- Interval after Wave 6
	12, -- Interval after Wave 7
	12, -- Interval after Wave 8
	15, -- Interval after Wave 9
	30, -- Interval after Wave 10 (final)
}

-- Submit the configured waves to the spawner component

-- -- TODO: Remove onUpdate function content (see GameLogic.cpp)
-- -- onUpdate is a special function called every frame by the engine, but this is setup code
-- -- remove the log statements

-- log "AAA"

-- function onUpdate(entity, deltaTime)
-- 	log "BBB"
-- 	if waveSystem.isWaveSetup == false then
-- 		setSpawnerConfig(entity, {
-- 		waves = waveSystem.waves,
-- 		wavesIntervals = waveSystem.wavesIntervals})
-- 		log "CCC"
-- 		waveSystem.isWaveSetup = true
-- 	end
-- end

function onUpdate(entity, deltaTime) end

function onGameStart(entity)
	-- Count total waves and enemies
	local totalWaves = 0
	local totalEnemies = 0
	for i, wave in pairs(waveSystem.waves) do
		totalWaves = totalWaves + 1
		if wave.enemyConfigs then
			for _, enemy in pairs(wave.enemyConfigs) do
				totalEnemies = totalEnemies + 1
			end
		end
	end
	print("========================================")
	print("[WAVE_MANAGER] Configuring " .. totalWaves .. " waves with " .. totalEnemies .. " total enemies")
	print("[WAVE_MANAGER] Wave intervals: " .. #waveSystem.wavesIntervals)
	print("========================================")

	setSpawnerConfig(entity, {
		waves = waveSystem.waves,
		wavesIntervals = waveSystem.wavesIntervals,
	})

	-- TEST: Spawn multishot collectibles for testing
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
end
