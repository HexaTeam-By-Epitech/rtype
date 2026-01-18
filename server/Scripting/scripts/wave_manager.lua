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
-- Each wave showcases ONE enemy type for easy testing

waveSystem.waves[1] = {
	name = "Wave 1: Zigzag (Green) - Up/Down Wave",
	duration = 12,
	enemyConfigs = {
		{ delay = 0.0, type = "zigzag", x = 1200, y = 200, health = 75, script = "enemy_zigzag.lua" },
		{ delay = 2.0, type = "zigzag", x = 1200, y = 400, health = 75, script = "enemy_zigzag.lua" },
		{ delay = 4.0, type = "zigzag", x = 1200, y = 600, health = 75, script = "enemy_zigzag.lua" },
		{ delay = 6.0, type = "zigzag", x = 1200, y = 300, health = 75, script = "enemy_zigzag.lua" },
	},
}

waveSystem.waves[2] = {
	name = "Wave 2: Diving (Orange) - Dive Attack",
	duration = 15,
	enemyConfigs = {
		{ delay = 0.0, type = "diving", x = 1200, y = 100, health = 80, script = "enemy_diving.lua" },
		{ delay = 3.0, type = "diving", x = 1200, y = 100, health = 80, script = "enemy_diving.lua" },
		{ delay = 6.0, type = "diving", x = 1200, y = 100, health = 80, script = "enemy_diving.lua" },
		{ delay = 9.0, type = "diving", x = 1200, y = 100, health = 80, script = "enemy_diving.lua" },
	},
}

waveSystem.waves[3] = {
	name = "Wave 3: Circular (Magenta) - Circle Motion",
	duration = 18,
	enemyConfigs = {
		{ delay = 0.0, type = "circular", x = 1200, y = 300, health = 90, script = "enemy_circular.lua" },
		{ delay = 4.0, type = "circular", x = 1200, y = 500, health = 90, script = "enemy_circular.lua" },
		{ delay = 8.0, type = "circular", x = 1200, y = 400, health = 90, script = "enemy_circular.lua" },
	},
}

waveSystem.waves[4] = {
	name = "Wave 4: Kamikaze (Cyan) - Charge Attack",
	duration = 15,
	enemyConfigs = {
		{ delay = 0.0, type = "kamikaze", x = 1200, y = 200, health = 60, script = "enemy_kamikaze.lua" },
		{ delay = 3.0, type = "kamikaze", x = 1200, y = 400, health = 60, script = "enemy_kamikaze.lua" },
		{ delay = 6.0, type = "kamikaze", x = 1200, y = 600, health = 60, script = "enemy_kamikaze.lua" },
		{ delay = 9.0, type = "kamikaze", x = 1200, y = 350, health = 60, script = "enemy_kamikaze.lua" },
	},
}

-- Waves intervals configuration
waveSystem.wavesIntervals = {
	6,  -- Interval after Wave 1 (Zigzag Green)
	6,  -- Interval after Wave 2 (Diving Orange)
	6,  -- Interval after Wave 3 (Circular Magenta)
	8,  -- Interval after Wave 4 (Kamikaze Cyan)
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
	setSpawnerConfig(entity, {
		waves = waveSystem.waves,
		wavesIntervals = waveSystem.wavesIntervals,
	})
end
