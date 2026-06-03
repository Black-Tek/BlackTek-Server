local combat = Combat(MonsterCombats.AggressiveLavaBomb)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, pos in ipairs(combat:getPositions(creature, variant)) do
		local tile = Tile(pos)
		if tile then
			local top = tile:getTopCreature()
			if top and top:isMonster() and top:getName():lower() == "the duke of the depths" then
				top:addHealth(math.random(0, 2000))
			end
		end
	end
	return true
end

spell:name("aggresive lava bomb")
spell:words("###75")
spell:isSelfTarget(true)
spell:needLearn(true)
spell:register()
