local events = {}
GlobalEvent = function(name)
    local event = {interval = function() end, register = function() end}
    events[name] = event
    return event
end
Position = function(x, y, z) return {x = x, y = y, z = z} end
local alive = {}
Creature = function(id) return alive[id] end
ITEM_MAGICWALL, ITEM_WILDGROWTH = 1, 2
TILESTATE_BLOCKSOLID, TILESTATE_FLOORCHANGE, TILESTATE_PROTECTIONZONE = 1, 2, 3
FLAG_IGNOREBLOCKCREATURE, FLAG_IGNOREBLOCKITEM = 4, 8
RETURNVALUE_NOERROR, SKULL_NONE = 0, 0
local destinationResult, teleports, moveSuccess = 0, 0, false
local destination = {
    getGround = function() return {} end,
    queryAdd = function(self, creature, flags)
        assert(creature and flags == FLAG_IGNOREBLOCKCREATURE)
        return destinationResult
    end,
    getItemById = function() return nil end,
    getItems = function() return {} end,
    getCreatures = function() return {} end,
    hasFlag = function() return false end,
    isWalkable = function() return true end
}
Tile = function() return destination end
Zones = {hasWorldFlag = function() return false end}
dofile('data/scripts/doors/door_lib.lua')
local npc = {
    getId = function() return 123 end,
    isRemoved = function() return false end,
    isMonster = function() return false end,
    getHealth = function() return 100 end,
    isPlayer = function() return false end,
    teleportTo = function() teleports = teleports + 1; return moveSuccess end
}
alive[123] = npc
storeCreatureEntryPosition(npc, {x = 10, y = 11, z = 7})
assert(getCreatureEntryPosition(npc).x == 10)
clearCreatureEntryPosition(npc)
assert(getCreatureEntryPosition(npc) == nil)
storeCreatureEntryPosition(npc, {x = 10, y = 11, z = 7})
alive[123] = nil; events.DoorEntryCleanup.onThink(60000)
assert(getCreatureEntryPosition(npc) == nil)
print('PASS: NPC-safe entry positions and removal cleanup')
for _, id in ipairs(lockedDoors.ids) do
    local unlocked = getUnlockedDoorId(id)
    assert(unlocked and normalDoors[unlocked])
    assert(getLockedDoorId(unlocked) == id)
end
assert(getUnlockedDoorId(14634) == 14633)
assert(not isLockedDoor(13236) and not isLockedDoor(13237) and not isLockedDoor(14641))
print('PASS: every supported key pair round-trips, unsupported assets stay unregistered')
for _, denied in ipairs({1, 2, 3}) do
    destinationResult = denied
    assert(not pushCreatureFromDoor(npc, {x = 1, y = 1, z = 7}))
end
assert(teleports == 0)
print('PASS: engine destination rejection prevents teleport in every search round')
destinationResult = 0
assert(not pushCreatureFromDoor(npc, {x = 1, y = 1, z = 7}))
moveSuccess = true
assert(pushCreatureFromDoor(npc, {x = 1, y = 1, z = 7}))
print('PASS: failed creature moves are not reported as success')
local item = {moveTo = function() return false end}
assert(not pushItemFromDoor(item, {x = 1, y = 1, z = 7}))
item.moveTo = function() return true end
assert(pushItemFromDoor(item, {x = 1, y = 1, z = 7}))
print('PASS: failed item moves try alternatives and return failure')

local occupant = {
    isPlayer = function() return true end, isMonster = function() return false end,
    isRemoved = function() return false end, getHealth = function() return 100 end,
    getSkull = function() return SKULL_NONE end
}
destination.getCreatures = function() return {occupant} end
doorConfig.allowPushOntoPlayers = false
assert(not canPushToPosition({x = 1, y = 1, z = 7}, 3, nil, npc))
doorConfig.allowPushOntoPlayers = true
assert(canPushToPosition({x = 1, y = 1, z = 7}, 3, nil, npc))
print('PASS: fallback search cannot bypass configured occupant restrictions')
