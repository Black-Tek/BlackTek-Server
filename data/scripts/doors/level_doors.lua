local levelDoor = Action()

function levelDoor.onUse(player, item, fromPosition, target, toPosition, isHotkey)
    local doorItem = findDoorOnTile(toPosition)
    if not doorItem then
        return false
    end

    local doorId = doorItem:getId()
    local isLvlDoor, doorState, pairedId = isLevelDoor(doorId)

    if not isLvlDoor then
        return false
    end

    local doorActionId = doorItem:getActionId()
    local requiredLevel = getRequiredLevel(doorActionId)

    if doorState == "closed" then
        if player:hasGamemasterAccess() and doorConfig.allowGamemasterBypass then
            teleportOnDoor(player, toPosition)
            return true
        end

        if doorActionId == 0 and not doorConfig.levelDoorAllowMissingActionId then
            player:sendTextMessage(MESSAGE_EVENT_ADVANCE, levelDoors.messages.noActionId)
            return true
        end

        if not meetsLevelRequirement(player, requiredLevel) then
            player:sendTextMessage(MESSAGE_EVENT_ADVANCE, string.format(levelDoors.messages.needLevel, requiredLevel))
            return true
        end

        local playerPosition = player:getPosition()
        player:teleportTo(toPosition, true)
        storeCreatureEntryPosition(player, playerPosition)
        doorItem:transform(pairedId)
        return true
    end

    if doorState == "open" then
        if doorConfig.levelDoorRequireToClose then
            if not meetsLevelRequirement(player, requiredLevel) then
                player:sendTextMessage(MESSAGE_EVENT_ADVANCE,
                    string.format(levelDoors.messages.needLevelClosing, requiredLevel))
                return true
            end
        end

        if closeDoor(toPosition, doorItem) then
            return true
        else
            player:sendTextMessage(MESSAGE_EVENT_ADVANCE, doorConfig.closeErrorMessage)
            return true
        end
    end

    return false
end

for closedId, openId in pairs(levelDoors.ids) do
    levelDoor:id(closedId, openId)
end

levelDoor:register()

local levelDoorStepIn = MoveEvent()

function levelDoorStepIn.onStepIn(creature, item, position, fromPosition)
    if not creature:isPlayer() then
        if not doorConfig.levelDoorAllowMonsters then
            creature:teleportTo(fromPosition, true)
            return false
        end
        return true
    end

    local player = creature

    if player:hasGamemasterAccess() and doorConfig.allowGamemasterBypass then
        return true
    end

    local doorActionId = item:getActionId()
    local requiredLevel = getRequiredLevel(doorActionId)

    if doorActionId == 0 and not doorConfig.levelDoorAllowMissingActionId then
        player:sendTextMessage(MESSAGE_EVENT_ADVANCE, levelDoors.messages.noActionId)
        player:teleportTo(fromPosition, true)
        return false
    end

    if not meetsLevelRequirement(player, requiredLevel) then
        player:sendTextMessage(MESSAGE_EVENT_ADVANCE, string.format(levelDoors.messages.needLevel, requiredLevel))
        player:teleportTo(fromPosition, true)
        return false
    end
    storeCreatureEntryPosition(creature, fromPosition)
    return true
end

for closedId, openId in pairs(levelDoors.ids) do
    levelDoorStepIn:id(openId)
end

levelDoorStepIn:register()

local levelDoorStepOut = MoveEvent()

function levelDoorStepOut.onStepOut(creature, item, position, fromPosition)
    clearCreatureEntryPosition(creature)
    local doorPosition = item:getPosition()
    local tile = Tile(doorPosition)

    if tile then
        local creatures = tile:getCreatures()
        if creatures then
            for _, c in ipairs(creatures) do
                if not (doorConfig.allowGamemasterBypass and c:hasGamemasterAccess()) then
                    return true
                end
            end
        end
    end

    return closeDoor(doorPosition, item)
end

for closedId, openId in pairs(levelDoors.ids) do
    levelDoorStepOut:id(openId)
end

levelDoorStepOut:register()
