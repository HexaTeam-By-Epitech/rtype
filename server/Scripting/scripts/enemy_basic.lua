--[[
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** enemy_basic.lua - Basic enemy behavior (linear left movement)
]]

-- Local constants for this script
local SPEED = 100 -- pixels per second
local DESTROY_X = -50

-- Called when the script is first loaded
log("enemy_basic.lua: Script initialized")

-- onUpdate is called for each entity with this script
function onUpdate(entity, deltaTime)
	if not entity:isValid() or not entity:hasTransform() then
		return
	end

	-- Get transform for THIS entity (not shared)
	local transform = entity:getTransform()

	-- Move left at constant speed
	transform.x = transform.x - SPEED * deltaTime

	-- Destroy if off-screen (left side)
	if transform.x < DESTROY_X then
		log("enemy_basic.lua: Destroying enemy " .. tostring(entity:getAddress()) .. " at x=" .. string.format("%.1f", transform.x))
		entity:destroy()
	end
end
