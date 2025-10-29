local useKeyOnDoor = Action()

function useKeyOnDoor.onUse(player, item, fromPosition, target, toPosition, isHotkey)
    local doorItem = findDoorOnTile(toPosition)
    if not doorItem then
        return false
    end

    local doorId = doorItem:getId()
    local doorActionId = doorItem:getActionId()
    local keyActionId = item:getActionId()

    if doorActionId == 0 or keyActionId == 0 then
        player:sendTextMessage(MESSAGE_EVENT_ADVANCE, lockedDoors.messages.wrongKey)
        return false
    end

    if keyActionId ~= doorActionId then
        player:sendTextMessage(MESSAGE_EVENT_ADVANCE, lockedDoors.messages.wrongKey)
        return true
    end

    if isLockedDoor(doorId) then
        local unlockedId = getUnlockedDoorId(doorId)
        if not unlockedId then
            return false
        end

        doorItem:transform(unlockedId)

        player:sendTextMessage(MESSAGE_EVENT_ADVANCE, lockedDoors.messages.unlock)
        return true
    end

    local isDoorItem, doorState, pairedId = isDoor(doorId)
    if isDoorItem and doorState == "closed" then
        local lockedId = getLockedDoorId(doorId)
        if not lockedId then
            return false
        end

        doorItem:transform(lockedId)

        player:sendTextMessage(MESSAGE_EVENT_ADVANCE, lockedDoors.messages.lock)
        return true
    end

    if isDoorItem and doorState == "open" then
        if doorConfig.closeDoorBeforeLocking then
            player:sendTextMessage(MESSAGE_EVENT_ADVANCE, lockedDoors.messages.needClose)
            return true
        else
            if not closeDoor(toPosition, doorItem) then
                player:sendTextMessage(MESSAGE_EVENT_ADVANCE, doorConfig.ids.closeErrorMessage)
                return true
            end
            local lockedId = getLockedDoorId(doorItem:getId())
            if not lockedId then
                return false
            end

            doorItem:transform(lockedId)
            player:sendTextMessage(MESSAGE_EVENT_ADVANCE, lockedDoors.messages.lock)
            return true
        end
    end

    return false
end

for _, keyId in ipairs(doorKeys) do
    useKeyOnDoor:id(keyId)
end

useKeyOnDoor:register()

local useLockedDoor = Action()

function useLockedDoor.onUse(player, item, fromPosition, target, toPosition, isHotkey)
    player:sendTextMessage(MESSAGE_EVENT_ADVANCE, lockedDoors.messages.locked)
    return true
end

for _, lockedId in ipairs(lockedDoors.ids) do
    useLockedDoor:id(lockedId)
end

useLockedDoor:register()
