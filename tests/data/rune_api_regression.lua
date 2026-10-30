-- Rune callbacks use only methods exposed by the native engine.
local passed, failed = 0, 0
local function test(name, callback)
	local ok, message = pcall(callback)
	if ok then
		passed = passed + 1
		print("PASS " .. name)
	else
		failed = failed + 1
		print("FAIL " .. name .. ": " .. tostring(message))
	end
end
local function noop() end
local lastSpell
Spell = function()
	lastSpell = setmetatable({}, {__index = function() return noop end})
	return lastSpell
end
PlayerFlag_CanConvinceAll, PlayerFlag_HasInfiniteMana = 1, 2
local position = {sendMagicEffect = noop}
local monsterType = {isConvinceable = function() return true end, manaCost = function() return 100 end}
local target = {isMonster = function() return true end, getType = function() return monsterType end}
Creature = function() return target end
local variant = {getNumber = function() return 1 end, getPosition = function() return position end}
local function player(mana, count)
	local value = {mana = mana, spent = 0, summons = {}}
	for _ = 1, count or 0 do value.summons[#value.summons + 1] = {} end
	function value:getMana() return self.mana end
	function value:addMana(amount) self.mana = self.mana + amount end
	function value:addManaSpent(amount) self.spent = self.spent + amount end
	function value:getSummons() return self.summons end
	function value:addSummon(monster) self.summons[#self.summons + 1] = monster; return true end
	function value:hasFlag() return false end
	function value:getPosition() return position end
	value.sendCancelMessage = noop
	return value
end
dofile("data/scripts/spells/runes/support/convince_creature_rune.lua")
local convince = lastSpell
test("convince rune uses MonsterType manaCost API", function()
	local caster = player(150)
	assert(convince.onCastSpell(caster, variant) == true)
	assert(caster.mana == 50 and caster.spent == 100 and caster.summons[1] == target)
end)
test("convince rune preserves resources on insufficient mana", function()
	local caster = player(99)
	assert(convince.onCastSpell(caster, variant) == false)
	assert(caster.mana == 99 and caster.spent == 0 and #caster.summons == 0)
end)
test("convince rune keeps summon cap", function()
	local caster = player(150, 2)
	assert(convince.onCastSpell(caster, variant) == false)
	assert(caster.mana == 150 and caster.spent == 0 and #caster.summons == 2)
end)

table.contains = function(values, candidate)
	for _, value in pairs(values) do if value == candidate then return true end end
	return false
end
local function item(options)
	options = options or {}
	local value = {removed = false}
	function value:getType() return {isMovable = function() return not options.immovable end} end
	function value:getUniqueId() return options.uniqueId or 65536 end
	function value:getActionId() return options.actionId or 0 end
	function value:getId() return options.id or 2148 end
	function value:isStoreItem() return options.store or false end
	function value:remove() self.removed = true end
	return value
end
local items, house
Tile = function() return {getHouse = function() return house end, getItems = function() return items end} end
dofile("data/scripts/spells/runes/support/disintegrate_rune.lua")
local disintegrate = lastSpell
test("disintegrate removes loose items using supported Item API", function()
	items = {item()}
	assert(disintegrate.onCastSpell({}, variant) == true)
	assert(items[1].removed)
end)
test("disintegrate preserves protected and store items", function()
	items = {item({uniqueId = 42}), item({actionId = 42}), item({id = 3058}), item({store = true}), item({immovable = true})}
	assert(disintegrate.onCastSpell({}, variant) == true)
	for _, value in ipairs(items) do assert(not value.removed) end
end)
test("disintegrate preserves house contents", function()
	house, items = true, {item()}
	assert(disintegrate.onCastSpell({}, variant) == true)
	assert(not items[1].removed)
	house = nil
end)
test("disintegrate preserves podiums when that optional API is present", function()
	local podium = item()
	function podium:isPodium() return true end
	items = {podium}
	assert(disintegrate.onCastSpell({}, variant) == true)
	assert(not podium.removed)
end)

print(string.format("%d rune API tests passed; %d failed", passed, failed))
assert(failed == 0)
