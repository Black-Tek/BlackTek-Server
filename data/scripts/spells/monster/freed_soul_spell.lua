local combat = Combat(MonsterCombats.FreedSoulSpell)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, pos in ipairs(combat:getPositions(creature, variant)) do
		local tile = Tile(pos)
		if tile then
			local top = tile:getTopCreature()
			if top and top:isMonster() and top:getName():lower() == "the souldespoiler" then
				top:addHealth(-math.random(500, 2000))
			end
		end
	end
	return true
end

spell:name("freed soul spell")
spell:words("###69")
spell:needDirection(true)
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
