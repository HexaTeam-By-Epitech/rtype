--[[
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** enemy_diagonal.lua - Enemy that moves in zigzag pattern
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
        enemyStates[addr] = {
            initialized = true,
            time = 0
        }
        return
    end
    
    -- Update time
    state.time = state.time + deltaTime
    
    -- Get current transform
    local transform = entity:getTransform()
    
    -- Move in zigzag pattern (horizontal + sinusoidal vertical movement)
    -- Horizontal: -100 px/s (leftward) - reduced speed
    transform.x = transform.x - 100 * deltaTime
    
    -- Vertical: sine wave oscillation
    local amplitude = 100  -- Height of zigzag (pixels) - reduced amplitude
    local frequency = 0.6  -- Speed of zigzag (Hz) - much slower oscillation
    local verticalSpeed = amplitude * frequency * 2 * math.pi * math.cos(2 * math.pi * frequency * state.time)
    transform.y = transform.y + verticalSpeed * deltaTime
    
    -- Cleanup if off-screen
    if transform.x < -150 then
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
