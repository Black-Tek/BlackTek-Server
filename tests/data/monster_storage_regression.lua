local events, live = {}, {}
local function event(name)
    local object = {register = function() end, interval = function() end}
    events[name] = object
    return object
end
CreatureEvent, GlobalEvent = event, event
Monster = setmetatable({}, {__call = function(self, id) return live[id] end})
dofile('data/scripts/monsters/monster_storages.lua')
local function monster(id)
    local value = setmetatable({getId = function() return id end, registerEvent = function() return true end}, {__index = Monster})
    live[id] = value
    return value
end
local current = monster(1)
assert(current:getStorageValue(5) == -1 and next(MonsterStorages) == nil)
current:setStorageValue(5, 0)
assert(current:getStorageValue(5) == 0)
current:setStorageValue(5, -1)
assert(next(MonsterStorages) == nil)
print('PASS: missing reads do not allocate and clearing the last key releases the record')
current:setStorageValue(5, 7)
for id = 2, 1001 do
    local value = monster(id)
    value:setStorageValue(5, 9)
    live[id] = nil
end
events.MonsterStorageCleanup.onThink(60000)
local count = 0; for _ in pairs(MonsterStorages) do count = count + 1 end
assert(count == 1 and current:getStorageValue(5) == 7)
events.MonsterStorages.onDeath(current)
assert(next(MonsterStorages) == nil)
print('PASS: non-death removal cleanup preserves live records and death clears immediately')
