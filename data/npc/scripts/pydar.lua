local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end

local BLESSCOST = 50000
-- Spark of the Phoenix
local blessKeyword = keywordHandler:addKeyword({'phoenix'}, StdModule.say, {npcHandler = npcHandler, text = 'The Spark of the Phoenix is given by me and by the great geomancer of the local earth temple. Do you wish to receive my part of the Spark of the Phoenix for '..BLESSCOST..' gold?'})
	blessKeyword:addChildKeyword({'yes'}, StdModule.bless, {npcHandler = npcHandler, text = 'So receive the spark of the phoenix, pilgrim.', cost = BLESSCOST, bless = 4})
	blessKeyword:addChildKeyword({''}, StdModule.say, {npcHandler = npcHandler, text = 'Maybe another time.', reset = true})
keywordHandler:addAliasKeyword({'spark'})



npcHandler:setMessage(MESSAGE_GREET, 'Be greeted |PLAYERNAME|! I can smell the scent of a {phoenix} on you!')
npcHandler:setMessage(MESSAGE_FAREWELL, 'May the fire in your heart never die, |PLAYERNAME|!')
npcHandler:setMessage(MESSAGE_WALKAWAY, 'May the fire in your heart never die.')

npcHandler:addModule(FocusModule:new())
