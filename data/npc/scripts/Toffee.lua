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
	{id=28686, buy=3, sell=0, name='gelling'},
	{id=28687, buy=3, sell=0, name='sugar candy'},
	{id=28695, buy=1, sell=0, name='gummy rotworm'},
	{id=28696, buy=3, sell=0, name='milk chocolate coin'},
	{id=28698, buy=1, sell=0, name='wafer paper flower'},
	{id=28699, buy=2, sell=0, name='brigadeiro'},
	{id=28700, buy=1, sell=0, name='beijinho'},
	{id=28701, buy=2, sell=0, name='churro heart'},
	{id=28702, buy=1, sell=0, name='pastry dragon'},
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
	local itemTypeID = 28697  

	if not ignoreCap and player:getFreeCapacity() < ItemType(itemsTable[item].id):getWeight(amount) then
		return player:sendTextMessage(MESSAGE_INFO_DESCR, "You don't have enough capacity.")
	end

	local currencyName = ItemType(itemTypeID):getPluralName():lower()  

	if itemsTable[item].buy then
		if player:removeItem(itemTypeID, amount * itemsTable[item].buy) then
			player:addItem(itemsTable[item].id, amount)
			return player:sendTextMessage(MESSAGE_TRADE,
						"Bought "..amount.."x "..itemsTable[item].name.." for "..itemsTable[item].buy * amount.." "..currencyName..".")
		else
			return player:sendTextMessage(MESSAGE_INFO_DESCR, "You don't have enough "..currencyName..".")
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
	{ text = 'We want all your chocolate!' },
	{ text = 'Sugar and Chocolate, mmm delicious pastry!' },
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
	if msgcontains(msg, 'dark chocolate') or msgcontains(msg, 'dark chocolate coins') then
		openShopWindow(cid, shop, onBuy, onSell)
		npcHandler:say("For some {dark chocolate coins} i will give you some delicious treats. If you are looking for the outfit or the mount just ask, {outfit} or {honey bee}.", cid)
	elseif msgcontains(msg, 'honey bee') then
		npcHandler:say("Honey Bee mount can be obtained using 100 gellings and 100 sugar candy directly from your backpack!", cid)
	elseif msgcontains(msg, 'outfit') then
		npcHandler:say("Honey outfit, talk with estherya it will give you the answer about the outfit!", cid)
	end
	return true
end

npcHandler:setCallback(CALLBACK_GREET, greetCallback)
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())
