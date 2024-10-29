local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end

local function creatureSayCallback(cid, type, msg)
	if(not npcHandler:isFocused(cid)) then
	return false
	end
	local player = Player(cid)
	if msg:lower() == "discover" then
		npcHandler:say("Ssshh puff, ssshh puff, so ahem, ahem, sorry about that, you want {explore} the ocean and found lost treasures ?.", cid)
	elseif msg:lower() == "explore" then
		npcHandler:say("Before explore the ocean, you need a boat, let's construct a boat, for this you need some {materials}, some exquisite planks for the hull and deck, some nails to hold the boat together,  metal fittings to reinforce and connect various components, a silk, important to stretch the sail of the boat, a small golden anchor to keep the boat in place when not sailing and some paint to protect the wood. Of course, you can buy all of this items to an old merchant. A whimsical merchant, an elusive ally, hoarding every ship part to sell to any daring enough to brave the tumultuous seas, called Shipwright Trader. You can found him on Dolwatha, on the passage to Falanaar Jungle.", cid)
	elseif msg:lower() == "materials" then
		npcHandler:say("Here is the list about what we need. You can found Shipwright Trader on the passage to falanaar at south of Dolwatha, come back when you get the materials for we start construction of the boat.\nFor Start we need some exquisite {planks}, for the hull and deck.", cid)
		player:addItem(7844, 1)
	elseif msg:lower() == "planks" then
		if player:removeItem(12503, 10) then
			npcHandler:say("Now for hold the boat together we need some {nails}", cid)
			player:setStorageValue(BoatMissions.MagnusBlackWaterFirst, 1)
		else
			npcHandler:say("For this you need 10 exquisite planks.", cid)
		end
	elseif msg:lower() == "nails" then
		if player:removeItem(8309, 10) and player:getStorageValue(BoatMissions.MagnusBlackWaterFirst) == 1 then
			npcHandler:say("We need also some extra {metal} fitting parts.", cid)
			player:setStorageValue(BoatMissions.MagnusBlackWaterSecond, 1)
		else
			npcHandler:say("You need 10 nails to continue the construction.", cid)
		end
	elseif msg:lower() == "metal" then
		if player:removeItem(10034, 5) and player:getStorageValue(BoatMissions.MagnusBlackWaterSecond) == 1 then
			npcHandler:say("Great, to continue we need one exquisite {silk} to connect various components.", cid)
			player:setStorageValue(BoatMissions.MagnusBlackWaterThird, 1)
		else
			npcHandler:say("You need 5 mettal fitting to reinforce the boat.", cid)
		end
	elseif msg:lower() == "silk" then
		if player:removeItem(12501, 1) and player:getStorageValue(BoatMissions.MagnusBlackWaterThird) == 1 then
			npcHandler:say("It's taking a while to construct the boat, is not so easy, well now we need a small golden {anchor} to keep the boat in place.", cid)
			player:setStorageValue(BoatMissions.MagnusBlackWaterFourth, 1)
		else
			npcHandler:say("You need 1 Silk to connect various components.", cid)
		end
	elseif msg:lower() == "anchor" then
		if player:removeItem(15432, 1) and player:getStorageValue(BoatMissions.MagnusBlackWaterFourth) == 1 then
			npcHandler:say("Perfect, now the last thing we need some red {paint} to protect the wood.", cid)
			player:setStorageValue(BoatMissions.MagnusBlackWaterFifth, 1)
		else
			npcHandler:say("You need 1 small golden anchor to keep the boat in place.", cid)
		end
	elseif msg:lower() == "paint" then
		if player:getStorageValue(BoatMissions.MagnusBlackWaterSixth) == 1 then
			npcHandler:say("You already have construct your boat.", cid)
		elseif player:removeItem(25308, 1) and player:getStorageValue(BoatMissions.MagnusBlackWaterFifth) == 1 then
			npcHandler:say("Fantastic! The boat is now ready, to start navigating, place the explorer to the boat slot on your inventory and walk through the deck that leads to the water.", cid)
			player:say("Get ready to explore the ocean. Equip the explorer on the Boat Slot of your inventory, and walk to the ocean through the deck that leads to the water.", TALKTYPE_MONSTER_SAY)
			player:setStorageValue(BoatMissions.MagnusBlackWaterSixth, 1)
			player:addItem(27799, 1)
			else
				npcHandler:say("You need 1 Red Paint to protect the wood.", cid)
			end
	end
	return true
end

npcHandler:setMessage(MESSAGE_GREET, "Psst.... Come here... You want {discover} the vast ocean and found treasures ?.")
npcHandler:setMessage(MESSAGE_FAREWELL, "Venture forth, my hearty crew! The boundless ocean awaits, where treasures lie hidden beneath each cresting wave. Hoist the sails high and let the winds of fortune guide our course to riches beyond measure!")
npcHandler:setMessage(MESSAGE_WALKAWAY, "Psst.... Come here... You want {discover} the vast ocean and found treasures ?")
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())