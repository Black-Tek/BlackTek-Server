local worldClock = ItemEvent()

worldClock.onUse = function(player, item, fromPosition, target, toPosition, isHotkey)
	player:sendTextMessage(MESSAGE_INFO_DESCR, "The time is " .. getFormattedWorldTime() .. ".")
	return true
end

for id = 1728, 1731 do worldClock:id(id) end
worldClock:id(2036)
worldClock:id(3900)
worldClock:id(6092)
worldClock:id(7828)
for id = 9443, 9444 do worldClock:id(id) end
worldClock:register()
