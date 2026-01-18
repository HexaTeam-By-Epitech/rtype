--[[
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** boss_stage_spawn.lua - Boss stage spawn pattern
]]

local bossStage = {
	currentWave = 0,
	isWaveSetup = false,
	waves = {},
	wavesIntervals = {}
}

-- Wave 1: Pré-boss - Vague d'introduction
bossStage.waves[1] = {
	name = "Boss Stage - Preparation",
	duration = 20,
	enemyConfigs = {
		{ delay = 1.0, type = "advanced", x = 1200, y = 200, health = 150, script = "enemy_advanced.lua" },
		{ delay = 2.0, type = "advanced", x = 1200, y = 600, health = 150, script = "enemy_advanced.lua" },
		{ delay = 4.0, type = "fast", x = 1200, y = 300, health = 40, script = "enemy_basic.lua" },
		{ delay = 4.0, type = "fast", x = 1200, y = 500, health = 40, script = "enemy_basic.lua" },
		{ delay = 6.0, type = "basic", x = 1200, y = 150, health = 70, script = "enemy_basic.lua" },
		{ delay = 6.0, type = "basic", x = 1200, y = 650, health = 70, script = "enemy_basic.lua" },
	},
}

-- Wave 2: Boss principal
-- Note: Le boss devrait avoir un script Lua dédié avec patterns d'attaque complexes
bossStage.waves[2] = {
	name = "Boss Stage - BOSS FIGHT",
	duration = 120, -- 2 minutes pour le boss
	enemyConfigs = {
		-- Boss principal (type "boss" nécessite un nouveau type d'ennemi)
		{ delay = 3.0, type = "advanced", x = 1100, y = 400, health = 1000, script = "enemy_advanced.lua" },
		-- Vagues de support pendant le combat de boss
		{ delay = 15.0, type = "fast", x = 1200, y = 200, health = 40, script = "enemy_basic.lua" },
		{ delay = 15.0, type = "fast", x = 1200, y = 600, health = 40, script = "enemy_basic.lua" },
		{ delay = 30.0, type = "basic", x = 1200, y = 300, health = 70, script = "enemy_basic.lua" },
		{ delay = 30.0, type = "basic", x = 1200, y = 500, health = 70, script = "enemy_basic.lua" },
		{ delay = 45.0, type = "fast", x = 1200, y = 250, health = 40, script = "enemy_basic.lua" },
		{ delay = 45.0, type = "fast", x = 1200, y = 550, health = 40, script = "enemy_basic.lua" },
		{ delay = 60.0, type = "advanced", x = 1200, y = 400, health = 150, script = "enemy_advanced.lua" },
	},
}

-- Intervalles entre les vagues
bossStage.wavesIntervals = {
	8,   -- 8 secondes avant le boss
	120, -- Fin après le boss
}

-- Function called by LuaEngine to setup the spawner
function onUpdate(entity, deltaTime)
	if bossStage.isWaveSetup == false then
		setSpawnerConfig(entity, {
			waves = bossStage.waves,
			wavesIntervals = bossStage.wavesIntervals
		})
		bossStage.isWaveSetup = true
	end
end
