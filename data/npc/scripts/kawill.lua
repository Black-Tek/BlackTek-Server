local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end


-- Kawill Blessing
local blessKeyword = keywordHandler:addKeyword({'spark of the phoenix'}, StdModule.say, {npcHandler = npcHandler, text = 'The Spark of the Phoenix is given by me and by the great pyromancer in the nearby fire temple. Do you wish to receive my part of the Spark of the Phoenix?'})
	blessKeyword:addChildKeyword({'yes'}, StdModule.say, {npcHandler = npcHandler, text = 'So receive the blessing of the life-giving earth, pilgrim.', reset = true}, nil, function(player) player:setStorageValue(PlayerStorageKeys.KawillBlessing, 1) player:getPosition():sendMagicEffect(CONST_ME_MAGIC_BLUE) end)
	blessKeyword:addChildKeyword({''}, StdModule.say, {npcHandler = npcHandler, text = 'Ok. If you don\'t want it...', reset = true})

npcHandler:setMessage(MESSAGE_GREET, 'Welcome |PLAYERNAME|! Maybe {spark of the phoenix} will protect you!')
npcHandler:setMessage(MESSAGE_FAREWELL, 'Earth under your feet, |PLAYERNAME|!')
npcHandler:setMessage(MESSAGE_WALKAWAY, 'Earth under your feet, pilgrim. What brings you here?')

npcHandler:addModule(FocusModule:new())
