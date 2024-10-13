-- data/creaturescripts/scripts/shop.lua
local config = require('data.creaturescripts.scripts.shop.config')
local Callbacks = require('data.creaturescripts.scripts.shop.callbacks')
local Repository = require('data.creaturescripts.scripts.shop.repository')

local SHOP_OFFERS = {}
local SHOP_CALLBACKS = {}
local SHOP_CATEGORIES = nil

function init()

    SHOP_CATEGORIES = {}

    local category1 = addCategory({
        type = "item",
        item = ItemType(2160):getClientId(),
        count = 100,
        name = "Items"
    })

    local category2 = addCategory({
        type = "outfit",
        name = "Outfits",
        outfit = {
            mount = 0,
            feet = 114,
            legs = 114,
            body = 116,
            type = 143,
            auxType = 0,
            addons = 3,
            head = 2,
            rotating = true
        }
    })

    category1.addItem(90, 2160, 100, "100 Crystal coin", "description of cristal coin")
    category1.addItem(200, 2493, 1, "Demon helmet", "woo\ndemon helmet\nnice, you should buy it")
    category2.addOutfit(100, {
        mount = 682,
        feet = 0,
        legs = 0,
        body = 0,
        type = 143,
        auxType = 0,
        addons = 0,
        head = 0,
        rotating = true
    }, "Mount example")

end

function addCategory(data)
    data['offers'] = {}
    table.insert(SHOP_CATEGORIES, data)
    table.insert(SHOP_CALLBACKS, {})
    local index = #SHOP_CATEGORIES
    return {
        addItem = function(cost, itemId, count, title, description, callback)
            if not callback then
                callback = Callbacks.defaultItemBuyAction
            end
            table.insert(SHOP_CATEGORIES[index]['offers'], {
                cost = cost,
                type = "item",
                item = ItemType(itemId):getClientId(), -- displayed
                itemId = itemId,
                count = count,
                title = title,
                description = description
            })
            table.insert(SHOP_CALLBACKS[index], callback)
        end,
        addOutfit = function(cost, outfit, title, description, callback)
            if not callback then
                callback = Callbacks.defaultOutfitBuyAction
            end
            table.insert(SHOP_CATEGORIES[index]['offers'], {
                cost = cost,
                type = "outfit",
                outfit = outfit,
                title = title,
                description = description
            })
            table.insert(SHOP_CALLBACKS[index], callback)
        end,
        addImage = function(cost, image, title, description, callback)
            if not callback then
                callback = Callbacks.defaultImageBuyAction
            end
            table.insert(SHOP_CATEGORIES[index]['offers'], {
                cost = cost,
                type = "image",
                image = image,
                title = title,
                description = description
            })
            table.insert(SHOP_CALLBACKS[index], callback)
        end
    }
end

function getStatus(player)
    local status = {
        ad = config.AD,
        points = Repository.getPoints(player),
        buyUrl = config.BUY_URL
    }
    return status
end

function sendJSON(player, action, data, forceStatus)
    local status = nil
    if not player:getStorageValue(1150001) or player:getStorageValue(1150001) + 10 < os.time() or forceStatus then
        status = getStatus(player)
    end
    player:setStorageValue(1150001, os.time())

    local buffer = json.encode({
        action = action,
        data = data,
        status = status
    })
    local s = {}
    for i = 1, #buffer, config.MAX_PACKET_SIZE do
        s[#s + 1] = buffer:sub(i, i + config.MAX_PACKET_SIZE - 1)
    end
    local msg = NetworkMessage()
    if #s == 1 then
        msg:addByte(50)
        msg:addByte(config.EXTENDED_OPCODE)
        msg:addString(s[1])
        msg:sendToPlayer(player)
        return
    end
    -- split message if too big
    msg:addByte(50)
    msg:addByte(config.EXTENDED_OPCODE)
    msg:addString("S" .. s[1])
    msg:sendToPlayer(player)
    for i = 2, #s - 1 do
        msg = NetworkMessage()
        msg:addByte(50)
        msg:addByte(config.EXTENDED_OPCODE)
        msg:addString("P" .. s[i])
        msg:sendToPlayer(player)
    end
    msg = NetworkMessage()
    msg:addByte(50)
    msg:addByte(config.EXTENDED_OPCODE)
    msg:addString("E" .. s[#s])
    msg:sendToPlayer(player)
end

function sendMessage(player, title, msg, forceStatus)
    sendJSON(player, "message", {
        title = title,
        msg = msg
    }, forceStatus)
end

local Repository = require('data.creaturescripts.scripts.shop.repository')

function processBuy(player, data)
    local categoryId = tonumber(data["category"])
    local offerId = tonumber(data["offer"])
    local offer = SHOP_CATEGORIES[categoryId]['offers'][offerId]
    local callback = SHOP_CALLBACKS[categoryId][offerId]

    -- Validate the offer and callback
    if not offer or not callback or data["title"] ~= offer["title"] or data["cost"] ~= offer["cost"] then
        sendJSON(player, "categories", SHOP_CATEGORIES) -- refresh categories, maybe invalid
        return sendMessage(player, "Error!", "Invalid offer")
    end

    -- Check if player has enough points
    local points = Repository.getPoints(player)
    if not offer['cost'] or offer['cost'] > points or points < 1 then
        return sendMessage(player, "Error!", "You don't have enough points to buy " .. offer['title'] .. "!", true)
    end

    -- Process the purchase via callback
    local status = callback(player, offer)
    if status == true then
        Repository.updatePoints(player, offer['cost']) -- Deduct points
        Repository.logPurchase(player, offer) -- Log the purchase
        return sendMessage(player, "Success!", "You bought " .. offer['title'] .. "!", true)
    end

    -- Handle error cases
    if status == nil or status == false then
        status = "Unknown error while buying " .. offer['title']
    end
    sendMessage(player, "Error!", status)
end

function sendHistory(player)
    -- Check cooldown to avoid spam requests
    if player:getStorageValue(1150002) and player:getStorageValue(1150002) + 10 > os.time() then
        return
    end
    player:setStorageValue(1150002, os.time())

    -- Fetch purchase history from the repository
    local history = Repository.getPurchaseHistory(player)

    -- Send history as JSON response
    sendJSON(player, "history", history)
end

function onExtendedOpcode(player, opcode, buffer)
    if opcode ~= config.EXTENDED_OPCODE then
        return false
    end
    local status, json_data = pcall(function()
        return json.decode(buffer)
    end)
    if not status then
        return false
    end

    local action = json_data['action']
    local data = json_data['data']
    if not action or not data then
        return false
    end

    if SHOP_CATEGORIES == nil then
        init()
    end

    if action == 'init' then
        sendJSON(player, "categories", SHOP_CATEGORIES)
    elseif action == 'buy' then
        processBuy(player, data)
    elseif action == "history" then
        sendHistory(player)
    end
    return true
end
