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

-- Wave 1: Introduction douce avec 3 ennemis basiques
level1.waves[1] = {
	name = "Level 1 - Wave 1: Introduction",
	duration = 12,
	enemyConfigs = {
		{ delay = 1.0, type = "basic", x = 1200, y = 200, health = 50, script = "enemy_basic.lua" },
		{ delay = 3.0, type = "basic", x = 1200, y = 400, health = 50, script = "enemy_basic.lua" },
		{ delay = 5.0, type = "basic", x = 1200, y = 600, health = 50, script = "enemy_basic.lua" },
	},
}

-- Wave 2: Augmentation avec ennemis rapides
level1.waves[2] = {
	name = "Level 1 - Wave 2: Vitesse",
	duration = 15,
	enemyConfigs = {
		{ delay = 0.5, type = "basic", x = 1200, y = 150, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.5, type = "fast", x = 1200, y = 350, health = 30, script = "enemy_basic.lua" },
		{ delay = 2.5, type = "basic", x = 1200, y = 550, health = 50, script = "enemy_basic.lua" },
		{ delay = 3.5, type = "fast", x = 1200, y = 750, health = 30, script = "enemy_basic.lua" },
		{ delay = 5.0, type = "basic", x = 1200, y = 300, health = 50, script = "enemy_basic.lua" },
	},
}

-- Wave 3: Formation en V
level1.waves[3] = {
	name = "Level 1 - Wave 3: Formation",
	duration = 18,
	enemyConfigs = {
		{ delay = 0.0, type = "basic", x = 1200, y = 400, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.5, type = "basic", x = 1250, y = 350, health = 50, script = "enemy_basic.lua" },
		{ delay = 0.5, type = "basic", x = 1250, y = 450, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "basic", x = 1300, y = 300, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "basic", x = 1300, y = 500, health = 50, script = "enemy_basic.lua" },
		{ delay = 4.0, type = "fast", x = 1200, y = 200, health = 30, script = "enemy_basic.lua" },
		{ delay = 4.0, type = "fast", x = 1200, y = 600, health = 30, script = "enemy_basic.lua" },
	},
}

-- Intervalles entre les vagues (en secondes)
level1.wavesIntervals = {
	5,  -- 5 secondes après wave 1
	6,  -- 6 secondes après wave 2
	30, -- 30 secondes après wave 3 avant fin de level
}

-- Function called by LuaEngine to setup the spawner
function onUpdate(entity, deltaTime)
	if level1.isWaveSetup == false then
		setSpawnerConfig(entity, {
			waves = level1.waves,
			wavesIntervals = level1.wavesIntervals
		})
		level1.isWaveSetup = true
	end
end
