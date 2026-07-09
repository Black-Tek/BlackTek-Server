local combat = Combat(MonsterCombats.EmberBeastArea)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, pos in ipairs(combat:getPositions(creature, variant)) do
		local tile = Tile(pos)
		if tile then
			local top = tile:getTopCreature()
			if top and top:isMonster() and top:getName():lower() == "the count of the core" then
				top:addHealth(math.random(0, 1500))
			end
		end
	end
	return true
end

spell:name("ember beast area")
spell:words("###76")
spell:isSelfTarget(true)
spell:needLearn(true)
spell:register()
