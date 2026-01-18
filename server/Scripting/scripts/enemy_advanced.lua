--[[
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** enemy_advanced.lua - Advanced enemy with sinusoidal movement pattern
]]

-- Per-entity state storage (indexed by entity address)
local entityStates = {}

local baseSpeed = 180 -- horizontal movement speed (pixels per second)
local sineSpeed = 2.5 -- oscillation frequency
local sineAmplitude = 1 -- oscillation amplitude

function onUpdate(entity, deltaTime)
	if not entity:isValid() or not entity:hasTransform() then
		return
	end

	local addr = entity:getAddress()
	local transform = entity:getTransform()

	-- Initialize per-entity state on first update
	if not entityStates[addr] then
		entityStates[addr] = {
			time = 0,
			startY = transform.y,
			initialized = true,
		}
	end

	local state = entityStates[addr]

	-- Increment time for this specific entity
	state.time = state.time + deltaTime

	-- Move left + oscillate up/down in a sinusoidal pattern
	transform.x = transform.x - baseSpeed * deltaTime
	transform.y = state.startY + math.sin(state.time * sineSpeed * math.pi) * sineAmplitude

	-- Destroy if off-screen (left side) and cleanup state
	if transform.x < -50 then
		entityStates[addr] = nil -- Cleanup state to prevent memory leak
		-- entity:destroy() -- Entity destruction handled by BoundarySystem
	end
end
