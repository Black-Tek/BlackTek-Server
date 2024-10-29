local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)
    npcHandler:onCreatureSay(cid, type, msg)

    local player = Player(cid) -- Access the player within onCreatureSay

    if not player then
        return
    end

    local BLESSCOST = 20000 + (player:getLevel() * 75) 


    local blessKeyword = keywordHandler:addKeyword({'solitude'}, StdModule.say, {npcHandler = npcHandler, text = 'Would you like to receive that protection for a sacrifice of '..BLESSCOST..' gold, child?'})
    blessKeyword:addChildKeyword({'yes'}, StdModule.bless, {npcHandler = npcHandler, text = 'So receive the wisdom of solitude, pilgrim.', cost = BLESSCOST, bless = 5})
    blessKeyword:addChildKeyword({''}, StdModule.say, {npcHandler = npcHandler, text = 'Fine. You are free to decline my offer.', reset = true})
    keywordHandler:addAliasKeyword({'wisdom'})
end
function onThink()				npcHandler:onThink()					end



npcHandler:setMessage(MESSAGE_GREET, 'Welcome to my little garden, adventurer |PLAYERNAME|, are you looking for the {solitude} bless? or simply to {trade} and bought an amulet of loss?')
npcHandler:setMessage(MESSAGE_FAREWELL, 'It was a pleasure to help you, |PLAYERNAME|.')
npcHandler:setMessage(MESSAGE_WALKAWAY, 'It was a pleasure to help you, |PLAYERNAME|.')

npcHandler:addModule(FocusModule:new())
