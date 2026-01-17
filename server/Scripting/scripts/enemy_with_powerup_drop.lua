-- enemy_with_powerup_drop.lua
-- Example Lua script for an enemy that drops power-ups when destroyed

-- Initialize enemy state
local initialized = false
local spawnX = 0
local spawnY = 0
local wavePhase = 0
local waveSpeed = 2.0
local waveAmplitude = 50.0

function onInit(entity)
    if not initialized then
        local transform = entity:getTransform()
        spawnX = transform.x
        spawnY = transform.y
        initialized = true
        print("[LUA] Enemy initialized at (" .. spawnX .. ", " .. spawnY .. ")")
    end
end

function onUpdate(entity, deltaTime)
    -- Sinusoidal movement pattern
    wavePhase = wavePhase + deltaTime * waveSpeed
    
    local transform = entity:getTransform()
    local velocity = entity:getVelocity()
    
    -- Move left and oscillate vertically
    local targetY = spawnY + math.sin(wavePhase) * waveAmplitude
    local dirY = (targetY - transform.y) * 0.05
    
    velocity:setDirection(-1.0, dirY)
end

function onDestroy(entity)
    local transform = entity:getTransform()
    local x = transform.x
    local y = transform.y
    
    print("[LUA] Enemy destroyed at (" .. x .. ", " .. y .. ")")
    
    -- 40% chance to drop a power-up
    local roll = math.random(1, 100)
    
    if roll <= 40 then
        local powerUpType = math.random(1, 5)
        
        if powerUpType == 1 then
            -- Speed boost for 5 seconds
            spawnPowerUp("speed", 5.0, 1.5, x, y)
            print("[LUA] Dropped speed boost")
            
        elseif powerUpType == 2 then
            -- Damage boost for 8 seconds
            spawnPowerUp("damage", 8.0, 2.0, x, y)
            print("[LUA] Dropped damage boost")
            
        elseif powerUpType == 3 then
            -- Shield for 3 seconds
            spawnPowerUp("shield", 3.0, 1.0, x, y)
            print("[LUA] Dropped shield")
            
        elseif powerUpType == 4 then
            -- Health pack
            spawnHealthPack(25, x, y)
            print("[LUA] Dropped health pack")
            
        else
            -- Rare: permanent upgrade
            local upgradeRoll = math.random(1, 3)
            
            if upgradeRoll == 1 then
                -- Double shot (permanent)
                spawnUpgrade("doubleshot", 1.0, x, y)
                print("[LUA] Dropped double shot upgrade!")
            elseif upgradeRoll == 2 then
                -- Triple shot (permanent)
                spawnUpgrade("tripleshot", 1.0, x, y)
                print("[LUA] Dropped triple shot upgrade!")
            else
                -- Max health increase
                spawnUpgrade("maxhealth", 50.0, x, y)
                print("[LUA] Dropped max health upgrade!")
            end
        end
    end
end

-- Wave patterns for different enemy types
function createWavePattern(type)
    if type == "sine" then
        return function(phase, amplitude)
            return math.sin(phase) * amplitude
        end
    elseif type == "zigzag" then
        return function(phase, amplitude)
            return (math.floor(phase * 2) % 2 == 0) and amplitude or -amplitude
        end
    elseif type == "circle" then
        return function(phase, amplitude)
            return math.cos(phase) * amplitude, math.sin(phase) * amplitude
        end
    end
end
