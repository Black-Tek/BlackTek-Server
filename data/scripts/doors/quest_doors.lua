local questDoor = Action()

function questDoor.onUse(player, item, fromPosition, target, toPosition, isHotkey)
    local doorId = item:getId()
    local isQuestDoor, doorState, pairedId = isQuestDoor(doorId)

    if not isQuestDoor then
        return false
    end

    local doorActionId = item:getActionId()

    if doorState == "closed" then

        if isGamemaster(player) and doorConfig.allowGamemasterBypass then
            teleportOnDoor(player, toPosition)
            return true
        end

        if doorActionId == 0 then
            player:sendTextMessage(MESSAGE_EVENT_ADVANCE, questDoors.messages.noActionId)
            return true
        end

        if not canOpenQuestDoor(player, doorActionId) then
            player:sendTextMessage(MESSAGE_EVENT_ADVANCE, questDoors.messages.sealed)
            return true
        end

        player:teleportTo(toPosition, true)
        item:transform(pairedId)
        return true
    end

    if doorState == "open" then
        if doorActionId ~= 0 and doorConfig.questDoorRequireToClose then
            if not canOpenQuestDoor(player, doorActionId) then
                player:sendTextMessage(MESSAGE_EVENT_ADVANCE, questDoors.messages.needQuestClosing)
                return true
            end
        end

        if closeDoor(toPosition, item) then
            return true
        else
            player:sendTextMessage(MESSAGE_EVENT_ADVANCE, doorConfig.closeErrorMessage)
            return true
        end
    end

    return false
end

for closedId, openId in pairs(questDoors.ids) do
    questDoor:id(closedId, openId)
end

questDoor:register()


local questDoorStepIn = MoveEvent()

function questDoorStepIn.onStepIn(creature, item, position, fromPosition)
    if not creature:isPlayer() then
        if not doorConfig.questDoorAllowMonsters then
            creature:teleportTo(fromPosition, true)
            return false
        end
        return true
    end

    local player = creature

    if isGamemaster(player) and doorConfig.allowGamemasterBypass then
        return true
    end

    local doorActionId = item:getActionId()

    if doorActionId == 0 then
        storeCreatureEntryPosition(creature, fromPosition)
        return true
    end

    if not canOpenQuestDoor(player, doorActionId) then
        player:sendTextMessage(MESSAGE_EVENT_ADVANCE, questDoors.messages.sealed)
        player:teleportTo(fromPosition, true)
        return false
    end

    storeCreatureEntryPosition(creature, fromPosition)
    return true
end

for closedId, openId in pairs(questDoors.ids) do
    questDoorStepIn:id(openId)
end

questDoorStepIn:register()


local questDoorStepOut = MoveEvent()

function questDoorStepOut.onStepOut(creature, item, position, fromPosition)
    local doorPosition = item:getPosition()
    local tile = Tile(doorPosition)

    if tile then
        local creatures = tile:getCreatures()
        if creatures then
            for _, c in ipairs(creatures) do
                if not (doorConfig.allowGamemasterBypass and isGamemaster(c)) then
                    return true
                end
            end
        end
    end

    closeDoor(doorPosition, item)
    return true
end

for closedId, openId in pairs(questDoors.ids) do
    questDoorStepOut:id(openId)
end

questDoorStepOut:register()
