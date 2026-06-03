local combat = Combat(MonsterCombats.RemorselessWave)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, pos in ipairs(combat:getPositions(creature, variant)) do
		local tile = Tile(pos)
		if tile then
			local top = tile:getTopCreature()
			if top and top:isPlayer() then
				top:addHealth(-math.random(0, 600))
			elseif top and top:isMonster() and top:getName():lower() == "stolen soul" then
				top:addHealth(-math.random(700, 1500))
			end
		end
	end
	return true
end

spell:name("remorseless wave")
spell:words("###73")
spell:needDirection(true)
spell:isAggressive(false)
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
