-- Run from the repository root with Lua 5.3+.
-- Exercise the real helpers without a database connection or game world.
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
Condition = function() return {} end
local online = {}
Player = setmetatable({}, {__call = function(_, id) return online[id] end})
dofile("data/lib/core/player.lua")

local function player(balance, cash, removeSucceeds)
	local value = {balance = balance, cash = cash, removed = {}, messages = {}}
	function value:getBankBalance() return self.balance end
	function value:setBankBalance(amount) self.balance = amount end
	function value:getMoney() return self.cash end
	function value:removeMoney(amount)
		self.removed[#self.removed + 1] = amount
		if not removeSucceeds then return false end
		self.cash = self.cash - amount
		return true
	end
	function value:sendTextMessage(_, message) self.messages[#self.messages + 1] = message end
	return setmetatable(value, {__index = Player})
end
local recipient = {guid = 123, name = "Recipient"}

test("failed offline transfer preserves sender balance", function()
	local sender = player(100, 0, true)
	db = {query = function() return false end}
	assert(sender:transferMoneyTo(recipient, 25) == false)
	assert(sender.balance == 100)
end)

test("successful offline transfer debits once", function()
	local sender, queries = player(100, 0, true), 0
	db = {query = function(query)
		assert(query:find("`balance` + 25", 1, true))
		queries = queries + 1
		return true
	end}
	assert(sender:transferMoneyTo(recipient, 25))
	assert(sender.balance == 75 and queries == 1)
end)

test("online transfer updates in-memory balances", function()
	local sender, receiver = player(100, 0, true), player(50, 0, true)
	online[recipient.guid] = receiver
	db = {query = function() error("online transfer must not write stale database balance") end}
	assert(sender:transferMoneyTo(recipient, 25))
	assert(sender.balance == 75 and receiver.balance == 75)
	online[recipient.guid] = nil
end)

test("failed cash-only payment is not reported as paid", function()
	local buyer = player(50, 100, false)
	assert(buyer:removeTotalMoney(75) == false)
	assert(buyer.balance == 50 and buyer.cash == 100 and #buyer.messages == 0)
end)

test("failed mixed payment preserves bank balance", function()
	local buyer = player(100, 25, false)
	assert(buyer:removeTotalMoney(75) == false)
	assert(buyer.balance == 100 and buyer.cash == 25 and #buyer.messages == 0)
end)

test("successful mixed payment deducts exact amount", function()
	local buyer = player(100, 25, true)
	assert(buyer:removeTotalMoney(75))
	assert(buyer.balance == 50 and buyer.cash == 0 and #buyer.messages == 1)
end)

dofile("data/npc/lib/npcsystem/modules.lua")
local position = {sendMagicEffect = noop}
Position = function() return position end
Npc = function() return {uid = 7, getName = function() return "Captain" end} end
getNpcCid = function() return 7 end
shop_npcuid, shop_cost, shop_destination, shop_premium = {[1] = 7}, {[1] = 10}, {[1] = {}}, {[1] = false}

local function traveler(locked)
	local value = {payments = 0, teleports = 0}
	function value:isPremium() return true end
	function value:isPzLocked() return locked end
	function value:removeTotalMoney() self.payments = self.payments + 1; return true end
	function value:getPosition() return position end
	function value:teleportTo() self.teleports = self.teleports + 1; return true end
	online[1] = value
	return value
end
local handler = {isFocused = function() return true end, say = noop, resetNpc = noop, releaseFocus = noop}
local travelParameters = {module = {npcHandler = handler}, cost = 10, destination = {}, premium = false}

test("confirmed travel checks combat lock before charging", function()
	local value = traveler(true)
	assert(TravelModule.onConfirm(1, "yes", {}, travelParameters))
	assert(value.payments == 0 and value.teleports == 0)
end)

test("direct travel also honors combat lock", function()
	local value = traveler(true)
	assert(TravelModule.bringMeTo(1, "bring me to port", {}, travelParameters))
	assert(value.payments == 0 and value.teleports == 0)
end)

test("unlocked travel keeps normal payment and teleport behavior", function()
	local value = traveler(false)
	assert(TravelModule.onConfirm(1, "yes", {}, travelParameters))
	assert(value.payments == 1 and value.teleports == 1)
	assert(TravelModule.bringMeTo(1, "bring me to port", {}, travelParameters))
	assert(value.payments == 2 and value.teleports == 2)
end)

test("container shop parser preserves container and item IDs", function()
	local module, registration = ShopModule:new()
	function module:addBuyableItemContainer(names, container, item, cost, subtype, realName)
		registration = {names, container, item, cost, subtype, realName}
	end
	module:parseBuyableContainers("backpack of runes,1988,2260,100,1,blank rune")
	assert(registration, "container offer was discarded")
	assert(registration[1][1] == "backpack of runes")
	assert(registration[2] == 1988 and registration[3] == 2260 and registration[4] == 100)
	assert(registration[5] == 1 and registration[6] == "blank rune")
end)

print(string.format("%d payment and travel tests passed; %d failed", passed, failed))
assert(failed == 0)
