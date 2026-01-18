--[[
** EPITECH PROJECT, 2026
** RTYPE
** File description:
** enemy_diving.lua - Diving enemy that swoops down towards player
]]

-- Per-entity state storage (indexed by entity address)
local entityStates = {}

local approachSpeed = 100 -- initial horizontal movement speed
local diveSpeed = 300 -- diving speed when attacking
local diveThreshold = 600 -- X position where dive starts
local returnSpeed = 150 -- speed when returning to top

function onUpdate(entity, deltaTime)
	if not entity:isValid() or not entity:hasTransform() then
		return
	end

	local addr = entity:getAddress()
	local transform = entity:getTransform()

	-- Initialize per-entity state on first update
	if not entityStates[addr] then
		entityStates[addr] = {
			state = "approach", -- approach, diving, return
			startY = transform.y,
			targetY = 0,
			initialized = true,
		}
	end

	local state = entityStates[addr]

	if state.state == "approach" then
		-- Move left slowly until dive threshold
		transform.x = transform.x - approachSpeed * deltaTime
		
		if transform.x < diveThreshold then
			-- Start diving
			state.state = "diving"
			-- Dive towards bottom third of screen
			state.targetY = 600 + math.random(-50, 50)
		end
		
	elseif state.state == "diving" then
		-- Dive down and slightly left
		transform.x = transform.x - approachSpeed * deltaTime
		
		local yDir = state.targetY - transform.y
		if math.abs(yDir) > 5 then
			transform.y = transform.y + (yDir > 0 and 1 or -1) * diveSpeed * deltaTime
		else
			-- Reached dive target, start returning
			state.state = "return"
			state.targetY = -50 -- Return above screen
		end
		
	elseif state.state == "return" then
		-- Return to top of screen
		transform.x = transform.x - approachSpeed * 0.5 * deltaTime
		transform.y = transform.y - returnSpeed * deltaTime
		
		if transform.y < state.targetY then
			-- Off-screen, cleanup
			entityStates[addr] = nil
		end
	end

	-- Cleanup if off-screen
	if transform.x < -100 or transform.y > 900 then
		entityStates[addr] = nil
	end
end
