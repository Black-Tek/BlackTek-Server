-- store the position of the player when they enter a door
-- now we can push them back there later
DOOR_ENTRY_STORAGE = {
    X = 50000,
    Y = 50001,
    Z = 50002
}

doorConfig = {
    -- note: defaults are set as recommended
    allowPushPlayers = true,
    allowPushMonsters = true,
    allowPushSkulled = true,                         -- players
    allowPushThroughMagicWalls = false,              -- magic wall & wild growth
    allowPushIntoProtectionZone = true,              -- push creatures into PZ
    allowPushOutOfProtectionZone = true,             -- push creatures out of PZ
    allowPushOntoMagicFields = true,                 -- push creatures onto fire fields, poison fields, etc.
    deleteUnpushableItems = true,                    -- if items don't succesfully move they will be deleted
    prioritizeEntryPosition = true,                  -- try to push creatures back to entry position first
    closeErrorMessage = "There is not enough room.", -- generic message applies to all doors when a player tries and fails to close a door
    -- locked_doors
    closeDoorBeforeLocking = true,                   -- if true, doors must already be closed to get locked. if false, the door will close and lock at the same time
    -- level_doors
    levelDoorRequireToClose = true,                  -- correct level required to close door, if false then lower level players can close level doors to push players out of them
    levelDoorAllowMonsters = false,                  -- if true, monsters can pass through open level doors
    -- quest_doors
    questDoorRequireToClose = true,
    questDoorAllowMonsters = false,
}

-- normalDoors open and close without any rules except for the doorConfig
-- also includes gates and things that function like doors but might not look like a door
normalDoors = {
    -- [closedId] = openId
    [1210]=1211, [1213]=1214, [1232]=1233, [1235]=1236, [1250]=1251, [1253]=1254, [1539]=1540, [1541]=1542,
    [1634]=1635, [1636]=1637, [1638]=1639, [1640]=1641, [3536]=3537, [3545]=3546, [4914]=4915, [4917]=4918,
    [5099]=5100, [5108]=5109, [5117]=5118, [5126]=5127, [5135]=5136, [5138]=5139, [5141]=5142, [5144]=5145,
    [5279]=5280, [5282]=5283, [5733]=5734, [5736]=5737, [6193]=6194, [6196]=6197, [6250]=6251, [6253]=6254,
    [6795]=6796, [6797]=6798, [6799]=6800, [6801]=6802, [6892]=6893, [6901]=6902, [6959]=6960, [6961]=6962,
    [7034]=7035, [7043]=7044, [7054]=7055, [7056]=7057, [8542]=8543, [8545]=8546, [9166]=9167, [9169]=9170,
    [9268]=9269, [9271]=9272,
    [10269]=10270, [10272]=10273, [10469]=10470, [10478]=10479, [10776]=10777, [10785]=10786, [12093]=12094,
    [12100]=12101, [12189]=12190, [12198]=12199, [12692]=12695, [12701]=12703, [14633]=14635, [17235]=17236,
    [17237]=17238, [19841]=19842, [19850]=19851, [19981]=19982, [19990]=19991, [20274]=20275, [20283]=20284,
    [22815]=22816, [22824]=22825, [25158]=25159, [25160]=25161, [25284]=25285, [25291]=25292
}

-- note: house specific logic is handled in the src however for additional custom logic house_doors.lua is provided
houseDoors = {
    [1219]=1220, [1221]=1222, [1237]=1238, [1239]=1240, [3538]=3539, [3547]=3548, [5082]=5083, [5084]=5085,
    [5101]=5102, [5110]=5111, [5119]=5120, [5128]=5129, [5284]=5285, [5286]=5287, [5515]=5516, [5517]=5518,
    [6198]=6199, [6200]=6201, [6255]=6256, [6257]=6258, [6894]=6895, [6903]=6904, [7036]=7037, [7045]=7046,
    [8547]=8548, [8549]=8550, [9171]=9172, [9173]=9174, [9273]=9274, [9275]=9276,
    [10274]=10275, [10276]=10277, [10471]=10472, [10480]=10481, [13020]=13021, [13022]=13023, [18208]=18209,
    [19843]=19844, [19852]=19853, [19983]=19984, [19992]=19993, [20276]=20277, [20285]=20286, [22817]=22818,
    [22826]=22827
}


