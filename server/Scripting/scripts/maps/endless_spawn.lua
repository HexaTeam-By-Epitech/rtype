--[[
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** endless_spawn.lua - Endless mode with progressive difficulty
]]

local endless = {
	currentWave = 0,
	isWaveSetup = false,
	waves = {},
	wavesIntervals = {},
	difficultyMultiplier = 1.0
}

-- Function to generate a wave with progressive difficulty
function generateWave(waveNumber)
	local difficulty = 1.0 + (waveNumber - 1) * 0.2  -- +20% difficulty per wave
	local enemyCount = math.min(3 + waveNumber, 12) -- Start with 3, max 12 enemies
	local duration = 10 + waveNumber * 2            -- Longer waves as difficulty increases
	
	local wave = {
		name = "Endless - Wave " .. waveNumber,
		duration = duration,
		enemyConfigs = {}
	}
	
	-- Generate enemies with increasing difficulty
	for i = 1, enemyCount do
		local delay = (i - 1) * 0.8
		local yPos = 150 + ((i - 1) % 6) * 100  -- Distribute vertically
		
		-- Alternate enemy types based on wave number
		local enemyType = "basic"
		local health = 50 * difficulty
		local script = "enemy_basic.lua"
		
		if waveNumber >= 3 and i % 3 == 0 then
			enemyType = "advanced"
			health = 100 * difficulty
			script = "enemy_advanced.lua"
		elseif waveNumber >= 2 and i % 2 == 0 then
			enemyType = "fast"
			health = 30 * difficulty
			script = "enemy_basic.lua"
		end
		
		table.insert(wave.enemyConfigs, {
			delay = delay,
			type = enemyType,
			x = 1200,
			y = yPos,
			health = health,
			script = script
		})
	end
	
	return wave
end

-- Pre-generate first 10 waves
for i = 1, 10 do
	endless.waves[i] = generateWave(i)
	endless.wavesIntervals[i] = math.max(3, 8 - i * 0.3) -- Reduce intervals as waves progress
end

-- Function called by LuaEngine to setup the spawner
function onUpdate(entity, deltaTime)
	if endless.isWaveSetup == false then
		setSpawnerConfig(entity, {
			waves = endless.waves,
			wavesIntervals = endless.wavesIntervals
		})
		endless.isWaveSetup = true
	end
end

-- Note: In actual implementation, you might want to generate waves dynamically
-- as the game progresses rather than pre-generating all of them

-- Function called by LuaEngine to setup the spawner
function onUpdate(entity, deltaTime)
	if endless.isWaveSetup == false then
		setSpawnerConfig(entity, {
			waves = endless.waves,
			wavesIntervals = endless.wavesIntervals
		})
		endless.isWaveSetup = true
	end
end
