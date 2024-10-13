local templeTeleportItem = Action()

function templeTeleportItem.onUse(player, item, fromPosition, target, toPosition, isHotkey)
    local cooldown = PlayerStorageKeys.templeTeleportItemCooldown -- Cooldown storage key
    local cooldownTime = 600 -- Cooldown time in seconds (10 minutes)

    -- Check if the cooldown has expired
    local currentTime = os.time()
    if player:getStorageValue(cooldown) > currentTime then
        local remainingTime = player:getStorageValue(cooldown) - currentTime
        player:sendTextMessage(MESSAGE_EVENT_ADVANCE, string.format("Cooldown: %d seconds.", remainingTime))
        return true
    end

    -- Check if the player is in a no-logout zone
    if getTileInfo(player:getPosition()).nologout then
        player:sendCancelMessage("You can't use this when you are inside a boss room.")
        player:getPosition():sendMagicEffect(CONST_ME_POFF)
        return true
    end

    -- Check if the player has premium time
    if not player:isPremium() then
        player:sendCancelMessage("You need active premium time to use this item.")
        player:getPosition():sendMagicEffect(CONST_ME_POFF)
        return true
    end

    -- Check if the player is not in combat or PZ-locked
    if not player:isPzLocked() and not player:getCondition(CONDITION_INFIGHT, CONDITIONID_DEFAULT) then
        local playerPos = player:getPosition()
        local town = player:getTown()
        local templePos = town and town:getTemplePosition() or Town(1):getTemplePosition()

        -- Perform teleportation
        playerPos:sendMagicEffect(CONST_ME_POFF)
        player:teleportTo(templePos, false)
        templePos:sendMagicEffect(CONST_ME_TELEPORT)

        -- Set the cooldown after successful teleport
        player:setStorageValue(cooldown, currentTime + cooldownTime)
        player:sendTextMessage(MESSAGE_EVENT_ADVANCE, string.format(
            "Welcome home adventurer! This item will be on cooldown for %d seconds.", cooldownTime))
    else
        player:sendCancelMessage("You can't use this shortly after being in combat.")
        player:getPosition():sendMagicEffect(CONST_ME_POFF)
    end

    return true
end

templeTeleportItem:id(6119)
templeTeleportItem:register()