levelDoors = {
    ids = {
        [1227]=1228, [1229]=1230, [1245]=1246, [1247]=1248, [1259]=1260, [1261]=1262, [3540]=3541, [3549]=3550,
        [5103]=5104, [5112]=5113, [5121]=5122, [5130]=5131, [5292]=5293, [5294]=5295, [6206]=6207, [6208]=6209,
        [6263]=6264, [6265]=6266, [6896]=6897, [6905]=6906, [7038]=7039, [7047]=7048, [8555]=8556, [8557]=8558,
        [9179]=9180, [9181]=9182, [9281]=9282, [9283]=9284, [9653]=9654, [9655]=9656,
        [10282]=10283, [10284]=10285, [10473]=10474, [10482]=10483, [10780]=10781, [10789]=10790, [12095]=12096,
        [12102]=12103, [12195]=12196, [12204]=12205, [19845]=19846, [19854]=19855, [19985]=19986, [19994]=19995,
        [20278]=20279, [20287]=20288, [22819]=22820, [22828]=22829, [25286]=25287, [25293]=25294
    },
    messages = {
        needLevel = "Only the worthy may pass.",
        noActionId = "You are not worthy.",
        needLevelClosing = "Only the worthy may close this door."
    }
}

-- item ids of keys
-- used to unlock/lock lockedDoors when key action id = door action id
-- many keys can be made from each key id simply by changing the action id and name using a script
doorKeys = { 2086, 2087, 2088, 2089, 2090, 2091, 2092, 10032, 21537, 25338 }

-- lockedDoors are locked/unlocked using doorKeys but otherwise have no special rules once unlocked
-- IMPORTANT: lockedDoors transform into normalDoors when unlocked, so they must have a corresponding entry in normalDoors
-- add 1 to the lockedDoor ID to get the normalDoor ID
lockedDoors = {
    ids = {
        1209, 1212, 1231, 1234, 1249, 1252, 3535, 3544, 4913, 4916, 5098, 5107, 5116, 5125, 5134, 5137, 5140, 5143,
        5278, 5281, 5732, 5735, 6192, 6195, 6249, 6252, 6891, 6900, 7033, 7042, 8541, 8544, 9165, 9168, 9267, 9270,
        10268, 10271, 10468, 10477, 10775, 10784, 12092, 12099, 12188, 12197, 13236, 13237, 14634, 14641, 19840, 19849,
        19980, 19989, 20273, 20282, 22814, 22823, 25283, 25290
    },
    messages = {
        locked = "The door is locked.",
        lock = "You locked the door.",
        unlock = "You unlocked the door.",
        wrongKey = "The key does not fit.",
        needClose = "Close the door first."
    }
}

questDoors = {
    ids = {
    [1223]=1224, [1225]=1226, [1241]=1242, [1243]=1244, [1255]=1256, [1257]=1258, [3542]=3543, [3551]=3552,
    [5105]=5106, [5114]=5115, [5123]=5124, [5132]=5133, [5288]=5289, [5290]=5291, [5745]=5746, [5748]=5749,
    [6202]=6203, [6204]=6205, [6259]=6260, [6261]=6262, [6898]=6899, [6907]=6908, [7040]=7041, [7049]=7050,
    [8551]=8552, [8553]=8554, [9175]=9176, [9177]=9178, [9277]=9278, [9279]=9280, [9649]=9650, [9651]=9652,
    [10278]=10279, [10280]=10281, [10475]=10476, [10484]=10485, [10782]=10783, [10791]=10792, [12097]=12098,
    [12104]=12105, [12193]=12194, [12202]=12203, [19847]=19848, [19856]=19857, [19987]=19988, [19996]=19997,
    [20280]=20281, [20289]=20290, [22821]=22822, [22830]=22831, [25162]=25163, [25164]=25165, [25288]=25289,
    [25295]=25296
    },
    messages = {
        sealed = "The door seems to be sealed against unwanted intruders.",
        noActionId = "The door is sealed.",
        needQuestClosing = "Unwanted intruders cannot close this door."
    }
}

