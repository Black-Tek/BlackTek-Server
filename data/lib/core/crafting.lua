function capAll(str)
    local newStr = ""; wordSeparate = string.gmatch(str, "([^%s]+)")
    for v in wordSeparate do
        v = v:gsub("^%l", string.upper)
        if newStr ~= "" then
            newStr = newStr.." "..v
        else
            newStr = v
        end
    end
    return newStr
end

function Player:sendMainCraftWindow(config)
	local function buttonCallback(button, choice)
		if button.text == "    Select    " then
			self:sendVocCraftWindow(config, choice.id)
		end	
	end
	
	local window = ModalWindow {
		title = config.mainTitleMsg,
		message = config.mainMsg.."\n\n" 
	}
 
	window:addButton("    Select    ", buttonCallback)
	window:addButton("    Exit    ", buttonCallback)
 
    for i = 1, #config.system do
		window:addChoice(config.system[i].tiers)
    end
	window:setDefaultEnterButton("    Select    ")
	window:setDefaultEscapeButton("    Exit    ")
	window:sendToPlayer(self)
end

local skillsStages = {
    {
        minlevel = 10,
        maxlevel = 30,
        multiplier = 45
    }, {
        minlevel = 31,
        maxlevel = 60,
        multiplier = 35
    }, {
        minlevel = 61,
        maxlevel = 90,
        multiplier = 25
    }, {
        minlevel = 91,
        maxlevel = 110,
        multiplier = 15
    }, {
        minlevel = 111,
        maxlevel = 130,
        multiplier = 10
    },{
        minlevel = 131,
        multiplier = 5
    }
}


function Player:sendVocCraftWindow(config, lastChoice)
    local function buttonCallback(button, choice)	
		if button.text == "    Back    " then
			self:sendMainCraftWindow(config)
		end 
        if button.text == "    Recipe    " then
            local item = config.system[lastChoice].items[choice.id].item
            local description = config.system[lastChoice].items[choice.id].description
            local skillRequired = config.system[lastChoice].items[choice.id].skill_required
            local requiredStorage = config.system[lastChoice].items[choice.id].storage
            local storageStatus = self:getStorageValue(requiredStorage)
            
            local details = "In order to craft " .. item .. " used for: " .. description .. "\n\nYou must collect the following items.\n\nRequired Items:\n"
            
            for i = 1, #config.system[lastChoice].items[choice.id].reqItems do
                local reqItems = config.system[lastChoice].items[choice.id].reqItems[i].item
                local reqItemsCount = config.system[lastChoice].items[choice.id].reqItems[i].count
                local reqItemsOnPlayer = self:getItemCount(reqItems)
                details = details .. "\n- " .. capAll(getItemName(reqItems) .. " { " .. reqItemsOnPlayer .. "/" .. reqItemsCount .. " }")
            end
            
            if storageStatus > 0 then
                details = details .. "\n\nDiscovered Recipe: Yes"
            else
                details = details .. "\n\nDiscovered Recipe: No"
            end
            
            details = details .. "\nSkill Required: " .. skillRequired
        
            self:showTextDialog(item, details)
        end
        


		if button.text == "    Craft    " then
            local requiredSkill = config.system[lastChoice].items[choice.id].skill_required
            local playerSkill = self:getSkillLevel(SKILL_CRAFTING)
                if playerSkill < requiredSkill then
                    self:sendTextMessage(MESSAGE_INFO_DESCR, "You need a crafting skill of at least " .. requiredSkill .. " to craft this item.")
                return true
                end

            local requiredStorage = config.system[lastChoice].items[choice.id].storage
            local itemName = config.system[lastChoice].items[choice.id].item

        if self:getStorageValue(requiredStorage) <= 0 then
         self:sendTextMessage(MESSAGE_INFO_DESCR, "You haven't discovered the recipe to craft " .. itemName .. ". Learn it in order to unlock the recipe and craft it.")
         return true
        end

    
		local item = config.system[lastChoice].items[choice.id].item
			for i = 1, #config.system[lastChoice].items[choice.id].reqItems do
				if self:getItemCount(config.system[lastChoice].items[choice.id].reqItems[i].item) < config.system[lastChoice].items[choice.id].reqItems[i].count then
					self:say(config.needItems..config.system[lastChoice].items[choice.id].item, TALKTYPE_MONSTER_SAY)
					return false
				end
			end	
			for i = 1, #config.system[lastChoice].items[choice.id].reqItems do
				self:removeItem(config.system[lastChoice].items[choice.id].reqItems[i].item, config.system[lastChoice].items[choice.id].reqItems[i].count)
			end	
            local skillMultiplier = 1 
            local playerSkill = self:getSkillLevel(SKILL_CRAFTING)
        for _, stage in ipairs(skillsStages) do
            if playerSkill >= stage.minlevel then
                if not stage.maxlevel or playerSkill <= stage.maxlevel then
                    skillMultiplier = stage.multiplier
                    break
                end
            end
        end	
        
       
        local skillTries = math.floor(skillMultiplier)
        self:getPosition():sendMagicEffect(176)
        self:getPosition():sendMagicEffect(179)
        local function craftEvent()
            self:addSkillTries(SKILL_CRAFTING, skillTries)
            self:addItem(config.system[lastChoice].items[choice.id].itemID)
            self:sendTextMessage(MESSAGE_EVENT_ADVANCE, "You have crafted x1 ["..item.."].")
            
        end
        addEvent(craftEvent, 2800)
		end	
    end
 
    local window = ModalWindow {
        title = config.craftTitle..config.system[lastChoice].tiers, 
        message = config.craftMsg..config.system[lastChoice].tiers..".\n\n", 
    }

	window:addButton("    Back    ", buttonCallback)
	window:addButton("    Exit    ")
	window:addButton("    Recipe    ", buttonCallback)
	window:addButton("    Craft    ", buttonCallback)
    window:setDefaultEnterButton("    Craft ")
    window:setDefaultEscapeButton("    Exit    ")
 
    for i = 1, #config.system[lastChoice].items do
        window:addChoice(config.system[lastChoice].items[i].item)
    end
 
    window:sendToPlayer(self)
end
