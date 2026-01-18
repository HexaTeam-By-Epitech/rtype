--[[
** EPITECH PROJECT, 2026
** RTYPE
** File description:
** enemy_circular.lua - Enemy that moves in circular patterns
]]

-- Per-entity state storage (indexed by entity address)
local entityStates = {}

local baseSpeed = 80 -- overall movement speed
local circleRadius = 60 -- radius of circular motion
local circleSpeed = 3.0 -- rotation speed (radians per second)

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
			centerX = transform.x,
			centerY = transform.y,
			angle = 0,
			initialized = true,
		}
	end

	local state = entityStates[addr]

	-- Increment time and angle
	state.time = state.time + deltaTime
	state.angle = state.angle + circleSpeed * deltaTime

	-- Move center point left
	state.centerX = state.centerX - baseSpeed * deltaTime

	-- Calculate circular position around moving center
	transform.x = state.centerX + math.cos(state.angle) * circleRadius
	transform.y = state.centerY + math.sin(state.angle) * circleRadius

	-- Cleanup if off-screen
	if state.centerX < -150 then
		entityStates[addr] = nil
	end
end
