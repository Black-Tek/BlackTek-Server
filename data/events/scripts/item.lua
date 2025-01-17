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
	if onRemoveImbue then
		onRemoveImbue(self, imbueType, decayRemoved)
	end
end

function Item:onAttack(player, creature, blockType, combatType)
	local onAttack = EventCallback.onAttack
	if onAttack then
		onAttack(self, player, creature, blockType, combatType)
	end
end

function Item:onDefend(player, creature, blockType, combatType)
	local onDefend = EventCallback.onDefend
	if onDefend then
		onDefend(self, player, creature, blockType, combatType)
	end
end

function Item:onAugment(augment)
	local onAugment = EventCallback.onItemAugment
	if onAugment then
		onAugment(self, augment)
	end
end

function Item:onRemoveAugment(augment)
	local onRemoveAugment = EventCallback.onRemoveItemAugment
	if onRemoveAugment then
		onRemoveAugment(self, augment)
	end
end