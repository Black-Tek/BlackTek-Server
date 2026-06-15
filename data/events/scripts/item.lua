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

function Item:onModifierAttack(player, creature, blockType, combatType)
	local onModifierAttack = EventCallback.onModifierAttack
	if onModifierAttack then
		onModifierAttack(self, player, creature, blockType, combatType)
	end
end

function Item:onModifierDefend(player, creature, blockType, combatType)
	local onModifierDefend = EventCallback.onModifierDefend
	if onModifierDefend then
		onModifierDefend(self, player, creature, blockType, combatType)
	end
end