local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end



local function creatureSayCallback(cid, type, msg)
    if not npcHandler:isFocused(cid) then
        return false
    end

    local player = Player(cid)

    if msgcontains(msg, "exchange") then
            npcHandler:say("Are you looking to exchange {blue and golden cordon}, {sun medal}, {sunray emblem} and {golden bijou}? I can trade my {regalia of suon} for these four items.", cid)
    elseif msgcontains(msg, "regalia of suon") then
        if player then
            if player:removeItem(27264,1) and player:removeItem(27265, 1) and player:removeItem(27266, 1) and player:removeItem(27267, 1) then 
                player:addItem(27268, 1)
            else
                npcHandler:say("Sorry about that, but you didn't have one of these necessary items to exchange my Regalia of Suon. Come back when you get all the four items.", cid)
            end
        end
    end

    return true
end

npcHandler:setMessage(MESSAGE_GREET, "Hello |PLAYERNAME|! Are you looking for an {exchange} to trade my {regalia of suon} ? ")
npcHandler:setMessage(MESSAGE_FAREWELL, "Always be on guard, |PLAYERNAME|!")

npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())
