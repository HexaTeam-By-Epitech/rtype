--[[
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** enemy_homing.lua - Ground walking enemy
]]

-- Per-entity state storage
local enemyStates = {}

function onUpdate(entity, deltaTime)
    if not entity:isValid() or not entity:hasTransform() then
        return
    end
    
    local addr = entity:getAddress()
    local state = enemyStates[addr]
    
    if not state then
        -- Initialize state for this enemy
        -- Use current Y position as the target ground level
        local transform = entity:getTransform()
        enemyStates[addr] = {
            initialized = true,
            onGround = true,
            groundY = transform.y  -- Already at ground level on spawn
        }
        return
    end
    
    -- Get current transform
    local transform = entity:getTransform()
    
    -- Walk horizontally
    if state.onGround then
        transform.x = transform.x - 120 * deltaTime  -- Walking speed on ground
    else
        transform.x = transform.x - 50 * deltaTime  -- Slower horizontal during descent
    end
    
    -- Cleanup if off-screen
    if transform.x < -100 then
        enemyStates[addr] = nil
    end
end

-- Cleanup function (called when entity is destroyed)
function onDestroy(entity)
    local addr = entity:getAddress()
    if enemyStates[addr] then
        enemyStates[addr] = nil
    end
end
