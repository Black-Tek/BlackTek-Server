-- Configuration settings for the imbuing process
local config = {
    shrineId = 24184,              -- ID of the shrine
    shrineAId = 35531,             -- ActionID of the shrine
    removeCost = 15000,            -- Cost to remove an imbuement
    basicImbueCost = 15000,        -- Cost for basic imbuement
    intricateImbueCost = 55000,    -- Cost for intricate imbuement
    powerfulImbueCost = 150000     -- Cost for powerful imbuement
}

-- Initialization of variables to keep track of slots, selected imbuement names, and choices
local slots = {}
local selectedImbueName = {}
local choiceMap = {}

-- Function to retrieve all items within a container
local function getAllItemsInContainer(container)
    local items = {}
    local function getItems(container)
        if not container:isContainer() then
            return
        end
        for i = 0, container:getSize() - 1 do
            local item = container:getItem(i)
            if item then
                table.insert(items, item)
                if item:isContainer() then
                    getItems(item)
                end
            end
        end
    end
    getItems(container)
    return items
end

-- Function to update the player's special skills based on equipped imbuements
local function addChanceToImbuement(player, item, equipped, imbueId)

local imbuementEffects = {
    [IMBUEMENT_TYPE_CRITICAL_AMOUNT] = {skill = SPECIALSKILL_CRITICALHITCHANCE, value = 10},
    [IMBUEMENT_TYPE_LIFE_LEECH] = {skill = SPECIALSKILL_LIFELEECHCHANCE, value = 100},
    [IMBUEMENT_TYPE_MANA_LEECH] = {skill = SPECIALSKILL_MANALEECHCHANCE, value = 100}
}

    if not item:isItem() or not item:hasImbuements() then
        return false
    end
	
	if imbueId then
		for imbuementType, effect in pairs(imbuementEffects) do
			if imbueId == imbuementType and item:hasImbuementType(imbuementType) then
				local adjustment = equipped and effect.value or -effect.value
				player:addSpecialSkill(effect.skill, adjustment)
				return true
			end
		end
	end
	
	for imbuementType, effect in pairs(imbuementEffects) do
		if item:hasImbuementType(imbuementType) then
			local adjustment = equipped and effect.value or -effect.value
			player:addSpecialSkill(effect.skill, adjustment)
		end
	end
	
	return true
end

-- Function to get the imbuement item currently being used by the player
local function getImbueItem(player)
    local items = {}
    
    for slot = CONST_SLOT_HEAD, CONST_SLOT_AMMO do
        local item = player:getSlotItem(slot)
        if item then
            table.insert(items, item)
            if item:isContainer() then
                local containerItems = getAllItemsInContainer(item)
                for _, subItem in ipairs(containerItems) do
                    table.insert(items, subItem)
                end
            end
        end
    end

    for _, item in ipairs(items) do
        if item:getCustomAttribute("activeImbueItem") == true then
            return item
        end
    end
    return nil
end

-- Function to check the imbuement items equipped by the player
local function imbueItemCheck(player)
    local items = {}
    local activeItemCount = 0

    for slot = CONST_SLOT_HEAD, CONST_SLOT_AMMO do
        local item = player:getSlotItem(slot)
        if item then
            table.insert(items, item)
            if item:isContainer() then
                local containerItems = getAllItemsInContainer(item)
                for _, subItem in ipairs(containerItems) do
                    table.insert(items, subItem)
                end
            end
        end
    end

    for _, item in ipairs(items) do
        if item:getCustomAttribute("activeImbueItem") == true then
            activeItemCount = activeItemCount + 1
        end
    end

    return activeItemCount
end

-- Function to reset the active imbuement items for a player
local function resetActiveImbueItem(player)
    local items = {}

    for slot = CONST_SLOT_HEAD, CONST_SLOT_AMMO do
        local item = player:getSlotItem(slot)
        if item then
            table.insert(items, item)
            if item:isContainer() then
                local containerItems = getAllItemsInContainer(item)
                for _, subItem in ipairs(containerItems) do
                    table.insert(items, subItem)
                end
            end
        end
    end

    for _, item in ipairs(items) do
        if item:getCustomAttribute("activeImbueItem") then
            item:setCustomAttribute("activeImbueItem", false)
        end
    end
end

