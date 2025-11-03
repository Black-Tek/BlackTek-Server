local normalDoor = Action()
function normalDoor.onUse(player, item, fromPosition, target, toPosition, isHotkey)
    local doorId = item:getId()
    local isDoorItem, doorState, pairedId = isDoor(doorId)

    if not isDoorItem then
        return false
    end

    if doorState == "closed" then
        if isGamemaster(player) and doorConfig.allowGamemasterBypass then
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


local doorStepIn = MoveEvent()
function doorStepIn.onStepIn(creature, item, position, fromPosition)
    storeCreatureEntryPosition(creature, fromPosition)
    return true
end
for closedId, openId in pairs(normalDoors) do
    doorStepIn:id(openId)
end
doorStepIn:register()


local doorStepOut = MoveEvent()
function doorStepOut.onStepOut(creature, item, position, fromPosition)
    return true
end
for closedId, openId in pairs(normalDoors) do
    doorStepOut:id(openId)
end
doorStepOut:register()