local function shuffle(table)
    local shuffled = {}
    for i, v in ipairs(table) do
        shuffled[i] = v
    end
    for i = #shuffled, 2, -1 do
        local j = math.random(i)
        shuffled[i], shuffled[j] = shuffled[j], shuffled[i]
    end
    return shuffled
end

local function isSkulled(creature)
    if not creature then return false end
    if not creature:isPlayer() then return false end
    return creature:getSkull() > SKULL_NONE
end

function isDoor(itemId)
    if normalDoors[itemId] then
        return true, "closed", normalDoors[itemId]
    end

    for closedId, openId in pairs(normalDoors) do
        if openId == itemId then
            return true, "open", closedId
        end
    end
    return false, nil, nil
end

function storeCreatureEntryPosition(creature, fromPosition)
    if not creature then return end

    creature:setStorageValue(DOOR_ENTRY_STORAGE.X, fromPosition.x)
    creature:setStorageValue(DOOR_ENTRY_STORAGE.Y, fromPosition.y)
    creature:setStorageValue(DOOR_ENTRY_STORAGE.Z, fromPosition.z)
end

function getCreatureEntryPosition(creature)
    if not creature then return nil end

    local x = creature:getStorageValue(DOOR_ENTRY_STORAGE.X)
    local y = creature:getStorageValue(DOOR_ENTRY_STORAGE.Y)
    local z = creature:getStorageValue(DOOR_ENTRY_STORAGE.Z)

    if x == -1 or y == -1 or z == -1 then
        return nil
    end

    return Position(x, y, z)
end

function clearCreatureEntryPosition(creature)
    if not creature then return end

    creature:setStorageValue(DOOR_ENTRY_STORAGE.X, -1)
    creature:setStorageValue(DOOR_ENTRY_STORAGE.Y, -1)
    creature:setStorageValue(DOOR_ENTRY_STORAGE.Z, -1)
end

function canPushToPosition(position, round, doorPosition)
    if not position then return false end

    local tile = Tile(position)
    if not tile then return false end

    local hasMagicWall = tile:getItemById(ITEM_MAGICWALL)
    local hasWildGrowth = tile:getItemById(ITEM_WILDGROWTH)

    if hasMagicWall or hasWildGrowth then
        if doorConfig.allowPushThroughMagicWalls then
            return true
        else
            return false
        end
    end

    if not doorConfig.allowPushOntoMagicFields then
        local items = tile:getItems()
        if items then
            for _, item in ipairs(items) do
                local itemType = ItemType(item:getId())
                if itemType:isMagicField() then
                    return false
                end
            end
        end
    end

    if tile:hasFlag(TILESTATE_BLOCKSOLID) then return false end
    if tile:hasFlag(TILESTATE_FLOORCHANGE) then return false end

    local targetInPZ = tile:hasFlag(TILESTATE_PROTECTIONZONE)
    if doorPosition then
        local doorTile = Tile(doorPosition)
        local doorInPZ = doorTile and doorTile:hasFlag(TILESTATE_PROTECTIONZONE) or false

        if targetInPZ and not doorInPZ then
            if not doorConfig.allowPushIntoProtectionZone then
                return false
            end
        end

        if not targetInPZ and doorInPZ then
            if not doorConfig.allowPushOutOfProtectionZone then
                return false
            end
        end
    end

    local items = tile:getItems()
    if items then
        for _, item in ipairs(items) do
            local itemId = item:getId()
            if isDoor(itemId) then
                return false
            end
        end
    end

    local creatures = tile:getCreatures()
    local creatureCount = creatures and #creatures or 0

    if round == 1 and creatureCount == 0 then
        return true
    end

    if round == 2 and creatureCount > 0 then
        local hasSkulled = false
        for _, creature in ipairs(creatures) do
            if isSkulled(creature) then
                hasSkulled = true
                break
            end
        end

        if hasSkulled and not doorConfig.allowPushSkulled then
            return false
        end

        return true
    end

    if round == 3 then
        return tile:isWalkable()
    end

    return false
