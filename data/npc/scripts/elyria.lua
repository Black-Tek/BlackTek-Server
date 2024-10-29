local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end


npcHandler:setMessage(MESSAGE_GREET, "Hi |PLAYERNAME|. Interesting, are you looking to buy a scissor's to start harvesting plants and mushrooms ? Just {trade} with me.")
npcHandler:setMessage(MESSAGE_FAREWELL, "We can extract this substance from anemone and create a powerful potions.")
npcHandler:setMessage(MESSAGE_WALKAWAY, "Combining this cantharellus and... anemone, omg it doesn\'t work.")
npcHandler:setMessage(MESSAGE_SENDTRADE, 'Bought a scissors and start harvesting plant\'s, for starting you will be able to harvest Canterbury Bells, until you can craft the next scissor\'s on Herbalist\'s Craft table.')
npcHandler:addModule(FocusModule:new())
