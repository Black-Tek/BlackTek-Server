local rootsTable = {
	[23475] = 23477,
	[23476] = 23478
}

local juicyRoot = ItemEvent()

juicyRoot.onUse = function(player, item, fromPosition, target, toPosition, isHotkey)
	local roots = rootsTable[item.itemid]
	if not roots then
		player:sendTextMessage(MESSAGE_EVENT_ADVANCE, "This root has already been harvested, nothing to gain here.")
		return true
	end
	if math.random(2) == 1 then
		player:sendTextMessage(MESSAGE_EVENT_ADVANCE, "You successfully harvest some juicy roots.")
		player:addItem(23662, 1) -- juicy roots
	else
		player:sendTextMessage(MESSAGE_EVENT_ADVANCE, "Your harvesting attempt destroyed more of the juicy roots than you could salvage.")
	end
	item:transform(roots)
	item:getPosition():sendMagicEffect(CONST_ME_GREEN_RINGS)
	item:decay()
	return true
end

for k, v in pairs(rootsTable) do
	juicyRoot:id(k)
end
juicyRoot:id(23477, 23478)
juicyRoot:register()
