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

--- Offensive ---
function Item:onAttack(player, creature, combatType)
	local onAttack = EventCallback.onAttack
	if EventCallback.onAttack then
		onAttack(self, player, creature, combatType)
	end
end

function Item:onHit(player, creature, combatType)
	local onHit = EventCallback.onHit
	if EventCallback.onHit then
		onHit(self, player, creature, combatType)
	end
end

function Item:onBlocked(player, creature, combatType)
	local onBlocked = EventCallback.onBlocked
	if EventCallback.onBlocked then
		onBlocked(self, player, creature, combatType)
	end
end

--- Defensive ---
function Item:onDefendAttack(player, creature, combatType)
	local onDefendAttack = EventCallback.onDefendAttack
	if EventCallback.onDefendAttack then
		onDefendAttack(self, player, creature, combatType)
	end
end

function Item:onBlockAttack(player, creature, combatType)
	local onBlockAttack = EventCallback.onBlockAttack
	if EventCallback.onBlockAttack then
		onBlockAttack(self, player, creature, combatType)
	end
end