-- Function to create a message detailing required items and cost for an imbuement
local function createImbuementMessage(player, items, cost)
    local messageParts = {"Required items for the imbuement:\n\n" .. cost .. " gold\n"}
    for _, item in ipairs(items) do
        local playerItemCountStr = tostring(player:getItemCount(ItemType(item.itemId):getId()))
        local playerItemCount = tonumber(playerItemCountStr:match("^(%d+)"))
        local itemName = item.count > 1 and ItemType(item.itemId):getPluralName() or ItemType(item.itemId):getName()
        table.insert(messageParts, playerItemCount .. "/" .. item.count .. " " .. itemName .. "\n")
    end
    table.insert(messageParts, "\nDo you want to proceed with the imbuement?")
    return table.concat(messageParts)
end

-- Function to get the type name by its choice index from the map
local function getTypeNameByChoiceIndex(choiceIndex)
    return choiceMap[choiceIndex] or "Unknown"
end

-- Function to retrieve the slot type for a given player ID and slot number
local function getSlotTypeForPlayer(playerId, slotNumber)
    local slotsData = slots[playerId]
    if not slotsData then
        return "Empty Slot"
    end
    return slotsData[slotNumber] or "Empty Slot"
end

-- Function to check if item is equipped or not
local function isEquipped(item)
	if not item:isItem() then
        return false
    end
	local parent = item:getParent()
	if parent:isPlayer() then
		return true
	end
	return false
end

-- Function to create and display the initial imbuing window for a player
local function createFirstSelectSlotWindow(playerId)
    local player = Player(playerId)
    if not player then
        return false 
    end

    local usedItem = getImbueItem(player)
    local windowTitle = usedItem:getName():sub(1, 1):upper() .. usedItem:getName():sub(2)
    local window = ModalWindow(1000, "Imbuing", windowTitle .. "\n\nSelect your imbuements slot:")
    local totalImbueSlots = usedItem:getImbuementSlots() or 0
    local itemImbuements = usedItem:getImbuements()
	
	player:registerEvent("SelectImbueTypeOrRemoveWindow")

    local slotsData = {}
    for slot = 1, totalImbueSlots do
        local status = "Empty Slot"
        local imbue = itemImbuements[slot]

        if imbue and imbue.getType then
            local imbueId = imbue:getType()
            local imbueValue = imbue:getValue()
            local imbueDuration = imbue:getDuration()

            local imbueName, imbueLevelName = "Unknown", "Unknown"

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

            local hours = math.floor(imbueDuration / 3600)
            local remainingMinutes = math.floor((imbueDuration % 3600) / 60)
            local formattedOutput = string.format("%d:%02dh", hours, remainingMinutes)
            
            status = string.format("%s %s, %s", imbueLevelName, imbueName, formattedOutput)
        end
        
        slotsData[slot] = status
        window:addChoice(slot, status)
    end

    slots[playerId] = slotsData

    window:addButton(1, "Select")
    window:setDefaultEnterButton(1)
    window:addButton(2, "Cancel")
    window:setDefaultEscapeButton(2)
	
    window:sendToPlayer(player)

    return true
end

-- Function to check if a given itemid exists in the imbueItems list, ignoring case
local function isItemInList(itemid)
    local lowerItemid = string.lower(itemid)
    for _, item in ipairs(imbueItems) do
        if string.lower(item.id) == lowerItemid then
            return true
        end
    end
    return false
end

-- Initialize an Action to handle item use events specific to imbuing
local useItem = Action()

-- Defines the action to be taken when an item is used
function useItem.onUse(player, item, fromPosition, target, toPosition, isHotkey)
    if item.actionid ~= config.shrineAId then
        return false
    end
    if isItemInList(target:getName()) then
        resetActiveImbueItem(player)
        local topLevelPlayer = Player(target:getTopParent())
        if not topLevelPlayer or topLevelPlayer:getId() ~= player:getId() then
            player:sendTextMessage(MESSAGE_STATUS_SMALL, "The item must be in your inventory.")
            player:getPosition():sendMagicEffect(CONST_ME_POFF)
            return true
        end
        
        target:setCustomAttribute("activeImbueItem", true)
        return createFirstSelectSlotWindow(player:getId())
    else
        player:sendTextMessage(MESSAGE_STATUS_SMALL, "This item does not have any imbuement slots.")
    end        
    return true
end

useItem:id(config.shrineId)
useItem:register()

