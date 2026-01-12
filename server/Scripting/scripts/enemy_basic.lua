--[[
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** enemy_basic.lua - Basic enemy behavior (linear left movement)
]]

local speed = 250 -- pixels per second

function onUpdate(entity, deltaTime)
	if not entity:isValid() or not entity:hasTransform() then
		return
	end

	local transform = entity:getTransform()

	-- Move left at constant speed
	transform.x = transform.x - speed * deltaTime

	-- Destroy if off-screen (left side)
	if transform.x < -50 then
		entity:destroy()
	end
end
