--[[
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** wave_manager.lua - Orchestrates game waves using SpawnSystem
]]

-- Wave manager state (local to avoid pollution, but accessible within this script)
local waveSystem = {
	currentWave = 0,
	waveActive = false,
	waveStartTime = 0,
	enemiesSpawned = 0,
	totalEnemiesInWave = 0,
	spawnerEntity = nil,
	waves = {},
	waveCompleted = false,
	timeSinceWaveComplete = 0,
	delayBetweenWaves = 5.0, -- 5 seconds between waves
}

-- Define waves with spawn timings and enemy configurations
waveSystem.waves[1] = {
	name = "Wave 1",
	duration = 5,
	enemyConfigs = {
		{ delay = 0.0, type = "basic", x = 1200, y = 300, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "basic", x = 1200, y = 300, health = 50, script = "enemy_basic.lua" },
	},
}

waveSystem.waves[2] = {
	name = "Wave 2",
	duration = 5,
	enemyConfigs = {
		{ delay = 0.0, type = "basic", x = 1200, y = 300, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "basic", x = 1200, y = 300, health = 50, script = "enemy_basic.lua" },
	},
}

waveSystem.waves[3] = {
	name = "Wave 3",
	duration = 5,
	enemyConfigs = {
		{ delay = 0.0, type = "basic", x = 1200, y = 300, health = 50, script = "enemy_basic.lua" },
		{ delay = 1.0, type = "basic", x = 1200, y = 300, health = 50, script = "enemy_basic.lua" },
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
		log("No more waves - Game Complete!")
		return false
	end

	self.currentWave = waveNumber
	self.waveActive = true
	self.waveStartTime = 0
	self.enemiesSpawned = 0
	self.waveCompleted = false
	self.timeSinceWaveComplete = 0
	self.totalEnemiesInWave = #self.waves[waveNumber].enemyConfigs

	log("=== Starting " .. self.waves[waveNumber].name .. " ===")
	log("Total enemies to spawn: " .. tostring(self.totalEnemiesInWave))
	return true
end

---@brief Update wave system (called every frame)
---@param entity The spawner entity (should have Spawner component)
---@param deltaTime Time elapsed since last frame
function waveSystem:onUpdate(entity, deltaTime)
	-- Validate that this is the spawner entity
	-- Store reference on first valid call
	if not self.spawnerEntity or not self.spawnerEntity:isValid() then
		-- First time initialization - this should be the spawner with ID 1
		self.spawnerEntity = entity
		log("🎯 wave_manager: Locked to spawner entity " .. tostring(entity:getAddress()))
	end
	
	-- Only process updates on the original spawner entity
	-- This prevents the script from running on enemy entities
	if entity:getAddress() ~= self.spawnerEntity:getAddress() then
		-- Silently ignore - this is an enemy entity, not the spawner
		return
	end
	
	-- Handle delay between waves
	if not self.waveActive and self.waveCompleted then
		self.timeSinceWaveComplete = self.timeSinceWaveComplete + deltaTime
		
		if self.timeSinceWaveComplete >= self.delayBetweenWaves then
			log("⏰ Delay complete! Starting next wave...")
			self:startWave(self.currentWave + 1)
		end
		return
	end
	
	if not self.waveActive or not entity:isValid() then
		return
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

	-- Check if all enemies spawned and wave duration elapsed
	if self.enemiesSpawned >= self.totalEnemiesInWave then
		if not self.waveCompleted and self.waveStartTime >= wave.duration then
			log("✓ Wave " .. self.currentWave .. " completed!")
			log("📊 Wave stats: " .. self.enemiesSpawned .. "/" .. self.totalEnemiesInWave .. " enemies spawned")
			self.waveActive = false
			self.waveCompleted = true
			self.timeSinceWaveComplete = 0
			log("⏳ Next wave in " .. self.delayBetweenWaves .. " seconds...")
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
	-- Only process on entities with Spawner component
	-- wave_manager should only run on the spawner entity, not on enemy entities
	if not entity:isValid() then
		return
	end
	
	-- Only process waves after game has started
	if not gameStarted then
		return
	end

	waveSystem:onUpdate(entity, deltaTime)
end
