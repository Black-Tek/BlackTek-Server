--- Imbuements ---
function Item:onImbue(imbuement, firstCreation)
	local onImbue = EventCallback.onImbue
	if onImbue and firstCreation then
		onImbue(self, imbuement, firstCreation)
	end
	return true
end

function Item:onRemoveImbue(imbueType, decayRemoved)
	local onRemoveImbue = EventCallback.onRemoveImbue
	if EventCallback.onRemoveImbue then
		onRemoveImbue(self, imbueType, decayRemoved)
	end
end

function Item:onAttack(player, creature, blockType, combatType)
	local onAttack = EventCallback.onAttack
	if EventCallback.onAttack then
		onAttack(self, player, creature, blockType, combatType)
	end
end

function Item:onDefend(player, creature, blockType, combatType)
	local onDefend = EventCallback.onDefend
	if EventCallback.onDefend then
		onDefend(self, player, creature, blockType, combatType)
	end
end