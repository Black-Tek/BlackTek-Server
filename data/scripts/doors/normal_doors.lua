local normalDoor = ItemEvent()
normalDoor.onUse = function(player, item, fromPosition, target, toPosition, isHotkey)
    local doorId = item:getId()
    local isDoorItem, doorState, pairedId = isDoor(doorId)

    if not isDoorItem then
        return false
    end

    if doorState == "closed" then
        if player:hasGamemasterAccess() and doorConfig.allowGamemasterBypass then
            teleportOnDoor(player, toPosition)
            return true
        end

        item:transform(pairedId)
        return true
    end

    if doorState == "open" then
        if closeDoor(toPosition, item) then
            return true
        else
            player:sendTextMessage(MESSAGE_EVENT_ADVANCE, doorConfig.closeErrorMessage)
            return true
        end
    end
    return false
end

for closedId, openId in pairs(normalDoors) do
    normalDoor:id(closedId, openId)
end
normalDoor:register()


local doorStepOn = ItemEvent()
doorStepOn.onStepOn = function(creature, item, position, fromPosition)
    storeCreatureEntryPosition(creature, fromPosition)
    return true
end
for closedId, openId in pairs(normalDoors) do
    doorStepOn:id(openId)
end
doorStepOn:register()


local doorStepOff = ItemEvent()
doorStepOff.onStepOff = function(creature, item, position, fromPosition)
    clearCreatureEntryPosition(creature)
    return true
end
for closedId, openId in pairs(normalDoors) do
    doorStepOff:id(openId)
end
doorStepOff:register()
