local ec = EventCallback

ec.onLook = function(self, thing, position, distance, description)
	local description = "You see " .. thing:getDescription(distance)
	if self:getGroup():getAccess() then
		if thing:isItem() then
			description = string.format("%s\nItem ID: %d", description, thing:getId())

			local actionId = thing:getActionId()
			if actionId ~= 0 then
				description = string.format("%s, Action ID: %d", description, actionId)
			end

			local uniqueId = thing:getAttribute(ITEM_ATTRIBUTE_UNIQUEID)
			if uniqueId > 0 and uniqueId < 65536 then
				description = string.format("%s, Unique ID: %d", description, uniqueId)
			end

			local itemType = thing:getType()

			local transformEquipId = itemType:getTransformEquipId()
			local transformDeEquipId = itemType:getTransformDeEquipId()
			if transformEquipId ~= 0 then
				description = string.format("%s\nTransforms to: %d (onEquip)", description, transformEquipId)
			elseif transformDeEquipId ~= 0 then
				description = string.format("%s\nTransforms to: %d (onDeEquip)", description, transformDeEquipId)
			end

			local decayId = itemType:getDecayId()
			if decayId ~= -1 then
				description = string.format("%s\nDecays to: %d", description, decayId)
			end
		elseif thing:isCreature() then
			local str = "%s\nHealth: %d / %d"
			if thing:isPlayer() and thing:getMaxMana() > 0 then
				str = string.format("%s, Mana: %d / %d", str, thing:getMana(), thing:getMaxMana())
			end
			description = string.format(str, description, thing:getHealth(), thing:getMaxHealth()) .. "."
		end

		local position = thing:getPosition()
		description = string.format(
			"%s\nPosition: %d, %d, %d",
			description, position.x, position.y, position.z
		)

		if thing:isCreature() then
			if thing:isPlayer() then
				description = string.format("%s\nIP: %s.", description, Game.convertIpToString(thing:getIp()))
			end
		end
	end

	if thing:isItem() then
		local totalSlots = thing:getImbuementSlots()  -- Get the total number of imbument slots.

		if totalSlots > 0 then  -- Proceed only if slots are available.
			local occupiedSlotsDescriptions = {}
			local emptySlotsDescriptions = {}

			-- Fetch current imbuments on the item.
			local imbuments = thing:getImbuements()

			for slot = 1, totalSlots do
				local imbue = imbuments[slot]
				if imbue and imbue.getType then
					local imbueId = imbue:getType()
					local imbueDuration = imbue:getDuration()
					local imbueValue = imbue:getValue()
					
					-- Determine imbue type and name.
					
					local imbueName, imbueLevelName

					for key, value in pairs(imbuements) do
						if value.id == imbueId then
							imbueName = value.name
							
							for levelName, levelData in pairs(value.levels) do
								if levelData.value == imbueValue then
									imbueLevelName = levelName
									break
								end
							end
							break
						end
					end

					-- Format duration output.
					local hours = math.floor(imbueDuration / 3600)
					local remainingMinutes = math.floor((imbueDuration % 3600) / 60)
					local formattedOutput

					if imbueDuration < 60 then
						formattedOutput = "less than a minute"
					elseif imbueDuration < 3600 then
						formattedOutput = string.format("%d min", remainingMinutes)
					else
						formattedOutput = string.format("%d:%02dh", hours, remainingMinutes)
					end
					
					if imbueLevelName and imbueName and formattedOutput then
						table.insert(occupiedSlotsDescriptions, imbueLevelName .. " " .. imbueName .. " " .. formattedOutput)
					else
						table.insert(occupiedSlotsDescriptions, "Unknown imbuement")
					end
				else
					table.insert(emptySlotsDescriptions, "Empty Slot")
				end
			end

			-- Combine descriptions of occupied and empty slots.
			local slotsDescriptions = {}
			for _, desc in ipairs(occupiedSlotsDescriptions) do
				table.insert(slotsDescriptions, desc)
			end
			for _, desc in ipairs(emptySlotsDescriptions) do
				table.insert(slotsDescriptions, desc)
			end

			-- Format the final description of imbuements.
			if #slotsDescriptions > 0 then
				local imbuementsDescription = "Imbuements: (" .. table.concat(slotsDescriptions, ", ") .. ")."
				description = description and description .. "\n" .. imbuementsDescription or imbuementsDescription
			end
		end

		if thing:isAugmented() then
			for index, augment in pairs(thing:getAugments()) do
				augDesc = augment:getDescription()
				description = description.. " \n Augment : " .. augment:getName() .. " \n " .. augDesc
			end
		end

	end

	return description
end

ec:register()
