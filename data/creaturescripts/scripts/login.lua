function onLogin(player)
	local serverName = configManager.getString(configKeys.SERVER_NAME)
	local loginStr = "Welcome to " .. serverName .. "!"
	if player:getLastLoginSaved() <= 0 then
		loginStr = loginStr .. " Please choose your outfit."
		player:sendOutfitWindow()
	else
		if loginStr ~= "" then
			player:sendTextMessage(MESSAGE_STATUS_DEFAULT, loginStr)
		end

		loginStr = string.format("Your last visit in %s: %s.", serverName, os.date("%d %b %Y %X", player:getLastLoginSaved()))
	end
	player:sendTextMessage(MESSAGE_STATUS_DEFAULT, loginStr)

	-- Promotion
	local vocation = player:getVocation()
	local promotion = vocation:getPromotion()
	if player:isPremium() then
		local value = player:getStorageValue(PlayerStorageKeys.promotion)
		if value == 1 then
			player:setVocation(promotion)
		end
	elseif not promotion then
		player:setVocation(vocation:getDemotion())
	end
	
	-- Notify player about the rewards in their reward chest
	local rewardChest = player:getRewardChest()
	local rewardContainerCount = 0
	for _, item in ipairs(rewardChest:getItems()) do
		if item:getId() == ITEM_REWARD_CONTAINER then
			rewardContainerCount = rewardContainerCount + 1
		end
	end
	if rewardContainerCount > 0 then
		player:sendTextMessage(MESSAGE_STATUS_DEFAULT, string.format("You have %d reward%s in your reward chest.", rewardContainerCount, rewardContainerCount > 1 and "s" or ""))
	end

	-- Events
	player:registerEvent("PlayerDeath")
	player:registerEvent("DropLoot")
	player:registerEvent("Shop")
	return true
end
