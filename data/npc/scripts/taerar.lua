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
	{id=26756, buy=150, sell=75, name='frozen wand'},
	{id=26757, buy=150, sell=75, name='venom rod'},
	{id=26758, buy=150, sell=75, name='lazering bow'},
	{id=26759, buy=150, sell=75, name='seetheras sword'},
	{id=26760, buy=150, sell=75, name='varruth sword'},
	{id=26761, buy=100, sell=50, name='netherwave helmet'},
	{id=26762, buy=100, sell=50, name='netherwave armor'},
	{id=26763, buy=100, sell=50, name='netherwave legs'},
	{id=26764, buy=100, sell=50, name='netherwave boots'},
	{id=26765, buy=150, sell=75, name='exowing bow'},
	{id=26766, buy=100, sell=50, name='varruth helmet'},
	{id=26767, buy=100, sell=50, name='varruth armor'},
	{id=26768, buy=100, sell=50, name='varruth legs'},
	{id=26769, buy=100, sell=50, name='varruth boots'},
	{id=26770, buy=100, sell=50, name='seetheras helmet'},
	{id=26771, buy=100, sell=50, name='seetheras armor'},
	{id=26772, buy=100, sell=50, name='seetheras legs'},
	{id=26773, buy=100, sell=50, name='seetheras boots'},
	{id=26774, buy=100, sell=50, name='felsinger helmet'},
	{id=26775, buy=100, sell=50, name='felsinger armor'},
    {id=26776, buy=100, sell=50, name='felsinger legs'},
    {id=26777, buy=100, sell=50, name='felsinger boots'},
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
		if player:removeItem(26780, amount * itemsTable[item].buy) then
			if amount > 1 then
				currencyName = ItemType(26780):getPluralName():lower()
			else
				currencyName = ItemType(26780):getName():lower()
			end
			player:addItem(itemsTable[item].id, amount)
			return player:sendTextMessage(MESSAGE_INFO_DESCR,
						"Bought "..amount.."x "..itemsTable[item].name.." for "..itemsTable[item].buy * amount.." "..currencyName..".")
		else
			return player:sendTextMessage(MESSAGE_INFO_DESCR, "You don't have enough Azure Coins.")
		end
	end

	return true
end

local function onSell(cid, item, subType, amount, ignoreCap, inBackpacks)
	local player = Player(cid)
	local itemsTable = setNewTradeTable(shop)
	if not ignoreCap and player:getFreeCapacity() < ItemType(itemsTable[item].id):getWeight(amount) then
		return player:sendTextMessage(MESSAGE_FAILURE, "You don't have enough cap.")
		end
		if player:removeItem(item, amount, 1, amount* itemsTable[item].sell) then
			end
		if player:addItem(26780, amount* itemsTable[item].sell, amount* itemsTable[item].sell) then
			return player:sendTextMessage(MESSAGE_TRADE,
						"Sell "..amount.."x "..itemsTable[item].name.." for "..amount* itemsTable[item].sell.." Arkeron Coins.")
end
return true
end


local function greetCallback(cid)
    return true
end

local voices = {
	{ text = 'New Stuffs: Netherwave, Varruth, Seetheras and Felsinger!' },
	{ text = 'Warning, do not go to far on the cave, something powerful is hide.' },
	{ text = 'Azure coins!' }
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
	if msgcontains(msg, 'trade') or msgcontains(msg, 'exchange') then
		npcHandler:say("If you have any {azure} coin with you, let's have a look! Maybe I can offer you something in exchange.", cid)
	elseif msgcontains(msg, 'azure') then
		openShopWindow(cid, shop, onBuy, onSell)
		npcHandler:say({"Stuff for all vocations, Seetheras and varruth for knights, netherwave, for sorcerers and druids and felsinger for paladins."}, cid)
	end
	return true
end

npcHandler:setCallback(CALLBACK_GREET, greetCallback)
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())
