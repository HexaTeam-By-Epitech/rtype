--[[
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** enemy_zigzag.lua - Zigzag enemy behavior (moves left while oscillating up/down)
]]

-- Per-entity state storage (using entity address as key)
local entityStates = {}

function onUpdate(entity, deltaTime)
    if not entity:isValid() then
        return
    end

    local addr = entity:getAddress()
    
    -- Initialize state for new entities
    if not entityStates[addr] then
        entityStates[addr] = {
            time = 0.0,
            amplitude = 80.0,      -- Vertical oscillation height (pixels)
            frequency = 4.0,       -- Oscillations per second
            baseSpeed = 120.0      -- Horizontal speed (pixels/second)
        }
        log("Zigzag enemy initialized: " .. tostring(addr))
    end
    
    local state = entityStates[addr]
    state.time = state.time + deltaTime
    
    -- Get transform component
    if entity:hasTransform() then
        local transform = entity:getTransform()
        
        -- Horizontal movement (left)
        transform.x = transform.x - state.baseSpeed * deltaTime
        
        -- Vertical oscillation (sine wave)
        local verticalSpeed = state.amplitude * state.frequency * math.cos(2 * math.pi * state.frequency * state.time)
        transform.y = transform.y + verticalSpeed * deltaTime
    end
end

function onDestroy(entity)
    if entity:isValid() then
        local addr = entity:getAddress()
        entityStates[addr] = nil
        log("Zigzag enemy destroyed: " .. tostring(addr))
    end
end
