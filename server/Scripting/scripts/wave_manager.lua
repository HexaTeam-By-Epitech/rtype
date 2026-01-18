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
	name = "Wave 2: Mixed",
	duration = 15,
	enemyConfigs = {
		{ delay = 0.5, type = "basic", x = 1200, y = 150, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "advanced", x = 1200, y = 350, health = 100, script = "enemy_advanced.lua" },
		{ delay = 1.5, type = "basic", x = 1200, y = 550, health = 50, script = "enemy_basic.lua" },
		{ delay = 2.0, type = "advanced", x = 1200, y = 750, health = 100, script = "enemy_advanced.lua" },
		{ delay = 2.5, type = "basic", x = 1200, y = 200, health = 50, script = "enemy_basic.lua" },
		{ delay = 3.0, type = "advanced", x = 1200, y = 400, health = 100, script = "enemy_advanced.lua" },
	},
}

-- Wave 3 for testing purposes
waveSystem.waves[3] = {
	name = "Wave 3: Challenge",
	duration = 20,
	enemyConfigs = {
		{ delay = 0.3, type = "advanced", x = 1200, y = 100, health = 100, script = "enemy_advanced.lua" },
		{ delay = 0.6, type = "advanced", x = 1200, y = 300, health = 100, script = "enemy_advanced.lua" },
		{ delay = 0.9, type = "basic", x = 1200, y = 500, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.2, type = "advanced", x = 1200, y = 700, health = 100, script = "enemy_advanced.lua" },
	},
}

-- Waves intervals configuration
waveSystem.wavesIntervals = {
	5, -- Interval after Wave 1
	7, -- Interval after Wave 2
	20, -- Interval after Wave 3
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
	log("DDD")
	setSpawnerConfig(entity, {
		waves = waveSystem.waves,
		wavesIntervals = waveSystem.wavesIntervals,
	})
	log("DDDD")
end
