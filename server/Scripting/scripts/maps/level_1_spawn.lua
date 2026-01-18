--[[
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** level_1_spawn.lua - Spawn pattern for Level 1 (Zone Alpha)
]]

local level1 = {
	currentWave = 0,
	isWaveSetup = false,
	waves = {},
	wavesIntervals = {}
}

level1.waves[1] = {
	name = "Wave 1: Diagonal (Vert) - Déplacement en diagonale",
	duration = 12,
	enemyConfigs = {
		{ delay = 0.0, type = "diagonal", x = 1200, y = 150, health = 75, script = "enemy_diagonal.lua" },
		{ delay = 2.0, type = "diagonal", x = 1200, y = 200, health = 75, script = "enemy_diagonal.lua" },
		{ delay = 4.0, type = "diagonal", x = 1200, y = 250, health = 75, script = "enemy_diagonal.lua" },
		{ delay = 6.0, type = "diagonal", x = 1200, y = 300, health = 75, script = "enemy_diagonal.lua" },
	},
}

level1.waves[2] = {
	name = "Wave 2: Ground Walker (Orange) - Marche au sol",
	duration = 15,
	enemyConfigs = {
		{ delay = 0.0, type = "homing", x = 1200, y = 580, health = 80, script = "enemy_homing.lua" },
		{ delay = 3.0, type = "homing", x = 1200, y = 580, health = 80, script = "enemy_homing.lua" },
		{ delay = 6.0, type = "homing", x = 1200, y = 580, health = 80, script = "enemy_homing.lua" },
		{ delay = 9.0, type = "homing", x = 1200, y = 580, health = 80, script = "enemy_homing.lua" },
	},
}

level1.waves[3] = {
	name = "Wave 3: Circular (Rose/Magenta) - Mouvement circulaire",
	duration = 18,
	enemyConfigs = {
		{ delay = 0.0, type = "circular", x = 1200, y = 300, health = 90, script = "enemy_circular.lua" },
		{ delay = 4.0, type = "circular", x = 1200, y = 500, health = 90, script = "enemy_circular.lua" },
		{ delay = 8.0, type = "circular", x = 1200, y = 400, health = 90, script = "enemy_circular.lua" },
	},
}

level1.waves[4] = {
	name = "Wave 4: Formation (Cyan) - Groupe en diagonal",
	duration = 15,
	enemyConfigs = {
		{ delay = 0.0, type = "formation", x = 1200, y = 200, health = 70, script = "enemy_formation.lua" },
		{ delay = 0.5, type = "formation", x = 1200, y = 260, health = 70, script = "enemy_formation.lua" },
		{ delay = 1.0, type = "formation", x = 1200, y = 320, health = 70, script = "enemy_formation.lua" },
		{ delay = 1.5, type = "formation", x = 1200, y = 380, health = 70, script = "enemy_formation.lua" },
		{ delay = 2.0, type = "formation", x = 1200, y = 440, health = 70, script = "enemy_formation.lua" },
		{ delay = 5.0, type = "formation", x = 1200, y = 250, health = 70, script = "enemy_formation.lua" },
		{ delay = 5.5, type = "formation", x = 1200, y = 310, health = 70, script = "enemy_formation.lua" },
		{ delay = 6.0, type = "formation", x = 1200, y = 370, health = 70, script = "enemy_formation.lua" },
	},
}

level1.wavesIntervals = {
	6,  -- Interval after Wave 1 (Diagonal)
	6,  -- Interval after Wave 2 (Homing)
	6,  -- Interval after Wave 3 (Circular)
	8,  -- Interval after Wave 4 (Formation)
}

-- Function called by LuaEngine to setup the spawner
function onUpdate(entity, deltaTime)
	if level1.isWaveSetup == false then
		spawnWall(960, 10, 1920, 20, false, 0)
		
		-- Bottom wall: position at the actual bottom (1080 - 10)
		spawnWall(960, 600, 1920, 20, false, 0)
		
		-- Test walls in the middle
		spawnWall(200, 200, 50, 50, false, 0)
		spawnWall(300, 200, 50, 50, false, 0)
		spawnWall(400, 200, 60, 80, true, 100)
		
		setSpawnerConfig(entity, {
			waves = level1.waves,
			wavesIntervals = level1.wavesIntervals
		})
		level1.isWaveSetup = true
	end
end