end

function pushCreatureFromDoor(creature, doorPosition)
    if not creature then return false end

    if creature:isPlayer() and not doorConfig.allowPushPlayers then
        return false
    end

    if not creature:isPlayer() and not doorConfig.allowPushMonsters then
        return false
    end

    if isSkulled(creature) and not doorConfig.allowPushSkulled then
        return false
    end

    if doorConfig.prioritizeEntryPosition then
        local entryPos = getCreatureEntryPosition(creature)
        if entryPos then
            for round = 1, 3 do
                if canPushToPosition(entryPos, round, doorPosition) then
                    creature:teleportTo(entryPos, true)
                    clearCreatureEntryPosition(creature)
                    return true
                end
            end
        end
    end

    local directions = {
        { x = 0,  y = -1 }, -- North
        { x = 0,  y = 1 },  -- South
        { x = -1, y = 0 },  -- West
        { x = 1,  y = 0 },  -- East
        { x = -1, y = -1 }, -- Northwest
        { x = 1,  y = -1 }, -- Northeast
        { x = -1, y = 1 },  -- Southwest
        { x = 1,  y = 1 }   -- Southeast
    }

    for round = 1, 3 do
        local shuffledDirs = shuffle(directions)
        for _, dir in ipairs(shuffledDirs) do
            local targetPos = Position(
                doorPosition.x + dir.x,
                doorPosition.y + dir.y,
                doorPosition.z
            )
            if canPushToPosition(targetPos, round, doorPosition) then
                creature:teleportTo(targetPos, true)
                clearCreatureEntryPosition(creature)
                return true
            end
        end
    end
    clearCreatureEntryPosition(creature)
    return false
end

function pushItemFromDoor(item, doorPosition)
    if not item then return false end
    local directions = {
        { x = 0,  y = -1 }, -- North
        { x = 0,  y = 1 },  -- South
        { x = -1, y = 0 },  -- West
        { x = 1,  y = 0 },  -- East
        { x = -1, y = -1 }, -- Northwest
        { x = 1,  y = -1 }, -- Northeast
        { x = -1, y = 1 },  -- Southwest
        { x = 1,  y = 1 }   -- Southeast
    }
    for round = 1, 3 do
        local shuffledDirs = shuffle(directions)
        for _, dir in ipairs(shuffledDirs) do
            local targetPos = Position(
                doorPosition.x + dir.x,
                doorPosition.y + dir.y,
                doorPosition.z
            )
            if canPushToPosition(targetPos, round, doorPosition) then
                item:moveTo(targetPos)
                return true
            end
        end
    end
    return false
end

