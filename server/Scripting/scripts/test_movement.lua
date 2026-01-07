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
    log("LUA: onUpdate called with deltaTime = " .. deltaTime)
    
    -- Check if entity is valid
    log("LUA: Checking if entity is valid...")
    local isValid = entity:isValid()
    log("LUA: entity:isValid() = " .. tostring(isValid))
    
    if not isValid then
        log("Entity is invalid!")
        return
    end
    
    -- Get Transform component
    log("LUA: Checking if entity has Transform...")
    local hasTransform = entity:hasTransform()
    log("LUA: entity:hasTransform() = " .. tostring(hasTransform))
    
    if not hasTransform then
        log("Entity has no Transform component")
        return
    end
    
    log("LUA: Getting Transform component...")
    local transform = entity:getTransform()
    log("LUA: Got transform at (" .. transform.x .. ", " .. transform.y .. ")")
    
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
