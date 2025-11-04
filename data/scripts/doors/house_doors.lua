-- house related logic is handled in src (house.cpp, house.h)
-- this script could even be combined with normal_doors
-- however it's kept separate to make it easier to apply custom logic
local houseDoor = Action()
function houseDoor.onUse(player, item, fromPosition, target, toPosition, isHotkey)
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

for closedId, openId in pairs(houseDoors) do
    houseDoor:id(closedId, openId)
end
houseDoor:register()


local doorStepIn = MoveEvent()
function doorStepIn.onStepIn(creature, item, position, fromPosition)
    if doorConfig.allowGamemasterBypass and creature:isPlayer() and creature:hasGamemasterAccess() then
        return true
    end

    storeCreatureEntryPosition(creature, fromPosition)
    return true
end

for closedId, openId in pairs(houseDoors) do
    doorStepIn:id(openId)
end
doorStepIn:register()


local doorStepOut = MoveEvent()
function doorStepOut.onStepOut(creature, item, position, fromPosition)
    clearCreatureEntryPosition(creature)
    return true
end

for closedId, openId in pairs(houseDoors) do
    doorStepOut:id(openId)
end
doorStepOut:register()
