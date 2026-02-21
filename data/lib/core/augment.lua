local ATTACK_MOD = 1
local DEFENSE_MOD = 2

local attackModifierNames = {
	[ATTACK_MODIFIER_NONE] = "none",
	[ATTACK_MODIFIER_LIFESTEAL] = "lifesteal",
	[ATTACK_MODIFIER_MANASTEAL] = "manasteal",
	[ATTACK_MODIFIER_STAMINASTEAL] = "staminasteal",
	[ATTACK_MODIFIER_SOULSTEAL] = "soulsteal",
	[ATTACK_MODIFIER_CRITICAL] = "critical hit",
	[ATTACK_MODIFIER_PIERCING] = "piercing",
	[ATTACK_MODIFIER_CONVERSION] = "damage conversion"
}

local defenseModifierNames = {
	[DEFENSE_MODIFIER_NONE] = "none",
	[DEFENSE_MODIFIER_ABSORB] = "absorb",
	[DEFENSE_MODIFIER_RESTORE] = "mana restore",
	[DEFENSE_MODIFIER_REPLENISH] = "stamina replenish",
	[DEFENSE_MODIFIER_REVIVE] = "soul revive",
	[DEFENSE_MODIFIER_REFLECT] = "reflect",
	[DEFENSE_MODIFIER_DEFLECT] = "deflect",
	[DEFENSE_MODIFIER_RICOCHET] = "ricochet",
	[DEFENSE_MODIFIER_RESIST] = "resistance",
	[DEFENSE_MODIFIER_REFORM] = "damage reform",
	[DEFENSE_MODIFIER_WEAKNESS] = "weakness"
}

local combatTypeNames = {
	[COMBAT_NONE] = "all",
	[COMBAT_PHYSICALDAMAGE] = "physical",
	[COMBAT_ENERGYDAMAGE] = "energy",
	[COMBAT_EARTHDAMAGE] = "earth",
	[COMBAT_FIREDAMAGE] = "fire",
	[COMBAT_UNDEFINEDDAMAGE] = "undefined",
	[COMBAT_LIFEDRAIN] = "lifedrain",
	[COMBAT_MANADRAIN] = "manadrain",
	[COMBAT_HEALING] = "healing",
	[COMBAT_DROWNDAMAGE] = "drown",
	[COMBAT_ICEDAMAGE] = "ice",
	[COMBAT_HOLYDAMAGE] = "holy",
	[COMBAT_DEATHDAMAGE] = "death"
}

local originTypeNames = {
	[ORIGIN_NONE] = "all",
	[ORIGIN_CONDITION] = "conditions",
	[ORIGIN_SPELL] = "spells",
	[ORIGIN_MELEE] = "melee",
	[ORIGIN_RANGED] = "ranged"
}

local function getModifierGroupKey(modifier)
	local stance = modifier:getStance()
	local modType = modifier:getType()
	local value = modifier:getValue()
	local chance = modifier:getChance()
	local isPercent = modifier:isPercent()
	local originType = modifier:getOrigin()
	local creatureName = modifier:getCreatureName() or "none"
	local conversionType = 0
	if modType == ATTACK_MODIFIER_CONVERSION or modType == DEFENSE_MODIFIER_REFORM then
		conversionType = modifier:getConversion() or 0
	end
	return string.format(
		"%d:%d:%d:%d:%s:%d:%s:%d",
		stance,
		modType,
		value,
		chance and chance or 0,
		isPercent and "p" or "f",
		originType,
		creatureName,
		conversionType
	)
end

local function buildGroupedModifierLines(modifiers, stance)
	local groups = {}
	local groupOrder = {}

	for _, modifier in ipairs(modifiers) do
		local value = modifier:getValue()
		local chance = modifier:getChance()
		if value ~= 0 then
			local key = getModifierGroupKey(modifier)
			if not groups[key] then
				groups[key] = {
					modifier = modifier,
					combatTypes = {}
				}
				table.insert(groupOrder, key)
			end

			local combatType = modifier:getCombat()
			local combatName = combatTypeNames[combatType] or "unknown"
			table.insert(groups[key].combatTypes, combatName)
		end
	end

	local lines = {}
	for _, key in ipairs(groupOrder) do
		local group = groups[key]
		local modifier = group.modifier
		local parts = {}

		local modType = modifier:getType()
		local value = modifier:getValue()
		local chance = modifier:getChance()
		local isPercent = modifier:isPercent()
		local originType = modifier:getOrigin()
		local creatureName = modifier:getCreatureName()

		local modName
		if stance == ATTACK_MOD then
			modName = attackModifierNames[modType] or "unknown"
		elseif stance == DEFENSE_MOD then
			modName = defenseModifierNames[modType] or "unknown"
		else
			modName = "unknown"
		end

		local valueStr
		if isPercent then
			valueStr = string.format("%d%%", value)
		else
			valueStr = string.format("+%d", value)
		end

		table.insert(parts, string.format("%s %s", valueStr, modName))

		local hasSpecificCombat = false
		for _, name in ipairs(group.combatTypes) do
			if name ~= "all" then
				hasSpecificCombat = true
				break
			end
		end

		if hasSpecificCombat then
			table.insert(parts, string.format("(%s)", table.concat(group.combatTypes, ", ")))
		end

		if chance > 0 and chance < 100 then
			table.insert(parts, string.format("[%d%% chance]", chance))
		end

		if originType ~= ORIGIN_NONE then
			local originName = originTypeNames[originType] or "unknown"
			table.insert(parts, string.format("from %s", originName))
		end

		if creatureName and creatureName ~= "none" then
			table.insert(parts, string.format("vs %s", creatureName))
		end

		if modType == ATTACK_MODIFIER_CONVERSION or modType == DEFENSE_MODIFIER_REFORM then
			local conversionType = modifier:getConversion()
			if conversionType and conversionType ~= COMBAT_NONE then
				local conversionName = combatTypeNames[conversionType] or "unknown"
				table.insert(parts, string.format("to %s", conversionName))
			end
		end

		table.insert(lines, "  " .. table.concat(parts, " "))
	end

	return lines
end

function buildAugmentDescription(augment)
	local lines = {}

	local attackMods = augment:getAttackModifiers()
	if attackMods and #attackMods > 0 then
		local attackLines = buildGroupedModifierLines(attackMods, ATTACK_MOD)
		if #attackLines > 0 then
			table.insert(lines, "Offense:")
			for _, line in ipairs(attackLines) do
				table.insert(lines, line)
			end
		end
	end

	local defenseMods = augment:getDefenseModifiers()
	if defenseMods and #defenseMods > 0 then
		local defenseLines = buildGroupedModifierLines(defenseMods, DEFENSE_MOD)
		if #defenseLines > 0 then
			table.insert(lines, "Defense:")
			for _, line in ipairs(defenseLines) do
				table.insert(lines, line)
			end
		end
	end

	if #lines > 0 then
		return table.concat(lines, "\n")
	end

	return ""
end
