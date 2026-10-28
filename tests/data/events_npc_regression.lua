-- Run from the repository root: lua tests/data/events_npc_regression.lua
local passed = 0
local function test(name, fn)
	local ok, err = pcall(fn)
	assert(ok, name .. ": " .. tostring(err))
	passed = passed + 1; print("PASS " .. name)
end
local function noop() end
local rawDofile = dofile
dofile = function(path) if path ~= "data/npc/lib/npcsystem/npcsystem.lua" then return rawDofile(path) end end
rawDofile("data/npc/lib/npc.lua")
dofile = rawDofile

test("money parser rejects text, excessive values and nonfinite money", function()
	assert(getMoneyCount("hello") == -1 and getCount("no") == -1)
	assert(getMoneyCount("withdraw 100") == 100)
	assert(getMoneyCount("4294967296") == -1)
	assert(getMoneyCount(string.rep("9", 400)) == -1)
	assert(not isValidMoney(0 / 0) and not isValidMoney(math.huge) and not isValidMoney(-1))
	assert(not isValidMoney(1.5) and isValidMoney(100))
end)

local scheduled = {}
function addEvent(fn, delay, ...) scheduled[#scheduled + 1] = {fn = fn, args = {...}}; return #scheduled end
local online = {}
Player = setmetatable({}, {__call = function(_, id) return type(id) == "table" and id or online[id] end})
local npcExists = true
function Creature(id) if npcExists then return {say = noop, getPosition = function() return {} end} end end
test("delayed NPC speech tolerates disappearance of player or NPC", function()
	online[1] = {}
	local event = {}; doCreatureSayWithDelay(2, "hello", 1, 1000, event, 1)
	online[1] = nil
	local queued = table.remove(scheduled, 1)
	assert(pcall(queued.fn, table.unpack(queued.args)))
	online[1] = {}; doCreatureSayWithDelay(2, "hello", 1, 1000, event, 1)
	npcExists = false
	queued = table.remove(scheduled, 1)
	assert(pcall(queued.fn, table.unpack(queued.args)))
	local before = #scheduled
	doCreatureSayWithDelay(2, "hello", 1, 1000, event, 1)
	assert(#scheduled == before)
	npcExists = true
end)

CALLBACK_GREET, CALLBACK_MESSAGE_DEFAULT, CALLBACK_ONRELEASEFOCUS, MESSAGE_GREET = 1, 2, 3, 4
VOCATION_NONE, ITEM_GOLD_COIN, ITEM_PLATINUM_COIN, ITEM_CRYSTAL_COIN = 0, 2148, 2152, 2160
NpcSystem = {parseParameters = noop}
local handler
NpcHandler = {new = function(_, keywords)
	handler = {topic = {}, callbacks = {}, messages = {}, keywordHandler = keywords}
	function handler:isFocused() return true end
	function handler:say(message) self.messages[#self.messages + 1] = message end
	function handler:setCallback(id, fn) self.callbacks[id] = fn end
	function handler:setMessage() end
	function handler:addModule() end
	function handler:releaseFocus() end
	function handler:resetNpc() end
	return handler
end}
dofile("data/npc/lib/npcsystem/keywordhandler.lua")
dofile("data/npc/lib/npcsystem/modules.lua")
string.split = function(s, sep) local t = {}; for v in s:gmatch("[^" .. sep .. "]+") do t[#t + 1] = v end; return t end
string.trim = function(s) return s:match("^%s*(.-)%s*$") end
local player = {money = 1000000, coins = {[2148] = 1000000, [2152] = 100000, [2160] = 10000}}
function player:getId() return 1 end
function player:getName() return "Sender" end
function player:getBankBalance() return 1000000 end
function player:getVocation() return {getId = function() return 1 end} end
function player:getFreeCapacity() return 1000000 end
function player:removeItem(id, count)
	assert(count >= 1 and count <= 1000000, "unsafe native removeItem count")
	self.removals = (self.removals or 0) + 1
	if self.coins[id] < count then return false end
	self.coins[id] = self.coins[id] - count; return true
end
function player:addItem(id, count)
	assert(count >= 1 and count <= 10000, "unsafe native addItem count")
	self.coins[id] = (self.coins[id] or 0) + count; return {}
end
function player:transferMoneyTo(target, count) self.transfer = {target = target, count = count}; return true end
online[1] = player
local lookups = 0
function getPlayerDatabaseInfo(name)
	lookups = lookups + 1
	return name == "Valid" and {name = "Valid", guid = 100, vocation = 1} or false
end
dofile("data/npc/scripts/bank.lua")
local bank = handler
local function say(message) return bank.callbacks[CALLBACK_MESSAGE_DEFAULT](1, 0, message) end
local function reset() bank.topic[1] = 0; bank.callbacks[CALLBACK_GREET](1) end
getMoneyWeight = function() return 0 end

test("staged bank transfer looks up and accepts valid recipient", function()
	reset(); say("transfer 100"); say("Valid"); say("yes")
	assert(lookups == 1 and player.transfer.count == 100 and player.transfer.target.name == "Valid")
end)
test("unknown bank recipients produce feedback without Lua errors", function()
	reset(); say("transfer 100 Missing")
	assert(bank.messages[#bank.messages] == "This player does not exist.")
	reset(); say("transfer 100"); say("Missing")
	assert(bank.messages[#bank.messages]:find("no records"))
	say("Valid"); say("yes")
	assert(player.transfer.target.name == "Valid")
end)
test("coin exchange rejects overflowing and huge output requests", function()
	local removals = player.removals or 0
	reset(); say("change gold"); say("42949673"); say("yes")
	assert((player.removals or 0) == removals)
	reset(); say("change platinum"); say("gold"); say("101"); say("yes")
	assert((player.removals or 0) == removals)
	reset(); say("change crystal"); say("101"); say("yes")
	assert((player.removals or 0) == removals)
end)
test("bounded conversions preserve exact coin value", function()
	reset(); local gold, platinum = player.coins[2148], player.coins[2152]
	say("change gold"); say("50"); say("yes")
	assert(player.coins[2148] == gold - 5000 and player.coins[2152] == platinum + 50)
	reset(); gold, platinum = player.coins[2148], player.coins[2152]
	say("change platinum"); say("gold"); say("50"); say("yes")
	assert(player.coins[2148] == gold + 5000 and player.coins[2152] == platinum - 50)
end)
test("nonnumeric amount reply remains a normal banking interaction", function()
	reset(); say("change crystal"); say("no")
	assert(bank.topic[1] == 0)
end)

CONST_ME_TELEPORT = 1
function Position(x, y, z)
	if type(x) == "table" then return x end
	return {x = x, y = y, z = z, sendMagicEffect = noop}
end
function player:isPremium() return true end
function player:isPzLocked() return false end
function player:getPosition() return Position(0, 0, 7) end
function player:removeTotalMoney(amount) self.money = self.money - amount; return true end
function player:teleportTo(destination) self.destination = destination; return true end
local randomCalls = 0
local savedRandom = math.random
math.random = function() randomCalls = randomCalls + 1; return randomCalls == 1 and 1 or 2 end
dofile("data/npc/scripts/ship.lua")
local ship = handler
local function route(name)
	for _, node in ipairs(ship.keywordHandler:getRoot().children) do
		if node.keywords[1] == name then return node end
	end
	error("missing route " .. name)
end
test("alternate ship destination is rolled for each voyage", function()
	assert(randomCalls == 0)
	local yes = route("trekolt").children[1]
	yes:processMessage(1, "yes"); assert(player.destination.x == 489)
	yes:processMessage(1, "yes"); assert(player.destination.x == 95 and randomCalls == 2)
end)
test("Varak fare agrees with its quoted price", function()
	assert(route("varak").parameters.text:find("150 gold"))
	local before = player.money
	route("varak").children[1]:processMessage(1, "yes")
	assert(player.money == before - 150)
end)
math.random = savedRandom

GUEST_LIST = 256
function player:sendCancelMessage(message) self.cancelMessage = message end
local canEdit, wraps = false, 0
local house = {canEditAccessList = function(_, listId, p)
	assert(listId == GUEST_LIST and p == player); return canEdit
end, getAccessList = function() error("authorization must use engine rules") end}
local replacement = {setAttribute = noop}
local tile = {getHouse = function() return house end, addItem = function() return replacement end}
function Tile() return tile end
local wrapped = {
	getTopParent = function() return {getPosition = function() return {} end} end,
	getAttribute = function() return 123 end, getId = function() return 456 end,
	remove = function() wraps = wraps + 1 end
}
EventCallback = {}
dofile("data/events/scripts/player.lua")
test("wrapping honors parsed engine house authorization", function()
	Player.onWrapItem(player, wrapped); assert(wraps == 0 and player.cancelMessage)
	canEdit = true; Player.onWrapItem(player, wrapped); assert(wraps == 1)
end)

local events = {}
function GlobalEvent(name)
	local e = {}; function e:type() end; function e:time() end; function e:register() events[name] = self end; return e
end
configKeys = {SERVER_SAVE_CLEAN_MAP = 1, SERVER_SAVE_CLOSE = 2, SERVER_SAVE_SHUTDOWN = 3,
	SERVER_SAVE_NOTIFY_MESSAGE = 4, SERVER_SAVE_NOTIFY_DURATION = 5}
configManager = {getBoolean = function() return false end, getNumber = function() return 1 end}
local saves = 0
function saveServer() saves = saves + 1 end
Game = {setGameState = function() error("server must remain online") end}
dofile("data/scripts/globalevents/serversave.lua")
test("scheduled save saves while close and shutdown are disabled", function()
	events.global_server_save.onTime()
	while #scheduled > 0 do
		local nextEvent = table.remove(scheduled, 1)
		nextEvent.fn(table.unpack(nextEvent.args))
	end
	assert(saves == 1)
end)

-- Query stubs model two expired house auctions with the same bidder and a
-- balance sufficient for only one. The test verifies the conditional atomic
-- statement and use of its resulting owner, not MySQL's execution engine.
local auctions = {
	[1] = {id = 1, owner = 0, highest_bidder = 7, last_bid = 60},
	[2] = {id = 2, owner = 0, highest_bidder = 7, last_bid = 60}
}
local balance, assigned, paidQueries = 100, {}, 0
configKeys.MARKET_OFFER_DURATION, configKeys.CHECK_DUPLICATE_STORAGE_KEYS = 6, 7
Game.getTowns = function() return {} end
db = {}
function db.query(query)
	if query:find("INNER JOIN", 1, true) then
		assert(query:find("`p`.`balance` = `p`.`balance` - `h`.`last_bid`", 1, true))
		assert(query:find("`p`.`balance` >= `h`.`last_bid`", 1, true))
		paidQueries = paidQueries + 1
		local id = tonumber(query:match("WHERE `h`%.`id` = (%d+)"))
		local auction = auctions[id]
		if balance >= auction.last_bid and auction.owner == 0 then
			balance = balance - auction.last_bid; auction.owner = 7
		end
	end
	return true
end
function db.asyncQuery() end
function db.storeQuery(query)
	if query:find("SELECT `id` FROM `houses`", 1, true) then return {rows = {auctions[1], auctions[2]}, index = 1} end
	local id = query:match("SELECT `owner` FROM `houses` WHERE `id` = (%d+)")
	if id then return {rows = {auctions[tonumber(id)]}, index = 1} end
	return false
end
result = {
	getNumber = function(r, name) return r.rows[r.index][name] end,
	next = function(r) r.index = r.index + 1; return r.rows[r.index] ~= nil end,
	free = noop
}
function House(id)
	return {getId = function() return id end, setOwnerGuid = function(_, owner, updateDatabase)
		assert(updateDatabase == false); assigned[id] = owner
	end}
end
dofile("data/scripts/globalevents/startup.lua")
test("house auction processing does not reuse bidder balance snapshots", function()
	events.global_startup_event.onStartup()
	assert(paidQueries == 2 and balance == 40 and assigned[1] == 7 and assigned[2] == nil)
end)

TILESTATE_PROTECTIONZONE, ITEM_TYPE_DEPOT = 1, 2
actionIds = {levelCheck = 1000, levelCheckLast = 2000, levelCheckExtended = 2001, levelCheckExtendedLast = 3000}
levelCheckConfig = {}
local stepped = {itemid = 416, actionid = 0, transform = noop}
local position = {marker = true}
function Tile(p)
	if p == position then return {hasFlag = function() return true end} end
	return nil
end
local walker = {
	isPlayer = function() return true end, isInGhostMode = function() return false end,
	getPosition = function() return {getNextPosition = noop} end,
	getDirection = function() return 1 end
}
local stepOn
function ItemEvent()
	local event = {id = noop}
	function event:register() if self.onStepOn then stepOn = self.onStepOn end end
	return event
end
dofile("data/scripts/itemevents/step/tiles.lua")
test("pressure plate tolerates an absent tile in front of player", function()
	assert(stepOn(walker, stepped, position, {}) == true)
end)
print(string.format("%d events NPC regression tests passed", passed))
