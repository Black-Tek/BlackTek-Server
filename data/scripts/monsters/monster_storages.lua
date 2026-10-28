-- Store numeric state only; reads must not allocate a record.
MonsterStorages = setmetatable(MonsterStorages or {}, nil)

local creatureEvent = CreatureEvent("MonsterStorages")
function creatureEvent.onDeath(monster)
    MonsterStorages[monster:getId()] = nil
    return true
end
creatureEvent:register()

function Monster:getStorageValue(key)
    local values = rawget(MonsterStorages, self:getId())
    return values and values[key] or -1
end

function Monster:setStorageValue(key, value)
    local id = self:getId()
    local values = rawget(MonsterStorages, id)
    if value == nil or value == -1 then
        if values then
            values[key] = nil
            if not next(values) then
                MonsterStorages[id] = nil
            end
        end
        return true
    end
    if not values then
        if not self:registerEvent("MonsterStorages") then
            return false
        end
        values = {}
        MonsterStorages[id] = values
    end
    values[key] = value
    return true
end

-- Despawns, summon dismissal and forced removal do not emit onDeath.
local cleanup = GlobalEvent("MonsterStorageCleanup")
function cleanup.onThink(interval)
    for id in pairs(MonsterStorages) do
        if not Monster(id) then
            MonsterStorages[id] = nil
        end
    end
    return true
end
cleanup:interval(60000)
cleanup:register()
