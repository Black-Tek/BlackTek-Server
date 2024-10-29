local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
local talkState = {}
local rtnt = {}
function onCreatureAppear(cid)                npcHandler:onCreatureAppear(cid)             end
function onCreatureDisappear(cid)             npcHandler:onCreatureDisappear(cid)         end
function onCreatureSay(cid, type, msg)         npcHandler:onCreatureSay(cid, type, msg)     end
function onThink()                             npcHandler:onThink()                         end

npcHandler:setMessage(MESSAGE_GREET, "Greetings |PLAYERNAME|. If you have some precious nightmare coins to trade just say {addons} if you don't know what to do.")

addoninfo = {

---- Base Outfits + Addon 1 + Addon 2 -----
['base small angel outfit'] = {cost = 0, items = {{26779,10}}, outfit_female = 971, outfit_male = 972, addon = 0, storageID = 133031},
['first small angel addon'] = {cost = 0, items = {{26779,15}}, outfit_female = 971, outfit_male = 972, addon = 1, storageID = 133032},
['second small angel addon'] = {cost = 0, items = {{26779,15}}, outfit_female = 971, outfit_male = 972, addon = 2, storageID = 133033},
['base everblight outfit'] = {cost = 0, items = {{26779,15}}, outfit_female = 973, outfit_male = 974, addon = 0, storageID = 133034},
['first everblight addon'] = {cost = 0, items = {{26779,20}}, outfit_female = 973, outfit_male = 974, addon = 1, storageID = 133035},
['second everblight addon'] = {cost = 0, items = {{26779,25}}, outfit_female = 973, outfit_male = 974, addon = 2, storageID = 133036},
['base herald outfit'] = {cost = 0, items = {{26779,15}}, outfit_female = 985, outfit_male = 984, addon = 0, storageID = 133037},
['first herald addon'] = {cost = 0, items = {{26779,20}}, outfit_female = 985, outfit_male = 984, addon = 1, storageID = 133038},
['second herald addon'] = {cost = 0, items = {{26779,25}}, outfit_female = 985, outfit_male = 984, addon = 2, storageID = 133039},
['base battle mage outfit'] = {cost = 0, items = {{26779,15}}, outfit_female = 998, outfit_male = 997, addon = 0, storageID = 133040},
['first battle mage addon'] = {cost = 0, items = {{27189,5}}, outfit_female = 998, outfit_male = 997, addon = 1, storageID = 133041},
['second battle mage addon'] = {cost = 0, items = {{27184,20}}, outfit_female = 998, outfit_male = 997, addon = 2, storageID = 133042},
['base discoverer outfit'] = {cost = 0, items = {{26779,15}}, outfit_female = 1000, outfit_male = 999, addon = 0, storageID = 133043},
['first discoverer addon'] = {cost = 0, items = {{26779,20}}, outfit_female = 1000, outfit_male = 999, addon = 1, storageID = 133044},
['second discoverer addon'] = {cost = 0, items = {{26779,25}}, outfit_female = 1000, outfit_male = 999, addon = 2, storageID = 133045},
['base hand of inquisition outfit'] = {cost = 0, items = {{26779,15}}, outfit_female = 1015, outfit_male = 1014, addon = 0, storageID = 133046},
['first hand of inquisition addon'] = {cost = 0, items = {{27234,1}}, outfit_female = 1015, outfit_male = 1014, addon = 1, storageID = 133047},
['second hand of inquisition addon'] = {cost = 0, items = {{27235,2}}, outfit_female = 1015, outfit_male = 1014, addon = 2, storageID = 133048},
['base doom knight outfit'] = {cost = 0, items = {{26779,15}}, outfit_female = 1017, outfit_male = 1016, addon = 0, storageID = 133049},
['first doom knight addon'] = {cost = 0, items = {{26779,1}}, outfit_female = 1017, outfit_male = 1016, addon = 1, storageID = 133050},
['second doom knight addon'] = {cost = 0, items = {{26779,2}}, outfit_female = 1017, outfit_male = 1016, addon = 2, storageID = 133051},
['base poltergeist outfit'] = {cost = 0, items = {{26779,15}}, outfit_female = 1038, outfit_male = 1037, addon = 0, storageID = 133052},
['first poltergeist addon'] = {cost = 0, items = {{27297,1}}, outfit_female = 1038, outfit_male = 1037, addon = 1, storageID = 133053},
['second poltergeist addon'] = {cost = 0, items = {{27298,1}}, outfit_female = 1038, outfit_male = 1037, addon = 2, storageID = 133054},
['base dragon slayer outfit'] = {cost = 0, items = {{26779,15}}, outfit_female = 1040, outfit_male = 1039, addon = 0, storageID = 133055},
['first dragon slayer addon'] = {cost = 0, items = {{27297,1}}, outfit_female = 1040, outfit_male = 1039, addon = 1, storageID = 133056},
['second dragon slayer addon'] = {cost = 0, items = {{27298,1}}, outfit_female = 1040, outfit_male = 1039, addon = 2, storageID = 133057},
['base golden outfit'] = {cost = 0, items = {{26779,25}}, outfit_female = 1065, outfit_male = 1064, addon = 0, storageID = 133058},
['first golden addon'] = {cost = 0, items = {{26779,20}}, outfit_female = 1065, outfit_male = 1064, addon = 1, storageID = 133059},
['second golden addon'] = {cost = 0, items = {{26779,20}}, outfit_female = 1065, outfit_male = 1064, addon = 2, storageID = 133060},
['base revenant outfit'] = {cost = 0, items = {{26779,25}}, outfit_female = 1067, outfit_male = 1066, addon = 0, storageID = 133061},
['first revenant addon'] = {cost = 0, items = {{27347,1}}, outfit_female = 1067, outfit_male = 1066, addon = 1, storageID = 133062},
['second revenant addon'] = {cost = 0, items = {{27348,1}}, outfit_female = 1067, outfit_male = 1066, addon = 2, storageID = 133063},
['first astro mage addon'] = {cost = 0, items = {{28029,3}}, outfit_female = 1207, outfit_male = 1207, addon = 1, storageID = 133064},
['second astro mage addon'] = {cost = 0, items = {{28029,5}}, outfit_female = 1207, outfit_male = 1207, addon = 2, storageID = 133065},
['first astro warrior addon'] = {cost = 0, items = {{28029,3}}, outfit_female = 1208, outfit_male = 1208, addon = 1, storageID = 133066},
['second astro warrior addon'] = {cost = 0, items = {{28029,5}}, outfit_female = 1208, outfit_male = 1208, addon = 2, storageID = 133067},
['first arodis addon'] = {cost = 0, items = {{28054,3}}, outfit_female = 1229, outfit_male = 1228, addon = 1, storageID = 133068},
['second arodis addon'] = {cost = 0, items = {{28054,5}}, outfit_female = 1229, outfit_male = 1228, addon = 2, storageID = 133069},
['first corrupted priest addon'] = {cost = 0, items = {{28056,3}}, outfit_female = 1199, outfit_male = 1194, addon = 1, storageID = 133070},
['second corrupted priest addon'] = {cost = 0, items = {{28056,5}}, outfit_female = 1199, outfit_male = 1194, addon = 2, storageID = 133071},
['first corrupted archer addon'] = {cost = 0, items = {{28056,3}}, outfit_female = 1200, outfit_male = 1195, addon = 1, storageID = 133072},
['second corrupted archer addon'] = {cost = 0, items = {{28056,5}}, outfit_female = 1200, outfit_male = 1195, addon = 2, storageID = 133073},
['first corrupted axe addon'] = {cost = 0, items = {{28056,3}}, outfit_female = 1202, outfit_male = 1196, addon = 1, storageID = 133074},
['second corrupted axe addon'] = {cost = 0, items = {{28056,5}}, outfit_female = 1202, outfit_male = 1196, addon = 2, storageID = 133075},
['first corrupted club addon'] = {cost = 0, items = {{28056,3}}, outfit_female = 1203, outfit_male = 1197, addon = 1, storageID = 133076},
['second corrupted club addon'] = {cost = 0, items = {{28056,5}}, outfit_female = 1203, outfit_male = 1197, addon = 2, storageID = 133077},
['first corrupted sword addon'] = {cost = 0, items = {{28056,3}}, outfit_female = 1204, outfit_male = 1198, addon = 1, storageID = 133078},
['second corrupted sword addon'] = {cost = 0, items = {{28056,5}}, outfit_female = 1204, outfit_male = 1198, addon = 2, storageID = 133079},
['first corrupted mage addon'] = {cost = 0, items = {{28056,3}}, outfit_female = 1201, outfit_male = 1224, addon = 1, storageID = 133080},
['second corrupted mage addon'] = {cost = 0, items = {{28056,5}}, outfit_female = 1201, outfit_male = 1224, addon = 2, storageID = 133081},
['base rootkraken outfit'] = {cost = 0, items = {{26779,25}}, outfit_female = 1263, outfit_male = 1262, addon = 0, storageID = 133082},
['first rootkraken addon'] = {cost = 0, items = {{28144,1}}, outfit_female = 1263, outfit_male = 1262, addon = 1, storageID = 133083},
['second rootkraken addon'] = {cost = 0, items = {{28259,1}}, outfit_female = 1263, outfit_male = 1262, addon = 2, storageID = 133084},
['base honey outfit'] = {cost = 0, items = {{28697,25}}, outfit_female = 1300, outfit_male = 1299, addon = 0, storageID = 133085},
['first honey addon'] = {cost = 0, items = {{28696,1}}, outfit_female = 1300, outfit_male = 1299, addon = 1, storageID = 133086},
['second honey addon'] = {cost = 0, items = {{28696,1}}, outfit_female = 1300, outfit_male = 1299, addon = 2, storageID = 133087},


}
local o = {'small angel', 'everblight', 'herald', 'battle mage', 'discoverer', 'hand of inquisition', 'doom knight', 'poltergeist', 'dragon slayer', 'golden', 'revenant', 'astro warrior', 'astro mage', 'arodis', 'corrupted priest/archer/axe/club/sword/mage' }
function creatureSayCallback(cid, type, msg)
local talkUser = cid

    if(not npcHandler:isFocused(cid)) then
        return false
    end

    if addoninfo[msg] ~= nil then
        if (getPlayerStorageValue(cid, addoninfo[msg].storageID) ~= -1) then
                npcHandler:say('You already have this addon!', cid)
                npcHandler:resetNpc()
        else
        local itemsTable = addoninfo[msg].items
        local items_list = ''
            if table.maxn(itemsTable) > 0 then
                for i = 1, table.maxn(itemsTable) do
                    local item = itemsTable[i]
                    items_list = items_list .. item[2] .. ' ' .. ItemType(item[1]):getName()
                    if i ~= table.maxn(itemsTable) then
                        items_list = items_list .. ', '
                    end
                end
            end
        local text = ''
            if (addoninfo[msg].cost > 0) then
                text = addoninfo[msg].cost .. ' gp'
            elseif table.maxn(addoninfo[msg].items) then
                text = items_list
            elseif (addoninfo[msg].cost > 0) and table.maxn(addoninfo[msg].items) then
                text = items_list .. ' and ' .. addoninfo[msg].cost .. ' gp'
            end
            npcHandler:say('For ' .. msg .. ' you will need ' .. text .. '. Do you have it all with you?', cid)
            rtnt[talkUser] = msg
            talkState[talkUser] = addoninfo[msg].storageID
            return true
        end
    elseif msgcontains(msg, "yes") then
        if (talkState[talkUser] > 133030 and talkState[talkUser] < 133237) then
            local items_number = 0
            if table.maxn(addoninfo[rtnt[talkUser]].items) > 0 then
                for i = 1, table.maxn(addoninfo[rtnt[talkUser]].items) do
                    local item = addoninfo[rtnt[talkUser]].items[i]
                    if (getPlayerItemCount(cid,item[1]) >= item[2]) then
                        items_number = items_number + 1
                    end
                end
            end
            if(getPlayerMoney(cid) >= addoninfo[rtnt[talkUser]].cost) and (items_number == table.maxn(addoninfo[rtnt[talkUser]].items)) then
                PlayerRemoveMoney(cid, addoninfo[rtnt[talkUser]].cost)
                if table.maxn(addoninfo[rtnt[talkUser]].items) > 0 then
                    for i = 1, table.maxn(addoninfo[rtnt[talkUser]].items) do
                        local item = addoninfo[rtnt[talkUser]].items[i]
                        PlayerRemoveItem(cid,item[1],item[2])
                    end
                end
                PlayerAddOutfit(cid, addoninfo[rtnt[talkUser]].outfit_male, addoninfo[rtnt[talkUser]].addon)
                PlayerAddOutfit(cid, addoninfo[rtnt[talkUser]].outfit_female, addoninfo[rtnt[talkUser]].addon)
                PlayerAddMount(cid, addoninfo[rtnt[talkUser]].mount_id)
                PlayerSetStorageValue(cid,addoninfo[rtnt[talkUser]].storageID,1)
                npcHandler:say('Here you are.', cid)
            else
                npcHandler:say('You do not have needed items!', cid)
            end
            rtnt[talkUser] = nil
            talkState[talkUser] = 0
            npcHandler:resetNpc()
            return true
        end
    elseif msgcontains(msg, "addon") then
        npcHandler:say('I can give you base outfit (for few outfits) and addons for {' .. table.concat(o, "}, {") .. '} outfits. Say {help} if you need.', cid)
        rtnt[talkUser] = nil
        talkState[talkUser] = 0
        npcHandler:resetNpc()
        return true
    elseif msgcontains(msg, "help") then
        npcHandler:say('To buy Outfits: the base outfit say \'base NAME outfit\', for first addon say \'first NAME addon\', for the second addon say \'second NAME addon\'.', cid)
        rtnt[talkUser] = nil
        talkState[talkUser] = 0
        npcHandler:resetNpc()
        return true
    else
        if talkState[talkUser] ~= nil then
            if talkState[talkUser] > 0 then
            npcHandler:say('Come back when you get nightmare coins.', cid)
            rtnt[talkUser] = nil
            talkState[talkUser] = 0
            npcHandler:resetNpc()
            return true
            end
        end
    end
    return true
end

npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())