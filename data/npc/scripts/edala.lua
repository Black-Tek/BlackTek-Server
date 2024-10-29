 local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end

local BLESSCOST = 25000


-- Fire of the Suns
local blessKeyword = keywordHandler:addKeyword({'suns'}, StdModule.say, {npcHandler = npcHandler, text = 'Would you like to receive that protection for a sacrifice of 25.000 gold, child?'})
	blessKeyword:addChildKeyword({'yes'}, StdModule.bless, {npcHandler = npcHandler, text = 'So receive the fire of the suns, pilgrim.', cost = BLESSCOST, bless = 3})
	blessKeyword:addChildKeyword({''}, StdModule.say, {npcHandler = npcHandler, text = 'Fine. You are free to decline my offer.', reset = true})
keywordHandler:addAliasKeyword({'fire'})


npcHandler:setMessage(MESSAGE_WALKAWAY, 'Asha Thrazi, |PLAYERNAME|!')
npcHandler:setMessage(MESSAGE_FAREWELL, 'Asha Thrazi, |PLAYERNAME|!')

npcHandler:setMessage(MESSAGE_GREET, 'Welcome |PLAYERNAME|! Are you looking for {suns} blessing?')

npcHandler:addModule(FocusModule:new())
