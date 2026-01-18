--[[
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** level_2_spawn.lua - Spawn pattern for Level 2 (Zone Beta)
]]

local level2 = {
	currentWave = 0,
	isWaveSetup = false,
	waves = {},
	wavesIntervals = {}
}

-- Wave 1: Mélange d'ennemis
level2.waves[1] = {
	name = "Level 2 - Wave 1: Mixed Assault",
	duration = 15,
	enemyConfigs = {
		{ delay = 0.5, type = "basic", x = 1200, y = 150, health = 60, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "advanced", x = 1200, y = 300, health = 120, script = "enemy_advanced.lua" },
		{ delay = 1.5, type = "fast", x = 1200, y = 450, health = 35, script = "enemy_basic.lua" },
		{ delay = 2.0, type = "basic", x = 1200, y = 600, health = 60, script = "enemy_basic.lua" },
		{ delay = 3.0, type = "advanced", x = 1200, y = 750, health = 120, script = "enemy_advanced.lua" },
		{ delay = 4.0, type = "fast", x = 1200, y = 200, health = 35, script = "enemy_basic.lua" },
	},
}

-- Wave 2: Ennemis avancés en force
level2.waves[2] = {
	name = "Level 2 - Wave 2: Advanced Units",
	duration = 20,
	enemyConfigs = {
		{ delay = 0.5, type = "advanced", x = 1200, y = 200, health = 120, script = "enemy_advanced.lua" },
		{ delay = 1.5, type = "advanced", x = 1200, y = 400, health = 120, script = "enemy_advanced.lua" },
		{ delay = 2.5, type = "advanced", x = 1200, y = 600, health = 120, script = "enemy_advanced.lua" },
		{ delay = 4.0, type = "fast", x = 1200, y = 300, health = 35, script = "enemy_basic.lua" },
		{ delay = 4.0, type = "fast", x = 1200, y = 500, health = 35, script = "enemy_basic.lua" },
		{ delay = 6.0, type = "basic", x = 1200, y = 100, health = 60, script = "enemy_basic.lua" },
		{ delay = 6.0, type = "basic", x = 1200, y = 700, health = 60, script = "enemy_basic.lua" },
	},
}

-- Wave 3: Chaos organisé
level2.waves[3] = {
	name = "Level 2 - Wave 3: Organized Chaos",
	duration = 25,
	enemyConfigs = {
		-- Ligne du haut
		{ delay = 0.0, type = "fast", x = 1200, y = 150, health = 35, script = "enemy_basic.lua" },
		{ delay = 0.5, type = "fast", x = 1250, y = 150, health = 35, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "fast", x = 1300, y = 150, health = 35, script = "enemy_basic.lua" },
		-- Centre avec avancés
		{ delay = 2.0, type = "advanced", x = 1200, y = 400, health = 120, script = "enemy_advanced.lua" },
		{ delay = 3.0, type = "advanced", x = 1250, y = 400, health = 120, script = "enemy_advanced.lua" },
		-- Ligne du bas
		{ delay = 4.0, type = "fast", x = 1200, y = 650, health = 35, script = "enemy_basic.lua" },
		{ delay = 4.5, type = "fast", x = 1250, y = 650, health = 35, script = "enemy_basic.lua" },
		{ delay = 5.0, type = "fast", x = 1300, y = 650, health = 35, script = "enemy_basic.lua" },
		-- Basiques en support
		{ delay = 6.0, type = "basic", x = 1200, y = 250, health = 60, script = "enemy_basic.lua" },
		{ delay = 6.0, type = "basic", x = 1200, y = 550, health = 60, script = "enemy_basic.lua" },
	},
}

-- Wave 4: Finale du niveau
level2.waves[4] = {
	name = "Level 2 - Wave 4: Grand Finale",
	duration = 30,
	enemyConfigs = {
		{ delay = 1.0, type = "advanced", x = 1200, y = 200, health = 150, script = "enemy_advanced.lua" },
		{ delay = 1.0, type = "advanced", x = 1200, y = 600, health = 150, script = "enemy_advanced.lua" },
		{ delay = 2.0, type = "basic", x = 1250, y = 300, health = 60, script = "enemy_basic.lua" },
		{ delay = 2.0, type = "basic", x = 1250, y = 500, health = 60, script = "enemy_basic.lua" },
		{ delay = 3.0, type = "fast", x = 1200, y = 100, health = 35, script = "enemy_basic.lua" },
		{ delay = 3.0, type = "fast", x = 1200, y = 700, health = 35, script = "enemy_basic.lua" },
		{ delay = 5.0, type = "advanced", x = 1200, y = 400, health = 150, script = "enemy_advanced.lua" },
		{ delay = 6.0, type = "fast", x = 1300, y = 250, health = 35, script = "enemy_basic.lua" },
		{ delay = 6.0, type = "fast", x = 1300, y = 550, health = 35, script = "enemy_basic.lua" },
	},
}

-- Intervalles entre les vagues
level2.wavesIntervals = {
	4,  -- 4 secondes après wave 1
	5,  -- 5 secondes après wave 2
	6,  -- 6 secondes après wave 3
	25, -- 25 secondes après wave 4 avant fin
}

-- Function called by LuaEngine to setup the spawner
function onUpdate(entity, deltaTime)
	if level2.isWaveSetup == false then
		setSpawnerConfig(entity, {
			waves = level2.waves,
			wavesIntervals = level2.wavesIntervals
		})
		level2.isWaveSetup = true
	end
end
