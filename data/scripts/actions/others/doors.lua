-- Configuration to enable/disable pushing players via doors
local config = {
    allowPushPlayers = true,            -- Set to false to disable pushing players via doors
    allowPushHostile = false,           -- Set to true to allow pushing through hostile players or their summons
    allowPushThroughMagicWall = false,  -- Set to true to allow pushing through Magic Walls & Wild Growths
}

-- Define door type arrays if they don't exist
local horizontalDoors = {
    -- Horizontal doors (doors that align east-west)
    -- Add your door IDs here
}

local verticalDoors = {
    -- Vertical doors (doors that align north-south)
    -- Add your door IDs here
}

-- Priority offset positions for pushing players
-- 1 = highest priority, 8 = lowest priority
local horizontalDoorOffsets = {
    {x = 0, y = 1, priority = 1},   -- South (highest priority)
    {x = 0, y = -1, priority = 2},  -- North (second priority)
    {x = 1, y = 0, priority = 3},   -- East
    {x = -1, y = 0, priority = 4},  -- West
    {x = 1, y = 1, priority = 5},   -- South-East
    {x = -1, y = 1, priority = 6},  -- South-West
    {x = -1, y = -1, priority = 7}, -- North-West
    {x = 1, y = -1, priority = 8}   -- North-East
}

local verticalDoorOffsets = {
    {x = 1, y = 0, priority = 1},   -- East (highest priority)
    {x = -1, y = 0, priority = 2},  -- West (second priority)
    {x = 0, y = 1, priority = 3},   -- South
    {x = 0, y = -1, priority = 4},  -- North
    {x = 1, y = 1, priority = 5},   -- South-East
    {x = -1, y = 1, priority = 6},  -- South-West
    {x = -1, y = -1, priority = 7}, -- North-West
    {x = 1, y = -1, priority = 8}   -- North-East
}

-- Default offsets if door orientation is unknown
local defaultOffsets = {
    {x = 1, y = 0, priority = 1},   -- East (highest priority)
    {x = 0, y = 1, priority = 2},   -- South (second priority)
    {x = -1, y = 0, priority = 3},  -- West
    {x = 0, y = -1, priority = 4},  -- North
    {x = 1, y = 1, priority = 5},   -- South-East
    {x = -1, y = 1, priority = 6},  -- South-West
    {x = -1, y = -1, priority = 7}, -- North-West
    {x = 1, y = -1, priority = 8}   -- North-East
}

-- Helper function to check if an item ID is in a table
local function contains(table, value)
    if not table then return false end
    for _, v in pairs(table) do
        if v == value then
            return true
        end
    end
    return false
end

-- Sort the offset tables by priority (lowest number = highest priority)
table.sort(horizontalDoorOffsets, function(a, b) return a.priority < b.priority end)
table.sort(verticalDoorOffsets, function(a, b) return a.priority < b.priority end)
table.sort(defaultOffsets, function(a, b) return a.priority < b.priority end)

local function isHostile(player)
    return player:isPlayer() and player:getSkull() > SKULL_NONE 
end

local function findPushPosition(creature, doorItem, round)
    if not config.allowPushPlayers then
        return nil 
    end

    local pos = creature:getPosition()
    local doorId = doorItem:getId()
    
    -- Determine door orientation by checking against our custom door arrays
    local isHorizontal = contains(horizontalDoors, doorId)
    local isVertical = contains(verticalDoors, doorId)
    
    -- Select the appropriate offset table based on door orientation
    local offsetsToUse = defaultOffsets
    if isHorizontal then
        offsetsToUse = horizontalDoorOffsets
    elseif isVertical then
        offsetsToUse = verticalDoorOffsets
    end
    
    -- Check each position in order of priority
    for _, offsetData in ipairs(offsetsToUse) do
        local offsetPosition = Position(pos.x + offsetData.x, pos.y + offsetData.y, pos.z)
        local tile = Tile(offsetPosition)
        if tile then
            -- Get all creatures on the tile
            local creatures = tile:getCreatures()
            local hasMagicWall = tile:getItemById(ITEM_MAGICWALL)
            local hasWildGrowth = tile:getItemById(ITEM_WILDGROWTH)
            local hasMovableItem = tile:getTopDownItem() and tile:getTopDownItem():getType():isMovable()
            local isSolid = tile:hasFlag(TILESTATE_BLOCKSOLID)

            local hasAggressiveEntity = false
            local hasNonHostileCreature = false
            for _, c in ipairs(creatures) do
                if isHostile(c) then
                    hasAggressiveEntity = true
                    break
                elseif not c:isPlayer() then
                    hasNonHostileCreature = true
                end
            end
            -- Allow pushing through Magic Walls/Wild Growth if enabled
            if (hasMagicWall or hasWildGrowth) then
                if config.allowPushThroughMagicWall then
                    return offsetPosition
                else
                    goto continue
                end
            end

            -- Check if the tile is fully blocked (but ignore Magic Walls if allowed)
            if isSolid or tile:hasFlag(TILESTATE_FLOORCHANGE) or tile:hasFlag(TILESTATE_PROTECTIONZONE) then
                goto continue
            end

            -- Prioritize empty tiles
            if round == 1 and #creatures == 0 then
                return offsetPosition
            end

            -- Push to tiles occupied by non-hostile creatures
            if round == 2 and #creatures > 0 then
                if not hasAggressiveEntity or config.allowPushHostile then
                    return offsetPosition
                end
            end

            -- Allow moving into tiles with movable objects
            if round == 3 and hasMovableItem then
                return offsetPosition
            end

            ::continue::

        end
    end

    -- Retry to allow pushing into occupied tiles if needed
    if round < 3 then
        return findPushPosition(creature, doorItem, round + 1)
    end
    return nil
