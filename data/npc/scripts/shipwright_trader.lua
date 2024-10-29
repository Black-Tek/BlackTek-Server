local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end

local function creatureSayCallback(cid, type, msg)
	if(not npcHandler:isFocused(cid)) then
	return false
	end
	return true
end


local function onTradeRequest(cid)
	return true
end

npcHandler:setMessage(MESSAGE_GREET, "Welcome |PLAYERNAME| are you looking for some pieces for your boat ? Ask me for a {trade}.")
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:setMessage(MESSAGE_FAREWELL, "Pieces for your boat here!")
npcHandler:setMessage(MESSAGE_WALKAWAY, "This one, oh no, this one better.")
npcHandler:setMessage(MESSAGE_SENDTRADE, "Let's do some business together.")
npcHandler:setCallback(CALLBACK_ONTRADEREQUEST, onTradeRequest)
npcHandler:addModule(FocusModule:new())
