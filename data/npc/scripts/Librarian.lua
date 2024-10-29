local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end

local function creatureSayCallback(cid, type, msg)
	if not npcHandler:isFocused(cid) then
		return false
	end

	return true
end



local travelNode = keywordHandler:addKeyword({'passage'}, StdModule.say, {npcHandler = npcHandler, text = 'Do you seek a passage to the isle of the kings for 10 gold coins?'})
	travelNode:addChildKeyword({'yes'}, StdModule.travel, {npcHandler = npcHandler, premium = false, text = 'Have a nice trip!', cost = 10, destination = Position(1001, 251, 6) })
	travelNode:addChildKeyword({'no'}, StdModule.say, {npcHandler = npcHandler, reset = true, text = 'Well, I\'ll be here if you change your mind.'})



local travelNode = keywordHandler:addKeyword({'return'}, StdModule.say, {npcHandler = npcHandler, text = 'Do you seek a passage to the Mordragor for 10 gold coins?'})
	travelNode:addChildKeyword({'yes'}, StdModule.travel, {npcHandler = npcHandler, premium = false, text = 'Have a nice trip!', cost = 10, destination = Position(973, 109, 7) })
	travelNode:addChildKeyword({'no'}, StdModule.say, {npcHandler = npcHandler, reset = true, text = 'Well, I\'ll be here if you change your mind.'})


npcHandler:setMessage(MESSAGE_GREET, "Be greeted, traveller |PLAYERNAME|. Do you want to visit the Isle of Kings or go to Secret Library ? Ask me for a {passage} or if you want to go back to Mordragor ask me for {return}")
npcHandler:setMessage(MESSAGE_FAREWELL, "Good bye. You are welcome.")
npcHandler:setMessage(MESSAGE_WALKAWAY, "Good bye.")

npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())