end


local door = Action()

function door.onUse(player, item, fromPosition, target, toPosition, isHotkey)
    local itemId = item:getId()
    if table.contains(closedQuestDoors, itemId) then
        if player:getStorageValue(item.actionid) ~= -1 or player:getGroup():getAccess() then
            item:transform(itemId + 1)
            player:teleportTo(toPosition, true)
        else
            player:sendTextMessage(MESSAGE_EVENT_ADVANCE, "The door seems to be sealed against unwanted intruders.")
        end
        return true
    elseif table.contains(closedLevelDoors, itemId) then
        if item.actionid > 0 and player:getLevel() >= item.actionid - actionIds.levelDoor or player:getGroup():getAccess() then
            item:transform(itemId + 1)
            player:teleportTo(toPosition, true)
        else
            player:sendTextMessage(MESSAGE_EVENT_ADVANCE, "Only the worthy may pass.")
        end
        return true
    elseif table.contains(keys, itemId) then
        local tile = Tile(toPosition)
        if not tile then
            return false
        end
        target = tile:getTopVisibleThing()
        if target.actionid == 0 then
            return false
        end
        if table.contains(keys, target.itemid) then
            return false
        end
        if not table.contains(openDoors, target.itemid) and not table.contains(closedDoors, target.itemid) and not table.contains(lockedDoors, target.itemid) then
            return false
        end
        if item.actionid ~= target.actionid then
            player:sendTextMessage(MESSAGE_STATUS_SMALL, "The key does not match.")
            return true
        end
        local transformTo = target.itemid + 2
        if table.contains(openDoors, target.itemid) then
            transformTo = target.itemid - 2
        elseif table.contains(closedDoors, target.itemid) then
            transformTo = target.itemid - 1
        end
        target:transform(transformTo)
        return true
    elseif table.contains(lockedDoors, itemId) then
        player:sendTextMessage(MESSAGE_INFO_DESCR, "It is locked.")
        return true   
    elseif table.contains(openDoors, itemId) or table.contains(openExtraDoors, itemId) or table.contains(openHouseDoors, itemId) then
        local creaturePositionTable = {}
        local doorCreatures = Tile(toPosition):getCreatures()
        if doorCreatures and #doorCreatures > 0 then
            for _, doorCreature in pairs(doorCreatures) do
                local pushPosition = findPushPosition(doorCreature, item, 1)
                if not pushPosition then
                    player:sendCancelMessage(RETURNVALUE_NOTENOUGHROOM)
                    return true
                end
                table.insert(creaturePositionTable, {creature = doorCreature, position = pushPosition})
            end
            for _, tableCreature in ipairs(creaturePositionTable) do
                tableCreature.creature:teleportTo(tableCreature.position, true)
            end
        end

        item:transform(itemId - 1)
        return true   
    elseif table.contains(closedDoors, itemId) or table.contains(closedExtraDoors, itemId) or table.contains(closedHouseDoors, itemId) then
        item:transform(itemId + 1)
        return true
    end
    return false
end

local doorTables = {keys, openDoors, closedDoors, lockedDoors, openExtraDoors, closedExtraDoors, openHouseDoors, closedHouseDoors, closedQuestDoors, closedLevelDoors}
for _, doors in pairs(doorTables) do
    for _, doorId in pairs(doors) do
        door:id(doorId)
    end
end
door:register()