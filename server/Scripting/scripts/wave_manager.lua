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

---@brief Start a wave
---@param waveNumber The wave number to start
---@return boolean true if wave started successfully
function waveSystem:startWave(waveNumber)
	if self.waveActive then
		log("Wave already active")
		return false
	end

	if not self.waves[waveNumber] then
		log("Wave " .. tostring(waveNumber) .. " not found")
		return false
	end

	self.currentWave = waveNumber
	self.waveActive = true
	self.waveStartTime = 0
	self.enemiesSpawned = 0
	self.totalEnemiesInWave = #self.waves[waveNumber].enemyConfigs

	log("=== Starting " .. self.waves[waveNumber].name .. " ===")
	log("Total enemies to spawn: " .. tostring(self.totalEnemiesInWave))
	return true
end

---@brief Update wave system (called every frame)
---@param entity The spawner entity (should have Spawner component)
---@param deltaTime Time elapsed since last frame
function waveSystem:onUpdate(entity, deltaTime)
	if not self.waveActive or not entity:isValid() then
		return
	end

	-- Store reference to spawner entity
	if not self.spawnerEntity or not self.spawnerEntity:isValid() then
		self.spawnerEntity = entity
	end

	self.waveStartTime = self.waveStartTime + deltaTime

	local wave = self.waves[self.currentWave]
	if not wave then
		return
	end

	-- Spawn enemies based on timing
	for i, config in ipairs(wave.enemyConfigs) do
		if self.waveStartTime >= config.delay and i > self.enemiesSpawned then
			self:spawnEnemy(entity, config)
			self.enemiesSpawned = i
		end
	end

	-- Check if all enemies spawned
	if self.enemiesSpawned >= self.totalEnemiesInWave then
		if self.enemiesSpawned == self.totalEnemiesInWave then
			log("✓ All enemies for wave " .. self.currentWave .. " have been spawned")
		end
	end
end

---@brief Queue an enemy spawn through the Spawner component
---@param spawnerEntity The entity with Spawner component
---@param config Enemy configuration table
function waveSystem:spawnEnemy(spawnerEntity, config)
	-- Validate spawner entity before queueing
	if not spawnerEntity or not spawnerEntity:isValid() then
		log("ERROR: Spawner entity is invalid")
		return
	end

	-- Use queueSpawn to send request to SpawnSystem
	queueSpawn(spawnerEntity, config.x, config.y, config.type, config.script, config.health, 100)

	log(
		"  [Enemy] Queued "
			.. config.type
			.. " at ("
			.. string.format("%.0f", config.x)
			.. ", "
			.. string.format("%.0f", config.y)
			.. ")"
	)
end

-- Track if game has started
local gameStarted = false

-- Register callback for when the game starts
onGameStart(function(roomId)
	log("=== Game Started in Room: " .. roomId .. " ===")
	gameStarted = true
	waveSystem:startWave(1)
end)

-- Main update function called by LuaSystemAdapter
function onUpdate(entity, deltaTime)
	-- Only process waves after game has started
	if not gameStarted then
		return
	end

	waveSystem:onUpdate(entity, deltaTime)
end
