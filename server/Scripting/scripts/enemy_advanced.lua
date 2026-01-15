--[[
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** enemy_advanced.lua - Advanced enemy with sinusoidal movement pattern
]]

local baseSpeed = 80 -- horizontal movement speed
local sineSpeed = 2.5 -- oscillation frequency
local sineAmplitude = 100 -- oscillation amplitude
local time = 0
local startY = 0
local initialized = false

function onUpdate(entity, deltaTime)
	if not entity:isValid() or not entity:hasTransform() then
		return
	end

	local transform = entity:getTransform()

	-- Initialize on first update
	if not initialized then
		startY = transform.y
		initialized = true
	end

	-- Increment time
	time = time + deltaTime

	-- Move left + oscillate up/down in a sinusoidal pattern
	transform.x = transform.x - baseSpeed * deltaTime
	transform.y = startY + math.sin(time * sineSpeed * math.pi) * sineAmplitude

	-- Destroy if off-screen (left side)
	if transform.x < -50 then
		entity:destroy()
	end
end
