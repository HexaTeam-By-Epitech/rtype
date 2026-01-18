--[[
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** test_movement.lua - Circular movement test
]]

-- Per-entity state storage (indexed by entity address)
local entityStates = {}

local centerX = 400
local centerY = 300
local radius = 150
local speed = 1.0 -- Vitesse de rotation (radians/sec)

function onUpdate(entity, deltaTime)
	if not entity:isValid() or not entity:hasTransform() then
		return
	end

	local addr = entity:getAddress()

	-- Initialize per-entity state if needed
	if not entityStates[addr] then
		entityStates[addr] = {
			time = 0
		}
	end

	local state = entityStates[addr]
	local transform = entity:getTransform()

	-- Increment time for this specific entity
	state.time = state.time + deltaTime

	-- Circular movement
	transform.x = centerX + math.cos(state.time * speed) * radius
	transform.y = centerY + math.sin(state.time * speed) * radius
end
