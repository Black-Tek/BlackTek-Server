local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end

local BLESSCOST = 50000
-- Spiritual Shielding
local blessKeyword = keywordHandler:addKeyword({'spiritual'}, StdModule.say, {npcHandler = npcHandler, text = 'Here in the whiteflower temple you may receive the blessing of spiritual shielding. But we must ask of you to sacrifice '..BLESSCOST..' gold. Are you still interested?'})
	blessKeyword:addChildKeyword({'yes'}, StdModule.bless, {npcHandler = npcHandler, text = 'So receive the shielding of your spirit, pilgrim.', cost = BLESSCOST, bless = 1})
	blessKeyword:addChildKeyword({''}, StdModule.say, {npcHandler = npcHandler, text = 'Fine. You are free to decline my offer.', reset = true})


-- Healing

npcHandler:setMessage(MESSAGE_GREET, 'Welcome, pilgrim. How may I help you ? Do you wish the {spiritual} blessing ?')

npcHandler:addModule(FocusModule:new())
