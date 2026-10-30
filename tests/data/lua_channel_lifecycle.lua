ACCOUNT_TYPE_GOD = 6
TALKTYPE_CHANNEL_Y, TALKTYPE_CHANNEL_O, TALKTYPE_CHANNEL_R1 = 1, 2, 3
local players, messages = {}, {}
Player = function(id) return players[id] end
local function player(id, accountType)
	local value = {}
	function value:getId() return id end
	function value:getAccountType() return accountType end
	function value:getName() return "Operator" end
	function value:sendChannelMessage(_, message, talkType)
		messages[#messages + 1] = {message = message, talkType = talkType}
	end
	players[id] = value
	return value
end
dofile("data/chatchannels/scripts/lua_channel.lua")

local unauthorized = player(1, 1)
assert(not canJoin(unauthorized))
assert(onSpeak(unauthorized, 1, "error('must not run')") == false)
assert(#messages == 0)

local operator = player(2, ACCOUNT_TYPE_GOD)
assert(canJoin(operator))
onSpeak(operator, 1, "sessionValue = 123")
onSpeak(operator, 1, "print(sessionValue)")
assert(messages[#messages].message == "123")
print("PASS Lua channel retains session state for authorized operators")

local retained = setmetatable({operator}, {__mode = "v"})
if onLeave then assert(onLeave(operator)) end
players[2], operator = nil, nil
collectgarbage("collect")
collectgarbage("collect")
assert(retained[1] == nil, "A departed player's console must not retain the player")
print("PASS departed Lua console releases its captured player")

operator = player(2, ACCOUNT_TYPE_GOD)
onSpeak(operator, 1, "print(sessionValue)")
assert(messages[#messages].message == "nil")
print("PASS leaving Lua channel discards the previous console session")
