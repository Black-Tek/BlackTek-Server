local homeScroll = Action()

function homeScroll.onUse(player, item, fromPosition, target, toPosition, isHotkey)
    local cooldown = PlayerStorageKeys.houseTeleportItemCooldown -- Cooldown storage value
    local cooldownTime = 5 -- cd in seconds

    if player:getStorageValue(cooldown) > os.time() then -- Timed Cooldown
        local remainingTime = player:getStorageValue(cooldown) - os.time()
        player:sendTextMessage(MESSAGE_EVENT_ADVANCE,
            'Cooldown: ' .. player:getStorageValue(cooldown) - os.time() .. ' seconds.')
        return true
    end

    if getTileInfo(player:getPosition()).nologout then -- Using in a nologout zone
        player:sendCancelMessage("You can't use this when you are inside a boss room.")
        player:getPosition():sendMagicEffect(3)
        return true
    end

    if not player:isPremium() then -- check if player has pdremium time
        player:sendCancelMessage("You need active premium time to use this item.")
        Position.sendMagicEffect(Creature.getPosition(player), CONST_ME_POFF)
        return true
    end
    if not player:isPzLocked() and not player:getCondition(CONDITION_INFIGHT, CONDITIONID_DEFAULT) then -- check if player is not in combat
        if Player.getHouse(player) then -- check if player has house
            local doorPosition = House.getExitPosition(Player.getHouse(player))
            Position.sendMagicEffect(Creature.getPosition(player), CONST_ME_POFF)
            Creature.teleportTo(player, doorPosition, false)
            Position.sendMagicEffect(doorPosition, CONST_ME_TELEPORT)
            player:setStorageValue(cooldown, os.time() + cooldownTime)
            player:sendTextMessage(MESSAGE_EVENT_ADVANCE,
                'You have been teleported to your house. This item will be on cooldown for ' .. cooldownTime ..
                    ' seconds.')
        else
            player:sendCancelMessage("You do not have a house.")
            Position.sendMagicEffect(Creature.getPosition(player), CONST_ME_POFF)
        end
    else
        player:sendCancelMessage("You can't use this shortly after being in combat.")
        Position.sendMagicEffect(Creature.getPosition(player), CONST_ME_POFF)
    end
    return true
end
homeScroll:id(2326)
homeScroll:register()