function closeDoor(doorPosition, doorItem)
    if not doorPosition or not doorItem then return false end
    local doorId = doorItem:getId()
    local closedId = nil

    local isOpenDoor = false
    for closed, open in pairs(normalDoors) do
        if open == doorId then
            closedId = closed
            isOpenDoor = true
            break
        end
    end

    if not isOpenDoor then
        return false
    end

    local tile = Tile(doorPosition)
    if not tile then return false end

    local creatures = tile:getCreatures()
    if creatures then
        for _, creature in ipairs(creatures) do
            if not pushCreatureFromDoor(creature, doorPosition) then
                return false
            end
        end
    end

    local items = tile:getItems()
    if items then
        local movableItems = {}
        for _, item in ipairs(items) do
            if item and item:getId() ~= doorId then
                local itemType = ItemType(item:getId())
                if itemType:isMovable() then
                    table.insert(movableItems, item)
                end
            end
        end

        for i = #movableItems, 1, -1 do
            local item = movableItems[i]
            local itemType = ItemType(item:getId())

            if itemType:isBlocking() then
                if not pushItemFromDoor(item, doorPosition) then
                    return false
                end
            else
                if not pushItemFromDoor(item, doorPosition) then
                    if doorConfig.deleteUnpushableItems then
                        item:remove()
                    else
                        return false
                    end
                end
            end
        end
    end

    doorItem:transform(closedId)
    return true
end


lockedDoorsSet = {}
for _, id in ipairs(lockedDoors.ids) do
    lockedDoorsSet[id] = true
end

function isLockedDoor(doorId)
    return lockedDoorsSet[doorId] == true
end

function getUnlockedDoorId(lockedDoorId)
    local unlockedId = lockedDoorId + 1
    if normalDoors[unlockedId] then
        return unlockedId
    end
    return nil
end

function getLockedDoorId(unlockedDoorId)
    local lockedId = unlockedDoorId - 1
    if isLockedDoor(lockedId) then
        return lockedId
    end
    return nil
end

function findDoorOnTile(position)
    local tile = Tile(position)
    if not tile then
        return nil
    end

    local items = tile:getItems()
    if not items then
        return nil
    end

    for _, item in ipairs(items) do
        local itemType = ItemType(item:getId())
        if itemType:isDoor() then
            return item
        end
    end

    return nil
end

function isLevelDoor(doorId)
    if levelDoors.ids[doorId] then
        return true, "closed", levelDoors.ids[doorId]
    end

    for closedId, openId in pairs(levelDoors.ids) do
        if openId == doorId then
            return true, "open", closedId
        end
    end

    return false, nil, nil
end

function getRequiredLevel(actionId)
    if actionId >= actionIds.levelCheck and actionId <= actionIds.levelCheckLast then
        return (actionId - actionIds.levelCheck) * levelCheckConfig.normal + levelCheckConfig.normalLevelOffset
    elseif actionId >= actionIds.levelCheckExtended and actionId <= actionIds.levelCheckExtendedLast then
        return (actionId - actionIds.levelCheckExtended) * levelCheckConfig.extended +
        levelCheckConfig.extendedLevelOffset
    end
    return nil
end

function meetsLevelRequirement(player, requiredLevel)
    if not requiredLevel then
        return true
    end
    return player:getLevel() >= requiredLevel
end

function canOpenQuestDoor(player, actionId)
    if not player then
        return false
    end

    local storageValue = player:getStorageValue(actionId)
    return storageValue ~= nil and storageValue ~= -1 and storageValue ~= 0
end

function isQuestDoor(doorId)
    if questDoors.ids[doorId] then
        return true, "closed", questDoors.ids[doorId]
    end

    for closedId, openId in pairs(questDoors.ids) do
        if openId == doorId then
            return true, "open", closedId
        end
    end

    return false, nil, nil
end

function isGamemaster(creature)
    if not creature:isPlayer() then return false end
    return creature:getGroup():getId() >= 4
end

function teleportThroughDoor(player, fromPosition, doorPosition)
    local deltaX = doorPosition.x - fromPosition.x
    local deltaY = doorPosition.y - fromPosition.y

    local isVertical = math.abs(deltaY) > math.abs(deltaX) and 1 or 0
    local isHorizontal = 1 - isVertical

    local signX = deltaX > 0 and 1 or -1
    local signY = deltaY > 0 and 1 or -1

    local destinationPos = Position(
        doorPosition.x + (isHorizontal * signX),
        doorPosition.y + (isVertical * signY),
        doorPosition.z
    )

    return player:teleportTo(destinationPos)
end
