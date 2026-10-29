local global_startup = GlobalEvent("global_startup_event")

global_startup.onStartup = function()

	db.query("TRUNCATE TABLE `players_online`")
	db.asyncQuery("DELETE FROM `guild_wars` WHERE `status` = 0")
	db.asyncQuery("DELETE FROM `players` WHERE `deletion` != 0 AND `deletion` < " .. os.time())
	db.asyncQuery("DELETE FROM `ip_bans` WHERE `expires_at` != 0 AND `expires_at` <= " .. os.time())
	db.asyncQuery("DELETE FROM `market_history` WHERE `inserted` <= " .. (os.time() - configManager.getNumber(configKeys.MARKET_OFFER_DURATION)))

	-- Move expired bans to ban history
	local resultId = db.storeQuery("SELECT * FROM `account_bans` WHERE `expires_at` != 0 AND `expires_at` <= " .. os.time())
	if resultId ~= false then
		repeat
			local accountId = result.getNumber(resultId, "account_id")
			db.asyncQuery("INSERT INTO `account_ban_history` (`account_id`, `reason`, `banned_at`, `expired_at`, `banned_by`) VALUES (" .. accountId .. ", " .. db.escapeString(result.getString(resultId, "reason")) .. ", " .. result.getNumber(resultId, "banned_at") .. ", " .. result.getNumber(resultId, "expires_at") .. ", " .. result.getNumber(resultId, "banned_by") .. ")")
			db.asyncQuery("DELETE FROM `account_bans` WHERE `account_id` = " .. accountId)
		until not result.next(resultId)
		result.free(resultId)
	end

	-- Check house auctions
	local auctionTime = os.time()
	local resultId = db.storeQuery("SELECT `id` FROM `houses` WHERE `owner` = 0 AND `bid_end` != 0 AND `bid_end` < " .. auctionTime)
	if resultId ~= false then
		repeat
			local house = House(result.getNumber(resultId, "id"))
			if house then
				local houseId = house:getId()
				-- Charge the current balance and award ownership in one statement.
				-- Multiple winning bids by one player cannot reuse an old balance.
				local paid = db.query("UPDATE `players` AS `p` INNER JOIN `houses` AS `h` ON `p`.`id` = `h`.`highest_bidder` SET `p`.`balance` = `p`.`balance` - `h`.`last_bid`, `h`.`owner` = `p`.`id` WHERE `h`.`id` = " .. houseId .. " AND `h`.`owner` = 0 AND `h`.`bid_end` != 0 AND `h`.`bid_end` < " .. auctionTime .. " AND `p`.`balance` >= `h`.`last_bid`")
				if paid then
					local ownerResult = db.storeQuery("SELECT `owner` FROM `houses` WHERE `id` = " .. houseId)
					if ownerResult then
						local owner = result.getNumber(ownerResult, "owner")
						result.free(ownerResult)
						if owner ~= 0 then house:setOwnerGuid(owner, false) end
						db.query("UPDATE `houses` SET `last_bid` = 0, `bid_end` = 0, `highest_bidder` = 0, `bid` = 0 WHERE `id` = " .. houseId .. " AND `bid_end` != 0 AND `bid_end` < " .. auctionTime)
					end
				end
			end
		until not result.next(resultId)
		result.free(resultId)
	end

	-- store towns in database
	db.query("TRUNCATE TABLE `towns`")
	for i, town in ipairs(Game.getTowns()) do
		local position = town:getTemplePosition()
		db.query("INSERT INTO `towns` (`id`, `name`, `posx`, `posy`, `posz`) VALUES (" .. town:getId() .. ", " .. db.escapeString(town:getName()) .. ", " .. position.x .. ", " .. position.y .. ", " .. position.z .. ")")
	end
	
	-- check for duplicate storages
	if configManager.getBoolean(configKeys.CHECK_DUPLICATE_STORAGE_KEYS) then
		local variableNames = {"AccountStorageKeys", "PlayerStorageKeys", "GlobalStorageKeys", "actionIds", "uniqueIds"}
		for _, variableName in ipairs(variableNames) do
			local duplicates = checkDuplicateStorageKeys(variableName)
			if duplicates then
				local message = "Duplicate keys found: " .. table.concat(duplicates, ", ")
				print(">> Checking " .. variableName .. ": " .. message)
			else
				print(">> Checking " .. variableName .. ": No duplicate keys found.")
			end
		end
	end
end

global_startup:type("startup")
global_startup:register()
