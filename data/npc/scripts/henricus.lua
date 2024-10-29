local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end

-- Define the getBlessings function


local function creatureSayCallback(cid, type, msg)
    if not npcHandler:isFocused(cid) then
        return false
    end

    local player = Player(cid)
    local totalBlessPrice = 75 * player:getLevel() * 5 * 1.1

    if msgcontains(msg, "blessing") or msgcontains(msg, "bless") then
        if player then
            npcHandler:say("Do you want to receive the blessing of the inquisition - which means all five available blessings - for " .. totalBlessPrice .. " gold?", cid)
            npcHandler.topic[cid] = 1 
        else
            npcHandler:say("You cannot get this blessing unless you have completed The Inquisition Quest.", cid)
            npcHandler.topic[cid] = 0 
        end
    elseif npcHandler.topic[cid] == 1 then
        if player:getBlessings() >= 5 then
            npcHandler:say("You already have been blessed!", cid)
        elseif player:removeMoney(totalBlessPrice) then
            npcHandler:say("You have been blessed by all five gods! ", cid)
            for b = 1, 5 do
                player:addBlessing(b)
            end
            player:getPosition():sendMagicEffect(CONST_ME_HOLYAREA)
        else
            npcHandler:say("Come back when you have enough money.", cid)
        end
        npcHandler.topic[cid] = 0
    end

    return true
end

keywordHandler:addKeyword({'paladin'}, StdModule.say, {npcHandler = npcHandler, text = 'It\'s a shame that only a few paladins still use their abilities to further the cause of the gods of good. Too many paladins have become selfish and greedy.'})
keywordHandler:addKeyword({'knight'}, StdModule.say, {npcHandler = npcHandler, text = 'Nowadays, most knights seem to have forgotten the noble cause to which all knights were bound in the past. Only a few have remained pious, serve the gods and follow their teachings.'})
keywordHandler:addKeyword({'sorcerer'}, StdModule.say, {npcHandler = npcHandler, text = 'Those who wield great power have to resist great temptations. We have the burden to eliminate all those who give in to the temptations.'})
keywordHandler:addKeyword({'druid'}, StdModule.say, {npcHandler = npcHandler, text = 'The druids here still follow the old rules. Sadly, the druids of Carlin have left the right path in the last years.'})

npcHandler:setMessage(MESSAGE_GREET, "Greetings, fellow believer |PLAYERNAME|! That was a great effort to come here far away from the mainlands, are you looking for all {blessing}s ?")
npcHandler:setMessage(MESSAGE_FAREWELL, "Always be on guard, |PLAYERNAME|!")
npcHandler:setMessage(MESSAGE_WALKAWAY, "This ungraceful haste is most suspicious!")

npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())
