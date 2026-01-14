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
}

-- Define waves with spawn timings and enemy configurations
waveSystem.waves[1] = {
	name = "Wave 1: Basics",
	duration = 10,
	enemyConfigs = {
		{ delay = 0.0, type = "basic", x = 1200, y = 200, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.0, type = "basic", x = 1200, y = 400, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.0, type = "basic", x = 1200, y = 600, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.0, type = "basic", x = 1200, y = 200, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.0, type = "basic", x = 1200, y = 400, health = 50, script = "enemy_basic.lua" },
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

-- Waves intervals configuration
waveSystem.wavesIntervals = {
	5,  -- Interval after Wave 1
	7,  -- Interval after Wave 2
}

-- Submit the configured waves to the spawner component
setSpawnerConfig(waveSystem.spawnerEntity, {
	waves = waveSystem.waves,
	wavesIntervals = waveSystem.wavesIntervals,
})
