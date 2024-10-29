local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end


npcHandler:setMessage(MESSAGE_GREET, "Hi |PLAYERNAME|. Interesting, are you looking to buy a pick to start mining ? Just {trade} with me.")
npcHandler:setMessage(MESSAGE_FAREWELL, "Bronze, silver, sapphire... i like emeraudes.")
npcHandler:setMessage(MESSAGE_WALKAWAY, "This green color...it's to brillant.")
npcHandler:setMessage(MESSAGE_SENDTRADE, 'Bought a Pick and start mining, for starting you will be able to mine on Bronze Veins, until you can craft the next pick on Mining Forge.')
npcHandler:addModule(FocusModule:new())
