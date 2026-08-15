local weaponDestruction = ItemEvent()

weaponDestruction.onUse = function(player, item, fromPosition, target, toPosition, isHotkey)
	return destroyItem(player, target, toPosition)
end

for id = 2376, 2404 do weaponDestruction:id(id) end
for id = 2406, 2415 do weaponDestruction:id(id) end
for id = 2417, 2419 do weaponDestruction:id(id) end
for id = 2421, 2441 do weaponDestruction:id(id) end
for id = 2443, 2453 do weaponDestruction:id(id) end
weaponDestruction:register()
