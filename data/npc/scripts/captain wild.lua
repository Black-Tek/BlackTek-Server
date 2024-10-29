local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid)              npcHandler:onCreatureAppear(cid)            end
function onCreatureDisappear(cid)           npcHandler:onCreatureDisappear(cid)         end
function onCreatureSay(cid, type, msg)      npcHandler:onCreatureSay(cid, type, msg)    end
function onThink()                          npcHandler:onThink()                        end

keywordHandler:addKeyword({'captain'}, StdModule.say, {npcHandler = npcHandler, text = 'I am the captain of this sailing-ship.'})
keywordHandler:addKeyword({'sail'}, StdModule.say, {npcHandler = npcHandler, text = 'Where do you want to go? To {Mordragor}, {Falanaar}, {Dolwatha}, {Freewind}, {Death Valley}, {Bounac} or {Arkeron}?'})

    local node1 = keywordHandler:addKeyword({'mordragor'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = 'Do you seek a passage to Mordragor for 280 gold?'})
        node1:addChildKeyword({'yes'}, StdModule.travel, {npcHandler = npcHandler, premium = true, cost = 280, destination = Position(1024, 448, 6)})
        node1:addChildKeyword({'no'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, reset = true, text = 'We would like to serve you some time.'})
    
    local node2 = keywordHandler:addKeyword({'falanaar'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = 'Do you seek a passage to Falanaar for 220 gold?'})
        node2:addChildKeyword({'yes'}, StdModule.travel, {npcHandler = npcHandler, premium = true, cost = 220, destination = Position(712, 461, 6)})
        node2:addChildKeyword({'no'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, reset = true, text = 'We would like to serve you some time.'})
    
    local node3 = keywordHandler:addKeyword({'dolwatha'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = 'Do you seek a passage to Dolwatha for 360 gold?'})
        node3:addChildKeyword({'yes'}, StdModule.travel, {npcHandler = npcHandler, premium = true, cost = 360, destination = Position(510, 318, 6)})
        node3:addChildKeyword({'no'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, reset = true, text = 'We would like to serve you some time.'})
    
    local node4 = keywordHandler:addKeyword({'freewind'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = 'Do you seek a passage to Freewind for 320 gold?'})
        node4:addChildKeyword({'yes'}, StdModule.travel, {npcHandler = npcHandler, premium = true, cost = 320, destination = Position(1264, 1105, 6)})
        node4:addChildKeyword({'no'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, reset = true, text = 'We would like to serve you some time.'})
    
    local node5 = keywordHandler:addKeyword({'arkeron'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = 'Do you seek a passage to Arkeron for 275 gold?'})
        node5:addChildKeyword({'yes'}, StdModule.travel, {npcHandler = npcHandler, premium = true, cost = 275, destination = Position(1443, 1221, 5)})
        node5:addChildKeyword({'no'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, reset = true, text = 'We would like to serve you some time.'})
    
    local node6 = keywordHandler:addKeyword({'death valley'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = 'Do you seek a passage to Death Valley for 350 gold?'})
        node6:addChildKeyword({'yes'}, StdModule.travel, {npcHandler = npcHandler, premium = true, cost = 350, destination = Position(1318, 645, 7)})
        node6:addChildKeyword({'no'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, reset = true, text = 'We would like to serve you some time.'})

    local node7 = keywordHandler:addKeyword({'bounac'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = 'Do you seek a passage to Bounac for 470 gold?'})
        node7:addChildKeyword({'yes'}, StdModule.travel, {npcHandler = npcHandler, premium = true, cost = 470, destination = Position(2236, 762, 6)})
        node7:addChildKeyword({'no'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, reset = true, text = 'We would like to serve you some time.'})



npcHandler:addModule(FocusModule:new())
