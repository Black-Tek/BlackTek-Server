function Combat:getPositions(creature, variant)
	self:execute(creature, variant)
	return self:getAreaPositions(creature, variant)
end

function Combat:getTargets(creature, variant)
	self:execute(creature, variant)

	local varType = variant:getType()
	if varType == VARIANT_NUMBER then
		local target = Creature(variant:getNumber())
		return target and {target} or {}
	elseif varType == VARIANT_STRING then
		local target = Player(variant:getString())
		return target and {target} or {}
	end

	local targets = {}
	for _, pos in ipairs(self:getAreaPositions(creature, variant)) do
		local tile = Tile(pos)
		if tile then
			for _, spec in ipairs(tile:getCreatures() or {}) do
				if spec ~= creature then
					targets[#targets + 1] = spec
				end
			end
		end
	end
	return targets
end