-- Handler for the second window where u select imbuement type or removal of imbuement
local SelectImbueTypeOrRemoveWindow = CreatureEvent("SelectImbueTypeOrRemoveWindow")
function SelectImbueTypeOrRemoveWindow.onModalWindow(player, modalWindowId, buttonId, choiceId)
    player:unregisterEvent("SelectImbueTypeOrRemoveWindow")

    local usedItem = getImbueItem(player)
    
    if imbueItemCheck(player) ~= 1 or not usedItem then
        player:getPosition():sendMagicEffect(CONST_ME_POFF, player)
        return true
    end
    
    local imbuementName = getSlotTypeForPlayer(player:getId(), choiceId)
    local imbuementId = nil
    
    local searchName = imbuementName:match("^(.+),")
    
    for imbueKey, imbueData in pairs(imbuements) do
        for levelKey, levelData in pairs(imbueData.levels) do
            local fullName = levelKey .. " " .. imbueData.name
            if fullName == searchName then
                imbuementId = imbueData.id
            end
        end
    end
        
    if imbuementName ~= "Empty Slot" then
        if modalWindowId == 1000 and buttonId == 1 then
            local window = ModalWindow {title = "Imbuing", message = "You need to remove your current imbuement before you can select a new one.\n\nDo you want to remove it for " .. config.removeCost .. " gold?"}
        
        window:addButton(' Yes ',
            function(button, choice)
                if imbueItemCheck(player) ~= 1 or not usedItem then
                    player:getPosition():sendMagicEffect(CONST_ME_POFF, player)
                    return true
                end
                
                if player:getMoney() < config.removeCost then
                    player:getPosition():sendMagicEffect(CONST_ME_POFF, player)
                    player:sendTextMessage(MESSAGE_STATUS_SMALL, "You don't have enough money.")
                    return true
                else
                    
                    for k,v in pairs (usedItem:getImbuements()) do
                        if v:getType() == imbuementId then
							if isEquipped(usedItem) then
								addChanceToImbuement(player, usedItem, false, imbuementId)
							end
							player:getPosition():sendMagicEffect(CONST_ME_MAGIC_RED)
                            usedItem:removeImbuement(v)	
                            player:removeMoney(config.removeCost)
                            player:sendTextMessage(MESSAGE_STATUS_SMALL, "Imbuement succesfully removed.")
                        end
                    end
                    
                end
            end
        )
        window:setDefaultEnterButton(' Yes ')
        window:addButton('  No  ')
        window:setDefaultEscapeButton('  No  ')
        window:sendToPlayer(player)
        end
    else
        if modalWindowId == 1000 and buttonId == 1 then
            player:registerEvent("SelectPowerOfImbuementWindow")
            local window = ModalWindow(1001, "Imbuing", "Select your imbuement:")
            
            local totalSlots = usedItem:getImbuementSlots() or 0
            local itemImbuements = usedItem:getImbuements()
            local activeImbuementIDs = {}

            for slot = 1, totalSlots do
                if itemImbuements and itemImbuements[slot] and itemImbuements[slot].getType then
                    local imbueId = itemImbuements[slot]:getType()
                    activeImbuementIDs[imbueId] = true
                end
            end

            for _, obj in ipairs(imbueItems) do
                if getItemIdByName(obj.id) == usedItem.itemid then
                    for choiceIndex, typeName in ipairs(obj.types) do
                        local imbueData = imbuements[typeName]
                        if imbueData and not activeImbuementIDs[imbueData.id] then
                            window:addChoice(choiceIndex, typeName)
                            choiceMap[choiceIndex] = typeName
                        end
                    end
                end
            end

            window:addButton(1, "Select")
            window:setDefaultEnterButton(1)
            window:addButton(2, "Cancel")
            window:setDefaultEscapeButton(2)
            window:sendToPlayer(player)
        end
    end
    return true
end
SelectImbueTypeOrRemoveWindow:register()

-- Handler for the selection of basic, intricate or powerful imbuement
local SelectPowerOfImbuementWindow = CreatureEvent("SelectPowerOfImbuementWindow")
function SelectPowerOfImbuementWindow.onModalWindow(player, modalWindowId, buttonId, choiceId)
	player:unregisterEvent("SelectPowerOfImbuementWindow")
	player:registerEvent("ImbuementApplicationWindow")
		
	local playerId = player:getId()
	local imbuementName = getTypeNameByChoiceIndex(choiceId)
	selectedImbueName[playerId] = imbuementName
	
    if modalWindowId == 1001 and buttonId == 1 then
        local window = ModalWindow(1002, "Imbuing", "All imbuements have 100% success rate.\nSelect your imbuement:")
        window:addChoice(1, "Basic " .. imbuements[imbuementName].name .. ": " .. config.basicImbueCost .. " gold")
        window:addChoice(2, "Intricate " .. imbuements[imbuementName].name .. ": " .. config.intricateImbueCost .. " gold")
        window:addChoice(3, "Powerful " .. imbuements[imbuementName].name .. ": " .. config.powerfulImbueCost .. " gold")
        
        window:addButton(1, "Select")
        window:setDefaultEnterButton(1)
        window:addButton(2, "Cancel")
        window:setDefaultEscapeButton(2)
        
        window:sendToPlayer(player)
    end
    return true
