local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid)              npcHandler:onCreatureAppear(cid)            end
function onCreatureDisappear(cid)           npcHandler:onCreatureDisappear(cid)         end
function onCreatureSay(cid, type, msg)      npcHandler:onCreatureSay(cid, type, msg)    end
function onThink()                          npcHandler:onThink()                        end

keywordHandler:addKeyword({'captain'}, StdModule.say, {npcHandler = npcHandler, text = 'I am the captain of this sailing-ship.'})
keywordHandler:addKeyword({'sail'}, StdModule.say, {npcHandler = npcHandler, text = 'Where do you want to go? To {Dwalling}, {Echtelion}, {Rivendell} or {Crab Island}?'})

local node1 = keywordHandler:addKeyword({'dwalling'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = 'Do you seek a passage to Dwalling for 400 gold?'})
    node1:addChildKeyword({'yes'}, StdModule.travel, {npcHandler = npcHandler, premium = true, cost = 400, destination = Position(1682, 1187, 7)})
    node1:addChildKeyword({'no'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, reset = true, text = 'We would like to serve you some time.'})

local node2 = keywordHandler:addKeyword({'echtelion'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = 'Do you seek a passage to Echtelion for 400 gold?'})
    node2:addChildKeyword({'yes'}, StdModule.travel, {npcHandler = npcHandler, premium = true, cost = 400, destination = Position(1483, 1011, 7)})
    node2:addChildKeyword({'no'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, reset = true, text = 'We would like to serve you some time.'})

local node3 = keywordHandler:addKeyword({'rivendell'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = 'Do you seek a passage to Rivendell for 850 gold?'})
    node3:addChildKeyword({'yes'}, StdModule.travel, {npcHandler = npcHandler, premium = true, cost = 850, destination = Position(1991, 1170, 7)})
    node3:addChildKeyword({'no'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, reset = true, text = 'We would like to serve you some time.'})

local node4 = keywordHandler:addKeyword({'crab island'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = 'Do you seek a passage to Crab Island for 150 gold?'})
    node4:addChildKeyword({'yes'}, StdModule.travel, {npcHandler = npcHandler, premium = true, cost = 150, destination = Position(1533, 1159, 7)})
    node4:addChildKeyword({'no'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, reset = true, text = 'We would like to serve you some time.'})





npcHandler:addModule(FocusModule:new())
