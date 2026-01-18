--[[
** EPITECH PROJECT, 2026
** RTYPE
** File description:
** enemy_kamikaze.lua - Kamikaze enemy that charges directly at player
]]

-- Per-entity state storage (indexed by entity address)
local entityStates = {}

local waitSpeed = 60 -- slow movement while waiting
local chargeSpeed = 400 -- fast charge speed
local chargeDistance = 700 -- X position where charge starts
local detectionDelay = 0.5 -- seconds before starting charge

function onUpdate(entity, deltaTime)
	if not entity:isValid() or not entity:hasTransform() then
		return
	end

	local addr = entity:getAddress()
	local transform = entity:getTransform()

	-- Initialize per-entity state on first update
	if not entityStates[addr] then
		entityStates[addr] = {
			state = "waiting", -- waiting, charging
			waitTimer = 0,
			targetX = 0,
			targetY = 0,
			velocityX = 0,
			velocityY = 0,
			initialized = true,
		}
	end

	local state = entityStates[addr]

	if state.state == "waiting" then
		-- Move left slowly
		transform.x = transform.x - waitSpeed * deltaTime
		state.waitTimer = state.waitTimer + deltaTime
		
		-- Check if ready to charge
		if transform.x < chargeDistance and state.waitTimer >= detectionDelay then
			-- Find player position (simplified - target middle-left of screen)
			-- In real implementation, you'd query player entity position
			state.targetX = 100
			state.targetY = 400 + math.random(-150, 150)
			
			-- Calculate charge direction
			local dx = state.targetX - transform.x
			local dy = state.targetY - transform.y
			local distance = math.sqrt(dx * dx + dy * dy)
			
			if distance > 0 then
				state.velocityX = (dx / distance) * chargeSpeed
				state.velocityY = (dy / distance) * chargeSpeed
			end
			
			state.state = "charging"
		end
		
	elseif state.state == "charging" then
		-- Charge at constant velocity towards target
		transform.x = transform.x + state.velocityX * deltaTime
		transform.y = transform.y + state.velocityY * deltaTime
		
		-- Check if off-screen (missed player or hit)
		if transform.x < -50 or transform.x > 1300 or transform.y < -50 or transform.y > 900 then
			entityStates[addr] = nil
		end
	end

	-- Cleanup if off-screen during wait
	if state.state == "waiting" and transform.x < -100 then
		entityStates[addr] = nil
	end
end