end
SelectPowerOfImbuementWindow:register()

-- Handler for the last window where u select yes or no 
local ImbuementApplicationWindow = CreatureEvent("ImbuementApplicationWindow")
function ImbuementApplicationWindow.onModalWindow(player, modalWindowId, buttonId, choiceId)
	player:unregisterEvent("ImbuementApplicationWindow")
	
	local playerId = player:getId()
	local imbuementName = selectedImbueName[playerId]
	local usedItem = getImbueItem(player)
	local title = nil
	local message = nil
	local itemsToImbue = nil
	local cost = 0
	local level = nil
	
	if modalWindowId == 1002 and buttonId == 1 then
		if choiceId == 1 then -- basic
		itemsToImbue = imbuements[imbuementName].levels.Basic.items
			title = "Basic " .. imbuements[imbuementName].name
			message = createImbuementMessage(player, itemsToImbue, config.basicImbueCost)
			cost = config.basicImbueCost
			level = "Basic"
		elseif choiceId == 2 then -- Intricate
		itemsToImbue = imbuements[imbuementName].levels.Intricate.items
			title = "Intricate " .. imbuements[imbuementName].name
			message = createImbuementMessage(player, itemsToImbue, config.intricateImbueCost)
			cost = config.intricateImbueCost
			level = "Intricate"
		elseif choiceId == 3 then -- Powerful
		itemsToImbue = imbuements[imbuementName].levels.Powerful.items
			title = "Powerful " .. imbuements[imbuementName].name
			message = createImbuementMessage(player, itemsToImbue, config.powerfulImbueCost)
			cost = config.powerfulImbueCost
			level = "Powerful"
		end
		
        local window = ModalWindow(title, message)
        local window = ModalWindow {title = title, message = message}

		
		    local imbuementData = imbuements[imbuementName]
			if not imbuementData then
				print("Imbuement not found.")
				return false
			end

			local levelData = imbuementData.levels[level]
			if not levelData then
				print("Imbuement level not found.")
				return false
			end

		window:addButton(' Yes ',
			function(button, choice)
			
				--Check if item is valid
				if imbueItemCheck(player) ~= 1 or not usedItem then
					player:getPosition():sendMagicEffect(CONST_ME_POFF, player)
					return true
				end
				
			    -- First, check if the player has all required items
				for _, item in ipairs(levelData.items) do
					local itemCount = player:getItemCount(item.itemId)  -- Check how many of the item the player has
					if itemCount < item.count then  -- Not enough items
						player:getPosition():sendMagicEffect(CONST_ME_POFF)
						player:sendTextMessage(MESSAGE_STATUS_SMALL, "You do not have the required items for the imbuement.")
						return false  -- Early exit if any item is insufficient
					end
				end
				
				-- Check and remove money
				if player:getMoney() >= cost then
					player:removeMoney(cost)
				else
					player:getPosition():sendMagicEffect(CONST_ME_POFF, player)
					player:sendTextMessage(MESSAGE_STATUS_SMALL, "You don't have enough money.")
					return false
				end

				-- If the player has all items, proceed to remove them
				for _, item in ipairs(levelData.items) do
					player:removeItem(item.itemId, item.count)
				end
				
				player:getPosition():sendMagicEffect(CONST_ME_MAGIC_RED)
				local imbue = Imbuement(imbuementData.id, levelData.value, imbuementData.duration, imbuementData.decaytype)
				usedItem:addImbuement(imbue)
				
				if isEquipped(usedItem) then
					addChanceToImbuement(player, usedItem, true, imbuementData.id)
				end
				
				player:sendTextMessage(MESSAGE_STATUS_SMALL, "The item has been successfully imbued.")
				
			end
		)
		window:setDefaultEnterButton(' Yes ')
		window:addButton('  No  ')
		window:setDefaultEscapeButton('  No  ')
        
        window:sendToPlayer(player)
    end
    return true
end
ImbuementApplicationWindow:register()

-- Initialize the event callback
local ec = EventCallback

-- Function to handle inventory updates and apply imbuement effects
function ec.onInventoryUpdate(player, item, slot, equip)
	addChanceToImbuement(player, item, equip)
    return true
end

-- Register the event callback
ec:register()
