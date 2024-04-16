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

function Creature:onAttack(target, origin, hitchance)
	local onAttack = EventCallback.onAttack
	if onAttack then
		return onAttack(self, target, origin, hitchance)
	end
	if hitChance == -1 then return 1 end
end

function Creature:onMissedAttack(target, combatType)
	local onMissedAttack = EventCallback.onMissedAttack
	if onMissedAttack then
		onMissedAttack(self, target, combatType)
	end
end

function Creature:onBlockedAttack(target, origin, combatType)
	local onBlockedAttack = EventCallback.onBlockedAttack
	if onBlockedAttack then
		onBlockedAttack(self, target, origin, combatType)
	end
end