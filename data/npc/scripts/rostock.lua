local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end


npcHandler:setMessage(MESSAGE_GREET, "Hi |PLAYERNAME|. Interesting, are you looking to buy an axe to start cutting tree's ? Just {trade} with me.")
npcHandler:setMessage(MESSAGE_FAREWELL, "This wood plank seems to be interesting.")
npcHandler:setMessage(MESSAGE_WALKAWAY, "Let's try this... hum, doesnt work.")
npcHandler:setMessage(MESSAGE_SENDTRADE, 'Bought an axe and start cutting tree\'s, for starting you will be able to cut Big Trees, until you can craft the next axe on Woodcutter\'s Craft table.')
npcHandler:addModule(FocusModule:new())
