local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)
local talkState = {}

function onCreatureAppear(cid)
	npcHandler:onCreatureAppear(cid)
end
function onCreatureDisappear(cid)
	npcHandler:onCreatureDisappear(cid)
end
function onCreatureSay(cid, type, msg)
	npcHandler:onCreatureSay(cid, type, msg)
end
function onThink()
	npcHandler:onThink()
end

local shop = {
	{id=27985, buy=100, sell=0, name='easter egg basket'},
	{id=27988, buy=100, sell=0, name='magical easter egg'},
	{id=27989, buy=100, sell=0, name='easter egg basket'},
}

local function setNewTradeTable(table)
	local items, item = {}
	for i = 1, #table do
		item = table[i]
		items[item.id] = {id = item.id, buy = item.buy, sell = item.sell, subType = 0, name = item.name}
	end
	return items
end

local function onBuy(cid, item, subType, amount, ignoreCap, inBackpacks)
	local player = Player(cid)
	local itemsTable = setNewTradeTable(shop)
	if not ignoreCap and player:getFreeCapacity() < ItemType(itemsTable[item].id):getWeight(amount) then
		return player:sendTextMessage(MESSAGE_INFO_DESCR, "You don't have enough cap.")
	end
	if itemsTable[item].buy then
		if player:removeItem(27987, amount * itemsTable[item].buy) then
			if amount > 1 then
				currencyName = ItemType(27987):getPluralName():lower()
			else
				currencyName = ItemType(27987):getName():lower()
			end
			player:addItem(itemsTable[item].id, amount)
			return player:sendTextMessage(MESSAGE_INFO_DESCR,
						"Bought "..amount.."x "..itemsTable[item].name.." for "..itemsTable[item].buy * amount.." "..currencyName..".")
		else
			return player:sendTextMessage(MESSAGE_INFO_DESCR, "You don't have enough Easter Egg's.")
		end
	end

	return true
end

local function onSell(cid, item, subType, amount, ignoreCap, inBackpacks)
	return true
end


local function greetCallback(cid)
    return true
end

local voices = {
	{ text = 'Trading Easter Eggs!' },
	{ text = 'Get your Celestial Avenger Outfit or Pegasus Mount!' },
	{ text = 'Easter Eggs, i want more Easter Eggs!' }
}

npcHandler:addModule(VoiceModule:new(voices))

function creatureSayCallback(cid, type, msg)
	if not npcHandler:isFocused(cid) then
		return false
	end
	local player = Player(cid)
	if not player then
		return false
	end
	if msgcontains(msg, 'easter') then
		npcHandler:say("I can exchange Easter Egg Basket, Magical Easter Egg or Easter Egg Nest for some Easter Eggs. Just tell me {trade} or if you are looking for the {outfit} and {addon}, or maybe you are looking for the new {mount}?", cid)
	elseif msgcontains(msg, 'trade') then
		openShopWindow(cid, shop, onBuy, onSell)
		npcHandler:say({"Here's the deal, " .. player:getName() .. ". For some Easter eggs i can exchange some easter items that you can bought after the Celestial Avenger Outfit or Pegasus Mount"}, cid)
	elseif msgcontains(msg, 'outfit') then
		if not(player:hasOutfit(1188, 0)) and not(player:hasOutfit(1189, 0)) then
			if player:removeItem(27988, 50) then
				npcHandler:say("Excellent, here you are the Celestial Avenger Outfit.", cid)
				player:addOutfitAddon(1188, 0)
				player:addOutfitAddon(1189, 0)
			else
				npcHandler:say("Sorry you don't enough ".. ItemType(27988):getPluralName():lower() .." to get the Celestial Avenger Outfit.", cid)
			end
		else
			npcHandler:say("You already have the Celestial Avenger Outfit.", cid)
		end
	elseif msgcontains(msg, 'addon') then
		if player:hasOutfit(1188, 0) or player:hasOutfit(1189, 0) then
			npcHandler:say("Ah, very good. Now choose your addon for celestial avenger outfit: {first} or {second}.", cid)
			npcHandler.topic[cid] = 3
		else
			npcHandler:say("Sorry, friend, but one good turn deserves another. You need to obtain the celestial avenger outfit first.", cid)
		end
	elseif isInArray({'first', 'second'}, msg:lower()) and npcHandler.topic[cid] == 3 then
		if msg:lower() == 'first' then
			if not(player:hasOutfit(1188, 1)) and not(player:hasOutfit(1189, 1)) then
				if player:removeItem(27985, 25) then
					npcHandler:say("Ah, excellent. Obtain the first addon for your celestial avenger outfit.", cid)
					player:addOutfitAddon(1188, 1)
					player:addOutfitAddon(1189, 1)
				else
					npcHandler:say("Sorry, friend, but one good turn deserves another. Bring enough ".. ItemType(27985):getPluralName():lower() .." and it's a deal.", cid)
				end
			else
				npcHandler:say("Sorry, friend, you already have the first Celestial Avenger addon.", cid)
			end
		elseif msg:lower() == 'second' then
			if not(player:hasOutfit(1188, 2)) and not(player:hasOutfit(1189, 2)) then
				if player:removeItem(27989, 25) then
					npcHandler:say("Ah, excellent. Obtain the second addon for your celestial avenger outfit.", cid)
					player:addOutfitAddon(1188, 2)
					player:addOutfitAddon(1189, 2)
				else
					npcHandler:say("Sorry, friend, but one good turn deserves another. Bring enough ".. ItemType(27989):getPluralName():lower() .." and it's a deal.", cid)
				end
			else
				npcHandler:say("Sorry, friend, you already have the second Celestial Avenger addon.", cid)
			end
		end
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'mount') then
			npcHandler:say("You can choose the mount: {Dawnbringer} Pegasus, {Skybreaker} Pegasus or {Wrathfire} Pegasus.", cid)
		elseif msgcontains(msg, 'dawnbringer') then
			if not(player:hasMount(114)) then
				if player:removeItem(27988, 30) then
					npcHandler:say("Excellent, here you are the Dawnbringer pegasus Mount.", cid)
					player:addMount(114)
				else
					npcHandler:say("Sorry you don't enough ".. ItemType(27988):getPluralName():lower() .." to get the Dawnbringer Pegasus mount.", cid)
				end
			else
				npcHandler:say("You already have the Dawnbringer Pegasus Mount.", cid)
			end
		elseif msgcontains(msg, 'skybreaker') then
			if not(player:hasMount(115)) then
				if player:removeItem(27988, 30) then
					npcHandler:say("Excellent, here you are the Skybreaker pegasus Mount.", cid)
					player:addMount(115)
				else
					npcHandler:say("Sorry you don't enough ".. ItemType(27988):getPluralName():lower() .." to get the Skybreaker Pegasus mount.", cid)
				end
			else
				npcHandler:say("You already have the Skybreaker Pegasus Mount.", cid)
			end
		elseif msgcontains(msg, 'wrathfire') then
			if not(player:hasMount(116)) then
				if player:removeItem(27988, 30) then
					npcHandler:say("Excellent, here you are the Wrathfire pegasus Mount.", cid)
					player:addMount(116)
				else
					npcHandler:say("Sorry you don't enough ".. ItemType(27988):getPluralName():lower() .." to get the Wrathfire Pegasus mount.", cid)
				end
			else
				npcHandler:say("You already have the Wrathfire Pegasus Mount.", cid)
			end
	end
	return true
end

npcHandler:setCallback(CALLBACK_GREET, greetCallback)
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())
