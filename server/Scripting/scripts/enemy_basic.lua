--[[
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** enemy_basic.lua - Basic enemy behavior (linear left movement)
]]

-- Per-entity state storage (indexed by entity address)
local entityStates = {}

local baseSpeed = 150 -- pixels per second

function onUpdate(entity, deltaTime)
	if not entity:isValid() or not entity:hasTransform() then
		return
	end

	local addr = entity:getAddress()

	-- Initialize per-entity state if needed
	if not entityStates[addr] then
		entityStates[addr] = {
			initialized = true,
		}
		log("Basic enemy initialized: " .. tostring(addr))
	end

	local transform = entity:getTransform()

	-- Move left at constant speed
	transform.x = transform.x - baseSpeed * deltaTime

	-- Cleanup if entity goes off-screen
	if transform.x < -100 then
		entityStates[addr] = nil
		-- entity:destroy() -- Entity destruction handled by BoundarySystem
	end
end
