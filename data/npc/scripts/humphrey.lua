local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end

local BLESSCOST = 50000
-- Embrace of Tibia
local blessKeyword = keywordHandler:addKeyword({'embrace'}, StdModule.say, {npcHandler = npcHandler, text = 'Would you like to receive that protection for a sacrifice of '..BLESSCOST..' gold, child?'})
	blessKeyword:addChildKeyword({'yes'}, StdModule.bless, {npcHandler = npcHandler, text = 'So receive the embrace of Tibia, pilgrim.', cost = BLESSCOST, bless = 2})
	blessKeyword:addChildKeyword({''}, StdModule.say, {npcHandler = npcHandler, text = 'Fine. You are free to decline my offer.', reset = true})


npcHandler:setMessage(MESSAGE_GREET, 'Welcome, noble |PLAYERNAME|, are you looking for {embrace} blessing huh?')
npcHandler:setMessage(MESSAGE_WALKAWAY, 'Good Bye, noble |PLAYERNAME|')
npcHandler:setMessage(MESSAGE_FAREWELL, 'Good Bye, noble |PLAYERNAME|')

npcHandler:addModule(FocusModule:new())
