--[[
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** test_movement.lua - Circular movement test
]]

local time = 0
local centerX = 400
local centerY = 300
local radius = 150
local speed = 2.0  -- Vitesse de rotation (radians/sec)

function onUpdate(entity, deltaTime)
    if not entity:isValid() or not entity:hasTransform() then
        return
    end
    
    local transform = entity:getTransform()
    
    -- Increment time
    time = time + deltaTime
    
    -- Circular movement
    transform.x = centerX + math.cos(time * speed) * radius
    transform.y = centerY + math.sin(time * speed) * radius
    
    -- Log position every second (at 60 FPS)
    local tick = math.floor(time * 60)
    if tick % 60 == 0 then
        log("Enemy rotating at (" .. string.format("%.1f", transform.x) .. ", " .. string.format("%.1f", transform.y) .. ")")
    end
end
