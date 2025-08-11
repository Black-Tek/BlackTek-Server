function Creature:onChangeOutfit(outfit)
	local onChangeMount = EventCallback.onChangeMount
	if onChangeMount then
		if not onChangeMount(self, outfit.lookMount) then
			return false
		end
	end
	local onChangeOutfit = EventCallback.onChangeOutfit
	if onChangeOutfit then
		return onChangeOutfit(self, outfit)
	end
	return true
end

function Creature:onAreaCombat(tile, isAggressive)
	local onAreaCombat = EventCallback.onAreaCombat
	if onAreaCombat then
		return onAreaCombat(self, tile, isAggressive)
	end
	return RETURNVALUE_NOERROR
end

function Creature:onTargetCombat(target)
	local onTargetCombat = EventCallback.onTargetCombat
	if onTargetCombat then
		return onTargetCombat(self, target)
	end
	return RETURNVALUE_NOERROR
end

function Creature:onHear(speaker, words, type)
	local onHear = EventCallback.onHeard
	if onHear then
		onHear(self, speaker, words, type)
	end
end

function Creature:onAttack(target, blockType, combatType, origin, criticalDamage, leechedDamage)
	local onAttack = EventCallback.onDoAttack
	if onAttack then
		onAttack(self, target, blockType, combatType, origin, criticalDamage, leechedDamage)
	end
end

function Creature:onDefend(attacker, blockType, combatType, origin, criticalDamage, leechedDamage)
	local onDefend = EventCallback.onDefendAttack
	if onDefend then
		onDefend(self, attacker, blockType, combatType, origin, criticalDamage, leechedDamage)
	end
end