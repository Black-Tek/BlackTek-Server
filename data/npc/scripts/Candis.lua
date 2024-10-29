 local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)


function onCreatureAppear(cid) npcHandler:onCreatureAppear(cid) end
function onCreatureDisappear(cid) npcHandler:onCreatureDisappear(cid) end
function onCreatureSay(cid, type, msg) npcHandler:onCreatureSay(cid, type, msg) end
function onThink() npcHandler:onThink() end

function creatureSayCallback(cid, type, msg)
	if not npcHandler:isFocused(cid) then
		return false
	end
	
	local player = Player(cid)  -- Convert cid to Player object
	
	if msgcontains(msg, "mission") then
		if player:getStorageValue(78194) == 7 then
			npcHandler:say("Wow, you see the isle? Beautiful! Be aware of the candy monsters... Now you should use the teleport to the south to find the boss room. Be prepared.", cid)
			player:setStorageValue(78195, 1)
		elseif player:getStorageValue(78193) < 1 then
			npcHandler:say("So I guess you heard about sugar bosses on Candia, right?", cid)
			npcHandler.topic[cid] = 1
		else 
			npcHandler:say("You already started the mission, check your quest log.", cid)
		end
	elseif msgcontains(msg, "yes") then
		if npcHandler.topic[cid] == 1 then
			player:setStorageValue(78193, 1)
			player:setStorageValue(78194, 1)
			npcHandler:say("Fine, the first thing I need is for you to find pastry structures around the isle and report back when you've discovered all of them. There are 6 pastry decorations; for a start, you can find one to the north. Click on it and find the others.", cid)
			npcHandler:say("Come back when you've found all of them.", cid)
		end	
	end

	return true
end

npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())