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
	local player = Player(cid)
	if msg:lower() == "mission" then
		npcHandler:say("Welcome to Kindernia, start your adventure exploring around and make sure you do the chain quest before leaving the island!.", cid)
			player:setStorageValue(Storage.exploringKindernia, 1)
			player:setStorageValue(Storage.exploreAroundKindernia, 1)
			player:getPosition():sendMagicEffect(227)
	end
	return true
end

npcHandler:setMessage(MESSAGE_GREET, "Hi there |PLAYERNAME|, are you looking for do a {mission} ?.")
npcHandler:setMessage(MESSAGE_FAREWELL, "Bye and take care.")
npcHandler:setMessage(MESSAGE_WALKAWAY, "I have a {mission} available adventurer!")
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())