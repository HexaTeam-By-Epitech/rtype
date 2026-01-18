--[[
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** enemy_formation.lua - Enemy that moves in formation with staggered vertical movement
]]

-- Shared formation data (persists across all formation enemies)
local formationData = {
    enemies = {},  -- List of all formation enemies
    nextOffset = 0  -- Next X offset for spacing
}

-- Per-entity state storage
local enemyStates = {}

-- Formation configuration
local horizontalSpeed = 60   -- pixels per second (left) - slower
local verticalSpeed = 40     -- pixels per second (up/down) - slower
local spacing = 20           -- pixels between enemies in formation (very close)
local minY = 50              -- Top limit (near top wall)
local maxY = 570             -- Bottom limit (near bottom wall)

function onUpdate(entity, deltaTime)
    if not entity:isValid() or not entity:hasTransform() then
        return
    end
    
    local addr = entity:getAddress()
    local state = enemyStates[addr]
    
    if not state then
        -- Initialize state for this enemy
        -- Assign formation offset based on spawn order
        local xOffset = formationData.nextOffset
        formationData.nextOffset = formationData.nextOffset + spacing
        
        -- Reset offset after 5 enemies (cycle the formation)
        if formationData.nextOffset >= spacing * 5 then
            formationData.nextOffset = 0
        end
        
        local transform = entity:getTransform()
        enemyStates[addr] = {
            initialized = true,
            xOffset = xOffset,  -- Horizontal spacing in formation
            verticalDirection = -1  -- Each enemy has its own direction (start going up)
        }
        
        -- Add to formation list
        table.insert(formationData.enemies, addr)
        
        state = enemyStates[addr]
    end
    
    -- Get current transform
    local transform = entity:getTransform()
    
    -- Each enemy checks its own position and changes direction independently
    if transform.y <= minY then
        state.verticalDirection = 1  -- Move down
    elseif transform.y >= maxY then
        state.verticalDirection = -1  -- Move up
    end
    
    -- Apply diagonal movement: horizontal (left) + vertical (up or down)
    transform.x = transform.x - horizontalSpeed * deltaTime
    transform.y = transform.y + (state.verticalDirection * verticalSpeed * deltaTime)
    
    -- Cleanup if off-screen
    if transform.x < -150 then
        enemyStates[addr] = nil
    end
end

-- Cleanup function (called when entity is destroyed)
function onDestroy(entity)
    local addr = entity:getAddress()
    if enemyStates[addr] then
        
        -- Remove from formation list
        for i, e in ipairs(formationData.enemies) do
            if e == addr then
                table.remove(formationData.enemies, i)
                break
            end
        end
        
        enemyStates[addr] = nil
    end
end
